#include "CodeGenerator.h"

const char* CodeGenerator::typeTag(NodeType type) {
    switch (type) {
    case NodeType::INT_LITERAL: return "0"; 
    case NodeType::FLOAT_LITERAL: return "1"; 
    case NodeType::BOOL_LITERAL: return "2"; 
    case NodeType::BOX_LITERAL: return "3"; 
    case NodeType::CHAR_LITERAL: return "4";
    default: return "0";
    }
}

std::string CodeGenerator::sanitize(const std::string& str) {
    std::string result = str;
    for (char& currentSymbol : result) if (currentSymbol == '.') currentSymbol = '_';   
    return result;
}

std::string CodeGenerator::floatLabel(const std::string& text) {
    return "LC_float_" + sanitize(text);
}

CodeGenerator::CodeGenerator(Program* root, const std::string& fileName): 
    root(root), file(fileName) {}

void CodeGenerator::enterScope() {
    localOffsetScopes.emplace_back();
    varTypeScopes.emplace_back();
}

void CodeGenerator::leaveScope() {
    localOffsetScopes.pop_back();
    varTypeScopes.pop_back();
}

int CodeGenerator::findLocalOffset(const std::string& name) const {
    for (auto iter = localOffsetScopes.rbegin(); iter != localOffsetScopes.rend(); ++iter) {
        auto found = iter->find(name);
        if (found != iter->end()) return found->second;
    }
    return 0;
}

VariableType CodeGenerator::findVarType(const std::string& name) const {
    for (auto iter = varTypeScopes.rbegin(); iter != varTypeScopes.rend(); ++iter) {
        auto found = iter->find(name);
        if (found != iter->end()) return found->second;
    }
    return VariableType::Int;
}

bool CodeGenerator::isFloatExpression(Expression* expression) {
    switch (expression->type) {
    case NodeType::FLOAT_LITERAL: return true;
    case NodeType::INT_LITERAL:
    case NodeType::CHAR_LITERAL:
    case NodeType::BOOL_LITERAL: return false;
    case NodeType::IDENTIFIER: {      
        auto id = (IdentifierExpression*)(expression);
        return findVarType(id->name) == VariableType::Float;
    }
    case NodeType::UNARY_EXPRESSION: {
        if (auto castExpression = dynamic_cast<CastExpression*>(expression))
            return castExpression->targetType == VariableType::Float;

        auto unaryExpression = (UnaryExpression*)(expression);
        return isFloatExpression(unaryExpression->operand.get());
    }
    case NodeType::ASSIGNMENT_EXPRESSION: {
        auto assignmentExpression = (AssignmentExpression*)(expression);
        return isFloatExpression(assignmentExpression->value.get());
    }
    case NodeType::BINARY_EXPRESSION: {
        auto binaryExpression = (BinaryExpression*)(expression);

        return isFloatExpression(binaryExpression->left.get()) ||
            isFloatExpression(binaryExpression->right.get());
    }
    case NodeType::CALL_EXPRESSION: {
        auto callExpression = (CallExpression*)(expression);
        auto iter = fnReturnTypes.find(callExpression->callee);
        return iter != fnReturnTypes.end() && iter->second == VariableType::Float;
    }
    case NodeType::INDEX_EXPRESSION: return false; 
    default: return false;
    }
}

VariableType CodeGenerator::expressionValueType(Expression* expression) const {
    switch (expression->type) {
    case NodeType::INT_LITERAL: return VariableType::Int;
    case NodeType::FLOAT_LITERAL: return VariableType::Float;
    case NodeType::CHAR_LITERAL: return VariableType::Char;
    case NodeType::BOOL_LITERAL: return VariableType::Bool;
    case NodeType::BOX_LITERAL: return VariableType::Box;
    case NodeType::IDENTIFIER: {
        auto id = (IdentifierExpression*)(expression);
        return findVarType(id->name);
    }
    case NodeType::UNARY_EXPRESSION: {
        if (auto castExpression = dynamic_cast<CastExpression*>(expression))
            return castExpression->targetType;

        auto unaryExpression = (UnaryExpression*)(expression);
        if (unaryExpression->oper == TokenType::NEGATION) return VariableType::Bool;
        return expressionValueType(unaryExpression->operand.get());
    }
    case NodeType::ASSIGNMENT_EXPRESSION: {
        auto assignmentExpression = (AssignmentExpression*)(expression);
        return expressionValueType(assignmentExpression->value.get());
    }
    case NodeType::BINARY_EXPRESSION: {
        auto binaryExpression = (BinaryExpression*)(expression);
        VariableType left = expressionValueType(binaryExpression->left.get());
        VariableType right = expressionValueType(binaryExpression->right.get());
        switch (binaryExpression->oper) {
        case TokenType::LOGICAL_AND:
        case TokenType::LOGICAL_OR:
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
            return VariableType::Bool;
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
            return (left == VariableType::Float || right == VariableType::Float) ? VariableType::Float : VariableType::Int;
        default:
            return VariableType::Int;
        }
    }
    case NodeType::CALL_EXPRESSION: {
        auto callExpression = (CallExpression*)(expression);
        auto iter = fnReturnTypes.find(callExpression->callee);
        return iter != fnReturnTypes.end() ? iter->second : VariableType::Int;
    }
    case NodeType::INDEX_EXPRESSION:
        return VariableType::Int;
    default:
        return VariableType::Int;
    }
}

int CodeGenerator::countVarDeclsInStatement(Statement* statement) const {
    switch (statement->type) {
    case NodeType::VARIABLE_DECLARATION:
        return 1;
    case NodeType::BLOCK_STATEMENT: {
        int total = 0;
        auto* block = (BlockStatement*)statement;
        for (auto& st : block->statements)
            total += countVarDeclsInStatement(st.get());
        return total;
    }
    case NodeType::IF_STATEMENT: {
        auto* ifStmt = (IfStatement*)statement;
        int total = countVarDeclsInStatement(ifStmt->thenConsequence.get());
        if (ifStmt->elseConsequence) total += countVarDeclsInStatement(ifStmt->elseConsequence.get());
        return total;
    }
    case NodeType::WHILE_STATEMENT: {
        auto* whileStmt = (WhileStatement*)statement;
        return countVarDeclsInStatement(whileStmt->body.get());
    }
    case NodeType::FOR_STATEMENT: {
        auto* forStmt = (ForStatement*)statement;
        int total = 0;
        if (forStmt->init) total += countVarDeclsInStatement(forStmt->init.get());
        total += countVarDeclsInStatement(forStmt->body.get());
        return total;
    }
    case NodeType::CHOOSE_STATEMENT: {
        auto* chooseStmt = (ChooseStatement*)statement;
        int total = 0;
        for (auto& c : chooseStmt->cases)
            total += countVarDeclsInStatement(c.body.get());
        if (chooseStmt->defaultCase)
            total += countVarDeclsInStatement(chooseStmt->defaultCase.get());
        return total;
    }
    default:
        return 0;
    }
}

void CodeGenerator::generateLine(const std::string& text) {
    file << text << std::endl;
}

void CodeGenerator::generate() {
    generateLine("default rel");              
    generateLine("section .text");
    generateLine("global _start");

    generateLine("extern exit");
    generateLine("extern print_int");
    generateLine("extern print_bool");
    generateLine("extern print_char");
    generateLine("extern print_float");
    generateLine("extern print_box");
    generateLine("extern yox_malloc");
    generateLine("extern print_newline");
    generateLine("");

    generateProgram(root);
              
    generateLine("_start:");
    generateLine("call main");
    generateLine("mov rdi, rax");
    generateLine("call exit");

    if (!floatPool.empty()) {
        generateLine("");
        generateLine("section .rodata align=8");

        for (auto& kv : floatPool)
            generateLine(kv.first + ": dq " + kv.second);
    }
    else {
        generateLine("");
        generateLine("section .rodata");
    }

    generateLine(".LC_fmt_int   db \"%lld\", 10, 0");
    generateLine(".LC_fmt_float db \"%f\",   10, 0");
}

void CodeGenerator::generateProgram(Program* program) {
    for (auto& currentFunction : program->functions)
        fnReturnTypes[currentFunction->name] = currentFunction->returnType;

    for (auto& currentFunction : program->functions)
        generateFunction(currentFunction.get());
}

void CodeGenerator::generateExpression(Expression* expression) {
    switch (expression->type) {
    case NodeType::INT_LITERAL: {
        auto* intLiteral = (IntegerLitExpression*)expression;
        generateLine("mov rax, " + intLiteral->text);
        break;
    }
    case NodeType::FLOAT_LITERAL: {
        auto* floatLiteral = (FloatLitExpression*)expression;
        std::string label = floatLabel(floatLiteral->text);
        floatPool.emplace(label, floatLiteral->text);
        generateLine("movsd xmm0, [rel " + label + "]");
        generateLine("movq rax, xmm0"); 
        break;
    }
    case NodeType::BOOL_LITERAL: {
        auto* boolLiteral = (BoolLitExpression*)expression;
        generateLine("mov rax, " + std::to_string(boolLiteral->value));
        break;
    }
    case NodeType::CHAR_LITERAL: {
        auto* charLiteral = (CharLitExpression*)expression; 
        char variable = (charLiteral->text.size() >= 3) ? charLiteral->text[1] : 0; 
        generateLine("mov rax," + std::to_string((int)variable)); 
        break;
    }  
    case NodeType::BOX_LITERAL: {
        auto* boxLiteral = (BoxLiteral*)(expression);
        const std::size_t bytes = boxLiteral->elements.size() * 16 + 8;   

        generateLine("mov  rdi, " + std::to_string(bytes));        
        generateLine("call yox_malloc");                               
        generateLine("mov  rdx, rax");                             

        generateLine("mov  rax, " + std::to_string(boxLiteral->elements.size()));
        generateLine("mov  [rdx], rax");                           
        generateLine("lea  rbx, [rdx+8]");                         

        for (auto& elemUP : boxLiteral->elements) {
            Expression* element = elemUP.get();
            VariableType elementType = expressionValueType(element);
            const char* tag = "0";
            switch (elementType) {
            case VariableType::Int: tag = "0"; break;
            case VariableType::Float: tag = "1"; break;
            case VariableType::Bool: tag = "2"; break;
            case VariableType::Box: tag = "3"; break;
            case VariableType::Char: tag = "4"; break;
            default: tag = "0"; break;
            }

            generateLine(std::string("mov qword [rbx], ") + tag);
            generateLine("add rbx, 8");       

            generateLine("push rbx");         
            generateExpression(element);          
            generateLine("pop  rbx");          

            if (isFloatExpression(element))
                generateLine("movq [rbx], xmm0");
            else
                generateLine("mov  [rbx], rax");

            generateLine("add rbx, 8");
        }

        generateLine("mov  rax, rdx");                             
        break;
    }
    case NodeType::IDENTIFIER: {
        auto* id = (IdentifierExpression*)expression;
        int off = findLocalOffset(id->name);
        generateLine("mov rax, [rbp-" + std::to_string(off) + "] ; " + id->name);

        if (findVarType(id->name) == VariableType::Float)
            generateLine("movq xmm0, rax");

        break;
    }
    case NodeType::UNARY_EXPRESSION: {
        if (auto* castExpression = dynamic_cast<CastExpression*>(expression)) {
            switch (castExpression->targetType) {
            case VariableType::Float: {
                if (castExpression->value->type == NodeType::INDEX_EXPRESSION) {
                    static int idxCastCounter = 0;
                    int currentId = idxCastCounter++;
                    std::string labelFloat = ".Lidx_float_" + std::to_string(currentId);
                    std::string labelDone = ".Lidx_done_" + std::to_string(currentId);

                    auto* indexExpression = (IndexExpression*)castExpression->value.get();
                    generateExpression(indexExpression->base.get());
                    generateLine("push rax");
                    generateExpression(indexExpression->index.get());
                    generateLine("pop rbx");
                    generateLine("imul rax, 16");
                    generateLine("add rbx, 8");
                    generateLine("add rbx, rax");
                    generateLine("add rbx, 8");
                    generateLine("mov rcx, [rbx-8]");
                    generateLine("cmp rcx, 1");
                    generateLine("je " + labelFloat);
                    generateLine("mov rax, [rbx]");
                    generateLine("cvtsi2sd xmm0, rax");
                    generateLine("movq rax, xmm0");
                    generateLine("jmp " + labelDone);
                    generateLine(labelFloat + ":");
                    generateLine("movq xmm0, [rbx]");
                    generateLine("movq rax, xmm0");
                    generateLine(labelDone + ":");
                }
                else {
                    generateExpression(castExpression->value.get());
                    generateLine("cvtsi2sd xmm0, rax");
                    generateLine("movq rax, xmm0");
                }
                break;
            }
            case VariableType::Int:
                generateExpression(castExpression->value.get());
                if (isFloatExpression(castExpression->value.get())) {
                    generateLine("cvttsd2si rax, xmm0");
                }
                break;
            case VariableType::Bool: {
                generateExpression(castExpression->value.get());
                if (isFloatExpression(castExpression->value.get())) {
                    generateLine("xorpd xmm1, xmm1");
                    generateLine("ucomisd xmm0, xmm1");
                    generateLine("setne al");
                    generateLine("movzx rax, al");
                }
                else {
                    generateLine("cmp rax, 0");
                    generateLine("setne al");
                    generateLine("movzx rax, al");
                }
                break;
            }
            default: generateLine("; TODO!!!"); break;
            }
            break;
        }
        auto* unaryExpression = (UnaryExpression*)expression;
        generateExpression(unaryExpression->operand.get());

        if (unaryExpression->oper == TokenType::NEGATION) {        
            generateLine("cmp rax, 0");
            generateLine("sete al");
            generateLine("movzx rax, al");
        }
        else if (unaryExpression->oper == TokenType::MINUS) {           
            if (isFloatExpression(unaryExpression->operand.get())) {
                generateLine("xorpd xmm1, xmm1");
                generateLine("subsd xmm1, xmm0");
                generateLine("movq xmm0, xmm1");
                generateLine("movq rax, xmm0");
            }
            else {
                generateLine("neg rax");
            }

        }
        break;
    }                            
    case NodeType::BINARY_EXPRESSION: {
        auto* binaryExpression = (BinaryExpression*)expression;
        bool isFloat = isFloatExpression(binaryExpression);

        if (isFloat) {
            generateExpression(binaryExpression->right.get());
            generateLine("sub rsp, 8");
            generateLine("movq [rsp], xmm0");

            generateExpression(binaryExpression->left.get());
            generateLine("movq xmm1, [rsp]");
            generateLine("add rsp, 8");

            switch (binaryExpression->oper) {
            case TokenType::PLUS:  generateLine("addsd xmm0, xmm1");  generateLine("movq rax, xmm0"); break;
            case TokenType::MINUS: generateLine("subsd xmm0, xmm1");  generateLine("movq rax, xmm0"); break;
            case TokenType::STAR:  generateLine("mulsd xmm0, xmm1");  generateLine("movq rax, xmm0"); break;
            case TokenType::SLASH: generateLine("divsd xmm0, xmm1");  generateLine("movq rax, xmm0"); break;
            case TokenType::GREATER:
                generateLine("comisd xmm0, xmm1");   
                generateLine("seta  al");
                generateLine("movzx rax, al");
                break;
            case TokenType::LESS:
                generateLine("comisd xmm0, xmm1");   
                generateLine("setb  al");
                generateLine("movzx rax, al");
                break;
            case TokenType::EQUAL_EQUAL:
                generateLine("comisd xmm0, xmm1");   
                generateLine("sete  al");
                generateLine("movzx rax, al");
                break;
            case TokenType::LESS_EQUAL:
                generateLine("comisd xmm0, xmm1");
                generateLine("setbe al");        
                generateLine("movzx rax, al");
                break;
            case TokenType::GREATER_EQUAL:
                generateLine("comisd xmm0, xmm1");
                generateLine("setae al");        
                generateLine("movzx rax, al");
                break;
            case TokenType::NOT_EQUAL:
                generateLine("comisd xmm0, xmm1");
                generateLine("setne al");        
                generateLine("movzx rax, al");
                break;
            default:
                generateLine("; TODO FLOAT-оператор");
                break;
            }
            break;     
        }

        generateExpression(binaryExpression->right.get());
        generateLine("push rax");
        generateExpression(binaryExpression->left.get());
        generateLine("pop rcx");

        switch (binaryExpression->oper) {
        case TokenType::PLUS: generateLine("add rax, rcx"); break;
        case TokenType::MINUS: generateLine("sub rax, rcx"); break;
        case TokenType::STAR: generateLine("imul rax, rcx"); break;
        case TokenType::SLASH: generateLine("cqo"); generateLine("idiv rcx"); break;
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL: {
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
        case TokenType::LOGICAL_AND: {
            generateLine("and rax, rcx");
            generateLine("cmp rax, 0");
            generateLine("setne al");
            generateLine("movzx rax, al");
            break;
        } 
        case TokenType::LOGICAL_OR: {
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
    case NodeType::CALL_EXPRESSION: {
        auto* callExpression = (CallExpression*)(expression);

        for (auto iter = callExpression->arguments.rbegin();
            iter != callExpression->arguments.rend(); ++iter) {
            generateExpression(iter->get());   
            generateLine("push rax");        
        }

        generateLine("call " + callExpression->callee);

        if (!callExpression->arguments.empty())
            generateLine("add rsp, " + std::to_string(callExpression->arguments.size() * 8));

        auto rt = fnReturnTypes.find(callExpression->callee);

        if (rt != fnReturnTypes.end() &&
            rt->second == VariableType::Float) {
            generateLine("movq rax, xmm0");
        }
        break;
    }
    case NodeType::INDEX_EXPRESSION: {
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
    case NodeType::ASSIGNMENT_EXPRESSION: {
        auto* assignmentExpression = (AssignmentExpression*)expression;

        if (assignmentExpression->target->type == NodeType::IDENTIFIER) {
            generateExpression(assignmentExpression->value.get());
            auto* id = (IdentifierExpression*)assignmentExpression->target.get();
            int currentOffset = findLocalOffset(id->name);
            generateLine("mov [rbp-" + std::to_string(currentOffset) + "], rax ; assign " + id->name);
        }
        else {
            generateExpression(assignmentExpression->value.get());
            VariableType valueType = expressionValueType(assignmentExpression->value.get());
            generateLine("push rax");
            auto* indexExpression = (IndexExpression*)assignmentExpression->target.get();
            
            generateExpression(indexExpression->base.get());        
            generateLine("push rax");
            generateExpression(indexExpression->index.get());       
            generateLine("pop rbx");
            generateLine("imul rax, 16");
            generateLine("add rbx, 8");
            generateLine("add rbx, rax");
            generateLine("add rbx, 8");
            const char* tag = "0";
            switch (valueType) {
            case VariableType::Int: tag = "0"; break;
            case VariableType::Float: tag = "1"; break;
            case VariableType::Bool: tag = "2"; break;
            case VariableType::Box: tag = "3"; break;
            case VariableType::Char: tag = "4"; break;
            default: tag = "0"; break;
            }
            generateLine(std::string("mov qword [rbx-8], ") + tag);
            generateLine("pop rax");
            if (valueType == VariableType::Float) {
                generateLine("movq xmm0, rax");
                generateLine("movq [rbx], xmm0");
            }
            else {
                generateLine("mov [rbx], rax");
            }
        }
        break;
    }
    default: generateLine("TODO!!!");
    }
}

void CodeGenerator::generateFunction(FunctionDeclaration* functionDeclaration) {
    localOffsetScopes.clear();
    varTypeScopes.clear();
    currentStackOffset = 0;
    while (!breakLabels.empty()) breakLabels.pop();
    enterScope();

    generateLine(functionDeclaration->name + ":");
    generateLine("push rbp");
    generateLine("mov rbp, rsp");
    generateLine("push rbx");               
    {
        int localCount = countVarDeclsInStatement(functionDeclaration->body.get());
        int frameSlots = (int)functionDeclaration->parameters.size() + localCount;
        if (frameSlots > 0)
            generateLine("sub rsp, " + std::to_string(frameSlots * 8));
    }

    int argOffset = 16;   

    for (auto& currentParam : functionDeclaration->parameters) {
        currentStackOffset += 8;
        localOffsetScopes.back()[currentParam.name] = currentStackOffset;
        varTypeScopes.back()[currentParam.name] = currentParam.type;

        generateLine("mov rax, [rbp+" + std::to_string(argOffset) + "]");
        generateLine("mov [rbp-" + std::to_string(currentStackOffset) +
            "], rax ; param " + currentParam.name);
        argOffset += 8;

    }

    generateStatement(functionDeclaration->body.get());
    generateLine(".return:");

    if (functionDeclaration->returnType == VariableType::Float)
        generateLine("movq xmm0, rax");

    generateLine("pop rbx");
    generateLine("mov rsp, rbp"); 
    generateLine("pop rbp");
    generateLine("ret");
    generateLine("");
    leaveScope();
}

void CodeGenerator::generateStatement(Statement* statement) {
    switch (statement->type) {
    case NodeType::VARIABLE_DECLARATION: {
        auto variableDeclaration = (VariableDeclaration*)(statement);
        varTypeScopes.back()[variableDeclaration->name] = variableDeclaration->type;

        currentStackOffset += 8;
        localOffsetScopes.back()[variableDeclaration->name] = currentStackOffset;
        
        if (variableDeclaration->init) {
            generateExpression(variableDeclaration->init.get());
            generateLine("mov [rbp - " + std::to_string(currentStackOffset) + "], rax ; init " + variableDeclaration->name);
        }
        else {
           generateLine("mov qword [rbp - " + std::to_string(currentStackOffset) + "], 0 ; default-init"); 
        }

        break;
    }
    case NodeType::RETURN_STATEMENT: {
        auto returnStatement = (ReturnStatement*)(statement);

        if (returnStatement->value)
            generateExpression(returnStatement->value.get());

        generateLine("jmp .return");
        break;
    }
    case NodeType::EXPRESSION_STATEMENT: {
        auto expressionStatement = (ExpressionStatement*)(statement);
        generateExpression(expressionStatement->expression.get());
        break;
    }
    case NodeType::BLOCK_STATEMENT: {
        auto blockStatement = (BlockStatement*)(statement);
        enterScope();

        for (auto& currentStatement : blockStatement->statements)
            generateStatement(currentStatement.get());

        leaveScope();
        break;
    }
    case NodeType::BREAK_STATEMENT: {
        if (!breakLabels.empty())
            generateLine("jmp " + breakLabels.top());

        break;
    }
    case NodeType::WHILE_STATEMENT: {
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
    case NodeType::FOR_STATEMENT: {
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

        if (forStatement->condition) {
            generateExpression(forStatement->condition.get());
            generateLine("cmp rax, 0");
            generateLine("jne " + bodyLabel);

        }
        else {
            generateLine("jmp " + bodyLabel);
        }

        generateLine(endLabel + ":");
        breakLabels.pop();
        break;
    }
    case NodeType::IF_STATEMENT: {
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
    case NodeType::CHOOSE_STATEMENT: {
        auto chooseStatement = (ChooseStatement*)(statement);
        static int chooseCounter = 0;
        int currentId = chooseCounter;
        chooseCounter += 1;

        std::string endLabel = ".Lend_choose_" + std::to_string(currentId);
        breakLabels.push(endLabel);

        generateExpression(chooseStatement->expression.get());
        generateLine("push rax ; value to match");

        for (int i = 0; i < chooseStatement->cases.size(); i++) {
            std::string caseLabel = ".Lcase_" + std::to_string(currentId) + "_" + std::to_string(i);
            generateExpression(chooseStatement->cases[i].test.get());
            generateLine("pop rcx");
            generateLine("push rcx");
            generateLine("cmp rax, rcx");
            generateLine("je " + caseLabel);
        }

        if (chooseStatement->defaultCase) {
            generateLine("add rsp, 8");
            generateLine("; default case");
            generateStatement(chooseStatement->defaultCase.get());
            generateLine("jmp " + endLabel);
        }
        else {
            generateLine("add rsp, 8");
            generateLine("jmp " + endLabel);
        }

        for (int i = 0; i < chooseStatement->cases.size(); i++) {
            std::string caseLabel = ".Lcase_" + std::to_string(currentId) + "_" + std::to_string(i);
            generateLine(caseLabel + ":");
            generateLine("add rsp, 8");
            generateStatement(chooseStatement->cases[i].body.get());
            generateLine("jmp " + endLabel);

        }

        generateLine(endLabel + ":");
        breakLabels.pop();
        break;
    }
    default:  generateLine("; TODO!!!"); break;
    }
}
