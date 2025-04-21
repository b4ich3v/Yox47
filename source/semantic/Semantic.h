#include "ast_nodes/node/Node.h"
#include "ast_nodes/call_expression_and_variables/CallExpressionAndVariables.h"
#include "ast_nodes/declarations_and_program/DeclarationsAndProgram.h"
#include <unordered_map>
#include <vector>
#include <string>
#pragma once

struct VariableInfo
{
public:

    VariableType type;

};

using Scope = std::unordered_map<std::string, VariableInfo>;

class ScopedSymbolTable
{
private:

    std::vector<Scope> stack{ 1 };

public:

    void enter();

    void leave();

    void declare(const std::string& name, VariableType type);

    const VariableInfo* findVariable(const std::string& name) const;

};

class SemanticChecker
{
private:

    ScopedSymbolTable symbols;

    void visit(Node* node);

    void visit(Program* program);

    void visit(FunctionDeclaration* functionDeclaration);

    void visit(BlockStatement* blockStatement);

    void visit(VariableDeclaration* variableDeclaration);

    void visit(ExpressionStatement* expressionStatement);

    void visit(ReturnStatement* returnStatement);

    void visit(IfStatement* ifStatement);

    void visit(WhileStatement* whileStatement);

    void visit(ForStatement* forStatement);

    void visit(BreakStatement* breakStatement);

    void visit(BoxLiteral* boxLiteral);

    void visit(IndexExpression* indexExpression);

    void visit(CallExpression* callExpression);

    void visit(UnaryExpression* unaryExpression);

    void visit(BinaryExpression* binaryExpression);

    void visit(ChooseStatement* chooseStatement);

    void visit(IdentifierExpression* id);

public:

    void check(Program* program);

};
