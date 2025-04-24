#include "CodeGenerator.h"

const char* CodeGenerator::typeTag(NodeType type)
{

    switch (type)
    {

    case NodeType::INT_LITERAL: return "0"; 
    case NodeType::FLOAT_LITERAL: return "1"; 
    case NodeType::BOOL_LITERAL: return "2"; 
    case NodeType::BOX_LITERAL: return "3"; 
    default: return "0";

    }

}

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
    generateLine("extern malloc");
    generateLine("extern printf");
    generateLine("extern fflush");
    generateLine("");

    generateProgram(root);

    generateLine("_start:");
    generateLine("call main");
    generateLine("mov rdi, 0");
    generateLine("call exit");

    if (!floatPool.empty())
    {

        generateLine("");
        generateLine("section .rodata");

        for (auto& kv : floatPool)               
            generateLine(kv.first + ": dq " + kv.second);

    }

}

void CodeGenerator::generateProgram(Program* program)
{

    for (auto& currentFunction : program->functions)
        generateFunction(currentFunction.get());

}

void CodeGenerator::generateExpression(Expression* expression)
{

    switch (expression->type)
    {
        
    case NodeType::INT_LITERAL: 
    {

        auto* intLiteral = (IntegerLitExpression*)expression;
        generateLine("mov rax, " + intLiteral->text);

        break;

    }
    case NodeType::FLOAT_LITERAL:
    {

        auto* floatLiteral = (FloatLitExpression*)expression;
        std::string lbl = ".LC_float_" + floatLiteral->text;
        floatPool.emplace(lbl, floatLiteral->text);
        generateLine("movsd xmm0, [rel " + lbl + "]");
        generateLine("movq rax, xmm0"); 
        
        break;

    }
    case NodeType::BOOL_LITERAL: 
    {

        auto* boolLiteral = (BoolLitExpression*)expression;
        generateLine("mov rax, " + std::to_string(boolLiteral->value));

        break;

    }
    case NodeType::CHAR_LITERAL: 
    {

        auto* charLiteral = (CharLitExpression*)expression; 
        char variable = (charLiteral->text.size() >= 3) ? charLiteral->text[1] : 0; 
        generateLine("mov rax," + std::to_string((int)variable)); 

        break;

    }  
    case NodeType::BOX_LITERAL:
    {

        auto* boxLiteral = (BoxLiteral*)expression;
        int size = (int)boxLiteral->elements.size();

        generateLine("; allocate space for box with " + std::to_string(size) + " elements");
        generateLine("mov rdi, " + std::to_string(8 + size * 16));
        generateLine("call malloc");
        generateLine("mov rbx, rax");
        generateLine("mov qword [rbx], " + std::to_string(size));
        generateLine("add rbx, 8"); 

        for (auto& currentElement : boxLiteral->elements)
        {

            generateLine("mov qword [rbx], " + std::string(typeTag(currentElement->type)));
            generateLine("add rbx, 8");
            generateExpression(currentElement.get());
            generateLine("mov [rbx], rax");
            generateLine("add rbx, 8");

        }

        generateLine("sub rbx, " + std::to_string(size * 16));
        generateLine("sub rbx, 8");
        generateLine("mov rax, rbx");

        break;

    }
    case NodeType::IDENTIFIER: 
    {

        auto* id = (IdentifierExpression*)expression;
        int off = localOffsets[id->name];
        generateLine("mov rax, [rbp-" + std::to_string(off) + "] ; " + id->name);

        break;

    }
    case NodeType::UNARY_EXPRESSION: 
    {

        auto* unaryExpression = (UnaryExpression*)expression;
        generateExpression(unaryExpression->operand.get());

        if (unaryExpression->oper == TokenType::NEGATION) 
        {        

            generateLine("cmp rax, 0");
            generateLine("sete al");
            generateLine("movzx rax, al");

        }
        else if (unaryExpression->oper == TokenType::MINUS) 
        {           

            generateLine("neg rax");

        }

        break;

    }                            
    case NodeType::BINARY_EXPRESSION: 
    {

        auto* binaryExpression = (BinaryExpression*)expression;

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
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL: 
        {

            generateLine("cmp rax, rcx");

            const char* setInstruction =
                (binaryExpression->oper == TokenType::EQUAL_EQUAL) ? "sete" :
                (binaryExpression->oper == TokenType::NOT_EQUAL) ? "setne" :
                (binaryExpression->oper == TokenType::LESS) ? "setl" :
                (binaryExpression->oper == TokenType::LESS_EQUAL) ? "setle" :
                (binaryExpression->oper == TokenType::GREATER) ? "setg" :
                "setge";            
            generateLine(std::string(setInstruction) + " al");
            generateLine("movzx rax, al");

            break;

        }
        case TokenType::LOGICAL_AND: 
        {

            generateLine("and rax, rcx");
            generateLine("cmp rax, 0");
            generateLine("setne al");
            generateLine("movzx rax, al");

            break;

        } 
        case TokenType::LOGICAL_OR:
        {

            generateLine("or rax, rcx");
            generateLine("cmp rax, 0");
            generateLine("setne al");
            generateLine("movzx rax, al");

            break;

        }
        default: break;

        }

        break;

    }
    case NodeType::CALL_EXPRESSION: 
    {

        auto* callExpression = (CallExpression*)expression;

        for (auto iter = callExpression->arguments.rbegin(); iter != callExpression->arguments.rend(); iter++)
        {

            generateExpression(iter->get());
            generateLine("push rax");

        }

        if (callExpression->arguments.size() & 1)
        {

            generateLine("sub rsp, 8");
            generateLine("call " + callExpression->callee);
            generateLine("add rsp, " + std::to_string(callExpression->arguments.size() * 8 + 8));

        }
        else 
        {

            generateLine("call " + callExpression->callee);
            generateLine("add rsp, " + std::to_string(callExpression->arguments.size() * 8));

        }

        break;

    }
    case NodeType::INDEX_EXPRESSION: 
    {

        auto* indexExpression = (IndexExpression*)expression;

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
    default: generateLine("TODO!!!");
        
    }

}

void CodeGenerator::generateFunction(FunctionDeclaration* fn)
{

    localOffsets.clear();
    currentStackOffset = 0;

    generateLine(fn->name + ":");
    generateLine("push rbp");
    generateLine("mov rbp, rsp");
    generateLine("push rbx");               

    int argOffset = 16;   

    for (auto& currentParam : fn->parameters)
    {

        currentStackOffset += 8;
        localOffsets[currentParam.name] = currentStackOffset;
        generateLine("sub rsp, 8");
        generateLine("mov rax, [rbp+" + std::to_string(argOffset) + "]");
        generateLine("mov [rbp-" + std::to_string(currentStackOffset) +
            "], rax ; param " + currentParam.name);
        argOffset += 8;

    }

    generateStatement(fn->body.get());

    generateLine(".return:");
    generateLine("pop rbx");
    generateLine("mov rsp, rbp"); 
    generateLine("pop rbp");
    generateLine("ret");
    generateLine("");

}

void CodeGenerator::generateStatement(Statement* statement)
{

    switch (statement->type) 
    {

    case NodeType::VARIABLE_DECLARATION:
    {

        auto variableDeclaration = (VariableDeclaration*)(statement);

        currentStackOffset += 8;
        localOffsets[variableDeclaration->name] = currentStackOffset;
        generateLine("sub rsp, 8");
        
        if (variableDeclaration->init)
        {

            generateExpression(variableDeclaration->init.get());
            generateLine("mov [rbp - " + std::to_string(currentStackOffset) + "], rax ; init " + variableDeclaration->name);

        }
        else
        {

           generateLine("mov qword [rbp - " + std::to_string(currentStackOffset) + "], 0 ; default-init"); 

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

