#include "Semantic.h"
#include <stdexcept>

VariableType SemanticChecker::evalType(Expression* expression)
{

    switch (expression->type)
    {

    case NodeType::INT_LITERAL: return VariableType::Int;
    case NodeType::FLOAT_LITERAL: return VariableType::Float;
    case NodeType::CHAR_LITERAL: return VariableType::Char;
    case NodeType::BOOL_LITERAL: return VariableType::Bool;
    case NodeType::BOX_LITERAL: return VariableType::Box;
    case NodeType::IDENTIFIER: 
    {

        auto id = (IdentifierExpression*)(expression);
        const VariableInfo* variable = symbols.findVariable(id->name);

        if (!variable) throw std::runtime_error("undeclared identifier " + id->name);
        return variable->type;

    }
    case NodeType::BINARY_EXPRESSION: 
    {

        auto binaryExpression = (BinaryExpression*)(expression);
        return evalType(binaryExpression->left.get());      

    }
    case NodeType::UNARY_EXPRESSION: 
    {

        auto unaryExpression = (UnaryExpression*)(expression);
        return evalType(unaryExpression->operand.get());

    }
    default: return VariableType::Int; 

    }

}

void ScopedSymbolTable::enter()
{

    stack.emplace_back();

}

void ScopedSymbolTable::leave()
{

    stack.pop_back();

}

void ScopedSymbolTable::declare(const std::string& name, VariableType type)
{

    auto& currentScope = stack.back();
    if (currentScope.count(name))  throw std::runtime_error("redeclaration of variable '" + name + "'");
    currentScope[name] = VariableInfo{ type };

}

const VariableInfo* ScopedSymbolTable::findVariable(const std::string& name) const
{

    for (auto iter = stack.rbegin(); iter != stack.rend(); iter++)
    {

        auto iterSymbolManipulation = iter->find(name);
        if (iterSymbolManipulation != iter->end()) return &iterSymbolManipulation->second;

    }

    return nullptr;

}

void SemanticChecker::check(Program* program)
{

    visit(program);

}

void SemanticChecker::visit(Node* node)
{

    switch (node->type)
    {

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
    case NodeType::INDEX_EXPRESSION: visit((IndexExpression*)(node)); break;
    case NodeType::CALL_EXPRESSION: visit((CallExpression*)(node)); break;
    case NodeType::UNARY_EXPRESSION: visit((UnaryExpression*)(node)); break;
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

void SemanticChecker::visit(Program* program)
{

    for (auto& currentFunction : program->functions) visit(currentFunction.get());

}

void SemanticChecker::visit(FunctionDeclaration* functionDeclaration)
{

    symbols.enter();
    for (auto& currentParam : functionDeclaration->parameters) symbols.declare(std::string(currentParam.name), currentParam.type);

    currentReturnType = functionDeclaration->returnType;
    visit(functionDeclaration->body.get());
    currentReturnType.reset();

    symbols.leave();

}

void SemanticChecker::visit(BlockStatement* blockStatement)
{

    symbols.enter();
    for (auto& currentStatement : blockStatement->statements) visit(currentStatement.get());
    symbols.leave();

}

void SemanticChecker::visit(VariableDeclaration* variableDeclaration)
{

    symbols.declare(variableDeclaration->name, variableDeclaration->type);
    if (variableDeclaration->init) visit(variableDeclaration->init.get());

}

void SemanticChecker::visit(ExpressionStatement* expressionStatement)
{

    visit(expressionStatement->expression.get());

}

void SemanticChecker::visit(ReturnStatement* returnStatement)
{


    if (!currentReturnType) throw std::runtime_error("return outside of function");
    if (returnStatement->value) visit(returnStatement->value.get());
    if (!returnStatement->value && *currentReturnType != VariableType::Void) throw std::runtime_error("missing return value");
    
    if (returnStatement->value)
    {

        VariableType actual = evalType(returnStatement->value.get());

        if (actual != *currentReturnType)
            throw std::runtime_error("return type mismatch");

    }

}

void SemanticChecker::visit(IfStatement* ifStatement)
{

    visit(ifStatement->condition.get());
    visit(ifStatement->thenConsequence.get());

    if (ifStatement->elseConsequence) visit(ifStatement->elseConsequence.get());

}

void SemanticChecker::visit(WhileStatement* whileStatement)
{

    visit(whileStatement->condition.get());
    loopDepth += 1;
    visit(whileStatement->body.get());
    loopDepth -= 1;

}

void SemanticChecker::visit(ForStatement* forStatement)
{

    if (forStatement->init) visit(forStatement->init.get());
    if (forStatement->condition) visit(forStatement->condition.get());
    if (forStatement->post) visit(forStatement->post.get());

    loopDepth += 1;
    visit(forStatement->body.get());
    loopDepth -= 1;

}

void SemanticChecker::visit(BreakStatement*)
{

    if (loopDepth == 0 && chooseDepth == 0)
        throw std::runtime_error("'break' outside loop/choose");

}

void SemanticChecker::visit(BoxLiteral* boxLiteral)
{

    for (auto& currentElement : boxLiteral->elements) visit(currentElement.get());

}

void SemanticChecker::visit(IndexExpression* indexExpression)
{

    visit(indexExpression->base.get());
    visit(indexExpression->index.get());

}

void SemanticChecker::visit(CallExpression* callExpression)
{

    for (auto& currentArgument : callExpression->arguments) visit(currentArgument.get());

}

void SemanticChecker::visit(UnaryExpression* unaryExpression)
{

    visit(unaryExpression->operand.get());

}

void SemanticChecker::visit(BinaryExpression* binaryExpression)
{

    visit(binaryExpression->left.get());
    visit(binaryExpression->right.get());

}

void SemanticChecker::visit(ChooseStatement* chooseStatement)
{

    visit(chooseStatement->expression.get());
    chooseDepth += 1;

    for (auto& currentCase : chooseStatement->cases)
    {

        visit(currentCase.test.get());
        visit(currentCase.body.get());

    }

    if (chooseStatement->defaultCase) visit(chooseStatement->defaultCase.get());
    chooseDepth -= 1;

}

void SemanticChecker::visit(IdentifierExpression* id)
{

    if (!symbols.findVariable(id->name))
        throw std::runtime_error("undeclared identifier: " + std::string(id->name));

}
