#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(Program* root, const std::string& file):
    root(root), file(file), currentStackOffset(0) {}

void CodeGenerator::generateLine(const std::string& text)
{

    file << text << std::endl;

}

void CodeGenerator::generate()
{

    generateLine("section .text");
    generateLine("global _start");
    generateLine("extern exit");
    generateLine("");
    generateProgram(root);

    generateLine("_start:");
    generateLine("call main");
    generateLine("mov rdi, 0");
    generateLine("call exit");

}

void CodeGenerator::generateProgram(Program* program)
{

    for (auto& currentFunction : program->functions)
        generateFunction(currentFunction.get());

}

void CodeGenerator::generateFunction(FunctionDeclaration* functionDeclaration)
{

    localOffsets.clear();
    currentStackOffset = 0;

    generateLine(functionDeclaration->name + ":");
    generateLine("push rbp");
    generateLine("mov rbp, rsp");

    for (auto& currentParam : functionDeclaration->parameters)
        generateLine("; TODO!!!");

    generateStatement(functionDeclaration->body.get());

    generateLine("mov rsp, rbp");
    generateLine("pop rbp");
    generateLine("ret");
    generateLine("");

}

std::stack<std::string> breakLabels;

void CodeGenerator::generateStatement(Statement* statement)
{

    switch (statement->type) 
    {

    case NodeType::VARIABLE_DECLARATION:
    {

        auto variableDeclaration = (VariableDeclaration*)(statement);

        if (variableDeclaration->init)
        {

            generateExpression(variableDeclaration->init.get());
            currentStackOffset += 8;
            localOffsets[variableDeclaration->name] = currentStackOffset;
            generateLine("sub rsp, 8");
            generateLine("mov [rbp - " + std::to_string(currentStackOffset) + "], rax ; variable " + variableDeclaration->name);

        }

        break;

    }
    case NodeType::RETURN_STATEMENT:
    {

        auto returnStatement = (ReturnStatement*)(statement);

        if (returnStatement->value)
            generateExpression(returnStatement->value.get());

        generateLine("jmp .return");
        break;

    }
    case NodeType::EXPRESSION_STATEMENT:
    {

        auto expressionStatement = (ExpressionStatement*)(statement);
        generateExpression(expressionStatement->expression.get());

        break;

    }
    case NodeType::BLOCK_STATEMENT:
    {

        auto blockStatement = (BlockStatement*)(statement);

        for (auto& currentStatement : blockStatement->statements)
            generateStatement(currentStatement.get());

        break;

    }
    case NodeType::BREAK_STATEMENT:
    {
        if (!breakLabels.empty())
            generateLine("jmp " + breakLabels.top());

        break;

    }
    case NodeType::WHILE_STATEMENT:
    {

        auto whileStatement = (WhileStatement*)(statement);
        static int loopCounter = 0;
        int currentId = loopCounter;
        loopCounter += 1;

        std::string conditionLabel = ".Lcond_while_" + std::to_string(currentId);
        std::string bodyLabel = ".Lbody_while_" + std::to_string(currentId);
        std::string endLabel = ".Lend_while_" + std::to_string(currentId);

        breakLabels.push(endLabel);

        generateLine("jmp " + conditionLabel);
        generateLine(bodyLabel + ":");
        generateStatement(whileStatement->body.get());
        generateLine(conditionLabel + ":");
        generateExpression(whileStatement->condition.get());
        generateLine("cmp rax, 0");
        generateLine("jne " + bodyLabel);
        generateLine(endLabel + ":");

        breakLabels.pop();
        break;

    }
    case NodeType::FOR_STATEMENT:
    {

        auto forStatement = (ForStatement*)(statement);
        static int loopCounter = 0;
        int currentId = loopCounter;
        loopCounter += 1;

        std::string conditionLabel = ".Lcond_for_" + std::to_string(currentId);
        std::string bodyLabel = ".Lbody_for_" + std::to_string(currentId);
        std::string endLabel = ".Lend_for_" + std::to_string(currentId);

        breakLabels.push(endLabel);

        if (forStatement->init)
            generateStatement(forStatement->init.get());

        generateLine("jmp " + conditionLabel);
        generateLine(bodyLabel + ":");
        generateStatement(forStatement->body.get());

        if (forStatement->post)
            generateExpression(forStatement->post.get());

        generateLine(conditionLabel + ":");

        if (forStatement->condition)
        {

            generateExpression(forStatement->condition.get());
            generateLine("cmp rax, 0");
            generateLine("jne " + bodyLabel);

        }
        else
        {

            generateLine("jmp " + bodyLabel);

        }

        generateLine(endLabel + ":");

        breakLabels.pop();
        break;

    }
    case NodeType::IF_STATEMENT:
    {

        auto ifStatement = (IfStatement*)(statement);
        static int ifCounter = 0;
        int currentId = ifCounter;
        ifCounter += 1;

        std::string elseLabel = ".Lelse_" + std::to_string(currentId);
        std::string endLabel = ".Lend_if_" + std::to_string(currentId);

        generateExpression(ifStatement->condition.get());
        generateLine("cmp rax, 0");
        generateLine("je " + elseLabel);
        generateStatement(ifStatement->thenConsequence.get());
        generateLine("jmp " + endLabel);
        generateLine(elseLabel + ":");

        if (ifStatement->elseConsequence)
            generateStatement(ifStatement->elseConsequence.get());

        generateLine(endLabel + ":");
        break;

    }
    case NodeType::CHOOSE_STATEMENT:
    {

        auto chooseStatement = (ChooseStatement*)(statement);
        static int chooseCounter = 0;
        int currentId = chooseCounter;
        chooseCounter += 1;

        std::string endLabel = ".Lend_choose_" + std::to_string(currentId);

        generateExpression(chooseStatement->expression.get());
        generateLine("push rax ; value to match");

        for (int i = 0; i < chooseStatement->cases.size(); i++)
        {

            std::string caseLabel = ".Lcase_" + std::to_string(currentId) + "_" + std::to_string(i);
            generateExpression(chooseStatement->cases[i].test.get());
            generateLine("pop rcx");
            generateLine("push rcx");
            generateLine("cmp rax, rcx");
            generateLine("je " + caseLabel);

        }

        if (chooseStatement->defaultCase) 
        {

            generateLine("; default case");
            generateStatement(chooseStatement->defaultCase.get());
            generateLine("jmp " + endLabel);

        }

        for (int i = 0; i < chooseStatement->cases.size(); i++)
        {

            std::string caseLabel = ".Lcase_" + std::to_string(currentId) + "_" + std::to_string(i);
            generateLine(caseLabel + ":");
            generateStatement(chooseStatement->cases[i].body.get());
            generateLine("jmp " + endLabel);

        }

        generateLine(endLabel + ":");
        break;

    }
    default:  generateLine("; TODO!!!"); break;


    }

}

void CodeGenerator::generateExpression(Expression* expression)
{

    switch (expression->type)
    {

    case NodeType::INT_LITERAL:
    {

        auto intLiteral = (IntegerLitExpression*)(expression);
        generateLine("mov rax, " + intLiteral->text);

        break;

    }
    case NodeType::FLOAT_LITERAL:
    {

        auto floatLiteral = (FloatLitExpression*)(expression);
        generateLine("movsd xmm0, [rel .LC_float_" + floatLiteral->text + "]");

        break;
    }
    case NodeType::BOOL_LITERAL:
    {

        auto boolLiteral = (BoolLitExpression*)(expression);
        generateLine("mov rax, " + std::to_string(boolLiteral->value));

        break;

    }
    case NodeType::CHAR_LITERAL:
    {

        auto charLiteral = (CharLitExpression*)(expression);

        if (!charLiteral->text.empty() && charLiteral->text.length() >= 3)
        {

            char value = charLiteral->text[1];
            generateLine("mov rax, " + std::to_string(static_cast<int>(value)));

        }
        else 
        {

            generateLine("mov rax, 0");

        }

        break;

    }
    case NodeType::IDENTIFIER:
    {

        auto id = (IdentifierExpression*)(expression);
        int offset = localOffsets[id->name];
        generateLine("mov rax, [rbp - " + std::to_string(offset) + "] ; load " + id->name);

        break;

    }
    case NodeType::BINARY_EXPRESSION:
    {

        auto binaryExpression = (BinaryExpression*)(expression);
        generateExpression(binaryExpression->right.get());
        generateLine("push rax");
        generateExpression(binaryExpression->left.get());
        generateLine("pop rcx");

        switch (binaryExpression->oper)
        {

        case TokenType::PLUS: generateLine("add rax, rcx"); break;
        case TokenType::MINUS: generateLine("sub rax, rcx"); break;
        case TokenType::STAR: generateLine("imul rax, rcx"); break;
        case TokenType::SLASH: generateLine("cqo"); generateLine("idiv rcx"); break;
        default: break;

        }

        break;

    }
    case NodeType::UNARY_EXPRESSION:
    {

        auto unaryExpression = (UnaryExpression*)(expression);
        generateExpression(unaryExpression->operand.get());

        if (unaryExpression->oper == TokenType::NEGATION)
            generateLine("not rax");

        break;

    }
    case NodeType::CALL_EXPRESSION:
    {

        auto callExpression = (CallExpression*)(expression);

        for (auto currentArgument = callExpression->arguments.rbegin(); 
            currentArgument != callExpression->arguments.rend(); currentArgument++)
        {

            generateExpression(currentArgument->get());
            generateLine("push rax");

        }

        generateLine("call " + callExpression->callee);
        generateLine("add rsp, " + std::to_string(callExpression->arguments.size() * 8));

        break;

    }
    case NodeType::INDEX_EXPRESSION:
    {

        auto indexExpression = (IndexExpression*)(expression);

        generateExpression(indexExpression->base.get());
        generateLine("push rax");
        generateExpression(indexExpression->index.get());
        generateLine("pop rbx");
        generateLine("imul rax, 16");
        generateLine("add rbx, 8");
        generateLine("add rbx, rax");
        generateLine("add rbx, 8");
        generateLine("mov rax, [rbx]");

        break;

    }
    case NodeType::BOX_LITERAL:
    {

        auto boxLiteral = (BoxLiteral*)(expression);
        int elementCount = (int)(boxLiteral->elements.size());

        generateLine("; allocate space for box with " + std::to_string(elementCount) + " elements");
        generateLine("mov rdi, " + std::to_string(8 + elementCount * (8 + 8)));
        generateLine("call malloc");
        generateLine("mov rbx, rax ; rbx = start of box");
        generateLine("mov [rbx], " + std::to_string(elementCount));
        generateLine("add rbx, 8");

        for (int i = 0; i < boxLiteral->elements.size(); i++)
        {

            generateLine("mov qword [rbx], 0 ; TYPE_INT for now");
            generateLine("add rbx, 8");
            generateExpression(boxLiteral->elements[i].get());
            generateLine("mov [rbx], rax");
            generateLine("add rbx, 8");

        }

        generateLine("sub rbx, " + std::to_string(elementCount * (8 + 8)));
        generateLine("sub rbx, 8");
        generateLine("mov rax, rbx ; return address of box");

        break;

    }
    default: generateLine("; TODO!!!"); break;
        
    } 

}

