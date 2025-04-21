#include "ast_nodes/node/Node.h"
#include "ast_nodes/expressions/Expressions.h"
#include <vector>
#pragma once

struct Statement : Node { using Node::Node; };

struct ExpressionStatement: public Statement
{
public:

    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expression);

};

struct ReturnStatement: public Statement
{
public:

    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> value);

};

struct IfStatement: public Statement
{
public:

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenConsequence;
    std::unique_ptr<Statement> elseConsequence;

    IfStatement(std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> thenC, std::unique_ptr<Statement> elseC);

};

struct BlockStatement: public Statement
{
public:

    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement();

};

struct BreakStatement: public Statement
{
public:

    BreakStatement();

};

struct WhileStatement: public Statement
{
public:

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> body);

};

struct ForStatement: public Statement
{
public:

    std::unique_ptr<Statement> init;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> post;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Statement> init,
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> post,
        std::unique_ptr<Statement> body);

};

struct CaseClause
{
public:

    std::unique_ptr<Expression> test;
    std::unique_ptr<Statement> body;

    CaseClause(std::unique_ptr<Expression> test, std::unique_ptr<Statement> body);

};

struct ChooseStatement: public Statement
{
public:

    std::unique_ptr<Expression> expression;
    std::vector<CaseClause> cases;
    std::unique_ptr<Statement> defaultCase;

    ChooseStatement(std::unique_ptr<Expression> expression,
        std::vector<CaseClause> cases,
        std::unique_ptr<Statement> defaultCase);

};
