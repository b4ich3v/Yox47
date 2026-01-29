#include "Semantic.h"
#include <stdexcept>

VariableType SemanticChecker::evaluateType(Expression* expression) {
    switch (expression->type) {
    case NodeType::INT_LITERAL: return VariableType::Int;
    case NodeType::FLOAT_LITERAL: return VariableType::Float;
    case NodeType::CHAR_LITERAL: return VariableType::Char;
    case NodeType::BOOL_LITERAL: return VariableType::Bool;
    case NodeType::BOX_LITERAL: return VariableType::Box;
    case NodeType::IDENTIFIER: {
        auto id = (IdentifierExpression*)(expression);
        const VariableInfo* variable = symbols.findVariable(id->name);

        if (!variable) throw std::runtime_error("undeclared identifier " + id->name);
        return variable->type;
    }
    case NodeType::BINARY_EXPRESSION: {
        auto* binaryExpression = (BinaryExpression*)expression;
        VariableType left = evaluateType(binaryExpression->left.get());
        VariableType right = evaluateType(binaryExpression->right.get());

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
            return (left == VariableType::Float ||
                right == VariableType::Float) ? VariableType::Float : VariableType::Int;
        default:
            return VariableType::Int;
        }
    }
    case NodeType::UNARY_EXPRESSION: {
        if (auto* castExpression = dynamic_cast<CastExpression*>(expression))
            return castExpression->targetType;

        auto unaryExpression = (UnaryExpression*)(expression);
        if (unaryExpression->oper == TokenType::NEGATION) return VariableType::Bool;
        return evaluateType(unaryExpression->operand.get());
    }
    case NodeType::ASSIGNMENT_EXPRESSION: {
        auto* assignmentExpression = (AssignmentExpression*)expression;
        return evaluateType(assignmentExpression->value.get());
    }
    case NodeType::CALL_EXPRESSION: {
        auto* callExpression = (CallExpression*)expression;
        auto iter = functions.find(callExpression->callee);
        if (iter == functions.end())
            throw std::runtime_error("call to undefined function " + callExpression->callee);
        return iter->second.returnType;
    }
    default: return VariableType::Int;
    }
}

bool SemanticChecker::canImplicitlyCast(VariableType from, VariableType to) {
    return (from == VariableType::Int && to == VariableType::Float) ||
        (from == VariableType::Char && to == VariableType::Int) ||
        (from == VariableType::Int && to == VariableType::Bool) ||
        (from == VariableType::Float && to == VariableType::Bool) ||
        (from == VariableType::Bool && to == VariableType::Int) ||
        (from == VariableType::Bool && to == VariableType::Float);
}

void ScopedSymbolTable::enter() {
    stack.emplace_back();
}

void ScopedSymbolTable::leave() {
    stack.pop_back();
}

void ScopedSymbolTable::declare(const std::string& name, VariableType type) {
    auto& currentScope = stack.back();
    if (currentScope.count(name))  throw std::runtime_error("redeclaration of variable '" + name + "'");
    currentScope[name] = VariableInfo{ type };
}

const VariableInfo* ScopedSymbolTable::findVariable(const std::string& name) const {
    for (auto iter = stack.rbegin(); iter != stack.rend(); iter++) {
        auto iterSymbolManipulation = iter->find(name);
        if (iterSymbolManipulation != iter->end()) return &iterSymbolManipulation->second;
    }

    return nullptr;
}

void SemanticChecker::check(Program* program) {
    visit(program);
}

void SemanticChecker::visit(Node* node) {
    switch (node->type) {
    case NodeType::PROGRAM: visit((Program*)(node)); break;
    case NodeType::FUNCTION_DECLARATION: visit((FunctionDeclaration*)(node)); break;
    case NodeType::BLOCK_STATEMENT: visit((BlockStatement*)(node)); break;
    case NodeType::VARIABLE_DECLARATION: visit((VariableDeclaration*)(node)); break;
    case NodeType::EXPRESSION_STATEMENT: visit((ExpressionStatement*)(node)); break;
    case NodeType::RETURN_STATEMENT: visit((ReturnStatement*)(node)); break;
    case NodeType::IF_STATEMENT: visit((IfStatement*)(node)); break;
    case NodeType::WHILE_STATEMENT: visit((WhileStatement*)(node)); break;
    case NodeType::FOR_STATEMENT: visit((ForStatement*)(node)); break;
    case NodeType::BREAK_STATEMENT: visit((BreakStatement*)(node)); break;
    case NodeType::BOX_LITERAL: visit((BoxLiteral*)(node)); break;
    case NodeType::ASSIGNMENT_EXPRESSION: visit((AssignmentExpression*)(node)); break;
    case NodeType::INDEX_EXPRESSION: visit((IndexExpression*)(node)); break;
    case NodeType::CALL_EXPRESSION: visit((CallExpression*)(node)); break;
    case NodeType::UNARY_EXPRESSION: {
        if (auto* castExpression = dynamic_cast<CastExpression*>(node))
            visit(castExpression);
        else
            visit((UnaryExpression*)(node));
        break;
    }
    case NodeType::BINARY_EXPRESSION: visit((BinaryExpression*)(node)); break;
    case NodeType::IDENTIFIER: visit((IdentifierExpression*)(node)); break;
    case NodeType::CHOOSE_STATEMENT: visit((ChooseStatement*)(node)); break;
    case NodeType::INT_LITERAL:
    case NodeType::FLOAT_LITERAL:
    case NodeType::BOOL_LITERAL:
    case NodeType::CHAR_LITERAL: break;
    default: break;
    }
}

void SemanticChecker::visit(Program* program) {
    functions.clear();
    for (auto& currentFunction : program->functions) {
        if (functions.count(currentFunction->name))
            throw std::runtime_error("redefinition of function '" + currentFunction->name + "'");

        FunctionInfo info;
        info.returnType = currentFunction->returnType;
        for (auto& param : currentFunction->parameters)
            info.paramTypes.push_back(param.type);

        functions[currentFunction->name] = std::move(info);
    }

    for (auto& currentFunction : program->functions) visit(currentFunction.get());
}

void SemanticChecker::visit(FunctionDeclaration* functionDeclaration) {
    symbols.enter();
    for (auto& currentParam : functionDeclaration->parameters) symbols.declare(std::string(currentParam.name), currentParam.type);

    currentReturnType = functionDeclaration->returnType;
    visit(functionDeclaration->body.get());
    currentReturnType.reset();

    symbols.leave();
}

void SemanticChecker::visit(BlockStatement* blockStatement) {
    symbols.enter();

    for (auto& currentStatement : blockStatement->statements) {
        visit(currentStatement.get());
    }

    symbols.leave();
}

void SemanticChecker::visit(VariableDeclaration* variableDeclaration) {
    symbols.declare(variableDeclaration->name, variableDeclaration->type);
    if (variableDeclaration->init) visit(variableDeclaration->init.get());
}

void SemanticChecker::visit(ExpressionStatement* expressionStatement) {
    visit(expressionStatement->expression.get());
}

void SemanticChecker::visit(ReturnStatement* returnStatement) {
    if (!currentReturnType) throw std::runtime_error("return outside of function");
    if (returnStatement->value) visit(returnStatement->value.get());
    if (!returnStatement->value && *currentReturnType != VariableType::Void) throw std::runtime_error("missing return value");

    if (returnStatement->value) {
        VariableType actual = evaluateType(returnStatement->value.get());

        if (actual != *currentReturnType)
            throw std::runtime_error("return type mismatch");
    }
}

void SemanticChecker::visit(AssignmentExpression* assignmentExpression) {
    visit(assignmentExpression->value.get());

    VariableType right = evaluateType(assignmentExpression->value.get());
    VariableType left;

    if (assignmentExpression->target->type == NodeType::IDENTIFIER) {
        auto id = (IdentifierExpression*)assignmentExpression->target.get();
        const VariableInfo* variable = symbols.findVariable(id->name);

        if (!variable) throw std::runtime_error("undeclared identifier " + id->name);
        left = variable->type;
    }
    else {
        left = VariableType::Box;
    }

    if (left != right) {
        if (canImplicitlyCast(right, left)) {
            assignmentExpression->value =
                std::make_unique<CastExpression>(left,
                    std::move(assignmentExpression->value));
        }
        else {
            throw std::runtime_error("type mismatch in assignment");
        }
    }
    visit(assignmentExpression->target.get());
}

void SemanticChecker::visit(IfStatement* ifStatement) {
    visit(ifStatement->condition.get());
    visit(ifStatement->thenConsequence.get());
    if (ifStatement->elseConsequence) visit(ifStatement->elseConsequence.get());
}

void SemanticChecker::visit(WhileStatement* whileStatement) {
    visit(whileStatement->condition.get());
    loopDepth += 1;
    visit(whileStatement->body.get());
    loopDepth -= 1;
}

void SemanticChecker::visit(ForStatement* forStatement) {
    if (forStatement->init) visit(forStatement->init.get());
    if (forStatement->condition) visit(forStatement->condition.get());
    if (forStatement->post) visit(forStatement->post.get());

    loopDepth += 1;
    visit(forStatement->body.get());
    loopDepth -= 1;
}

void SemanticChecker::visit(BreakStatement*) {
    if (loopDepth == 0 && chooseDepth == 0)
        throw std::runtime_error("'break' outside loop/choose");
}

void SemanticChecker::visit(BoxLiteral* boxLiteral) {
    for (auto& currentElement : boxLiteral->elements) visit(currentElement.get());
}

void SemanticChecker::visit(IndexExpression* indexExpression) {
    visit(indexExpression->base.get());
    visit(indexExpression->index.get());
}

void SemanticChecker::visit(CallExpression* callExpression) {
    auto iter = functions.find(callExpression->callee);
    if (iter == functions.end())
        throw std::runtime_error("call to undefined function " + callExpression->callee);

    if (callExpression->arguments.size() != iter->second.paramTypes.size())
        throw std::runtime_error("argument count mismatch in call to " + callExpression->callee);

    for (size_t i = 0; i < callExpression->arguments.size(); i++) {
        auto& arg = callExpression->arguments[i];
        visit(arg.get());
        VariableType actual = evaluateType(arg.get());
        VariableType expected = iter->second.paramTypes[i];

        if (actual != expected) {
            if (canImplicitlyCast(actual, expected)) {
                arg = std::make_unique<CastExpression>(expected, std::move(arg));
            }
            else
            {
                throw std::runtime_error("argument type mismatch in call to " + callExpression->callee);
            }
        }
    }
}

void SemanticChecker::visit(CastExpression* castExpression) {
    visit(castExpression->value.get());
}

void SemanticChecker::visit(UnaryExpression* unaryExpression) {
    visit(unaryExpression->operand.get());

    if (unaryExpression->oper == TokenType::NEGATION) {
        VariableType actual = evaluateType(unaryExpression->operand.get());
        if (actual != VariableType::Bool) {
            if (canImplicitlyCast(actual, VariableType::Bool)) {
                unaryExpression->operand =
                    std::make_unique<CastExpression>(VariableType::Bool, std::move(unaryExpression->operand));
            }
            else {
                throw std::runtime_error("invalid operand for '!'");
            }
        }
    }
    else if (unaryExpression->oper == TokenType::MINUS) {
        VariableType actual = evaluateType(unaryExpression->operand.get());
        VariableType target = (actual == VariableType::Float) ? VariableType::Float : VariableType::Int;

        if (actual != target) {
            if (canImplicitlyCast(actual, target)) {
                unaryExpression->operand =
                    std::make_unique<CastExpression>(target, std::move(unaryExpression->operand));
            }
            else {
                throw std::runtime_error("invalid operand for unary minus");
            }
        }
    }
}

void SemanticChecker::visit(BinaryExpression* binaryExpression) {
    visit(binaryExpression->left.get());
    visit(binaryExpression->right.get());

    VariableType left = evaluateType(binaryExpression->left.get());
    VariableType right = evaluateType(binaryExpression->right.get());

    auto castSide = [&](std::unique_ptr<Expression>& expr, VariableType target) {
        expr = std::make_unique<CastExpression>(target, std::move(expr));
    };

    switch (binaryExpression->oper) {
    case TokenType::LOGICAL_AND:
    case TokenType::LOGICAL_OR: {
        if (left != VariableType::Bool) {
            if (canImplicitlyCast(left, VariableType::Bool))
                castSide(binaryExpression->left, VariableType::Bool);
            else
                throw std::runtime_error("invalid left operand for logical operator");
        }
        if (right != VariableType::Bool) {
            if (canImplicitlyCast(right, VariableType::Bool))
                castSide(binaryExpression->right, VariableType::Bool);
            else
                throw std::runtime_error("invalid right operand for logical operator");
        }
        return;
    }
    case TokenType::EQUAL_EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::LESS:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER:
    case TokenType::GREATER_EQUAL: {
        VariableType target =
            (left == VariableType::Float || right == VariableType::Float) ? VariableType::Float : VariableType::Int;

        if (left != target) {
            if (canImplicitlyCast(left, target))
                castSide(binaryExpression->left, target);
            else
                throw std::runtime_error("invalid left operand for comparison");
        }
        if (right != target) {
            if (canImplicitlyCast(right, target))
                castSide(binaryExpression->right, target);
            else
                throw std::runtime_error("invalid right operand for comparison");
        }
        return;
    }
    case TokenType::PLUS:
    case TokenType::MINUS:
    case TokenType::STAR:
    case TokenType::SLASH: {
        VariableType target =
            (left == VariableType::Float || right == VariableType::Float) ? VariableType::Float : VariableType::Int;

        if (left != target) {
            if (canImplicitlyCast(left, target))
                castSide(binaryExpression->left, target);
            else
                throw std::runtime_error("invalid left operand for arithmetic");
        }
        if (right != target) {
            if (canImplicitlyCast(right, target))
                castSide(binaryExpression->right, target);
            else
                throw std::runtime_error("invalid right operand for arithmetic");
        }
        return;
    }
    default:
        break;
    }

    if (left == right) return;

    if (left == VariableType::Float && canImplicitlyCast(right, left)) {
        binaryExpression->right = std::make_unique<CastExpression>(left,
            std::move(binaryExpression->right));
        return;
    }
    if (right == VariableType::Float && canImplicitlyCast(left, right)) {
        binaryExpression->left = std::make_unique<CastExpression>(right,
            std::move(binaryExpression->left));
        return;
    }
    if (left == VariableType::Int && canImplicitlyCast(right, left)) {
        binaryExpression->right = std::make_unique<CastExpression>(left,
            std::move(binaryExpression->right));
        return;
    }
    if (right == VariableType::Int && canImplicitlyCast(left, right)) {
        binaryExpression->left = std::make_unique<CastExpression>(right,
            std::move(binaryExpression->left));
        return;
    }

    throw std::runtime_error("type mismatch in binary expression");
}

void SemanticChecker::visit(ChooseStatement* chooseStatement) {
    visit(chooseStatement->expression.get());
    chooseDepth += 1;

    for (auto& currentCase : chooseStatement->cases) {
        visit(currentCase.test.get());
        visit(currentCase.body.get());
    }

    if (chooseStatement->defaultCase) visit(chooseStatement->defaultCase.get());
    chooseDepth -= 1;
}

void SemanticChecker::visit(IdentifierExpression* id) {
    if (!symbols.findVariable(id->name))
        throw std::runtime_error("undeclared identifier: " + std::string(id->name));
}
