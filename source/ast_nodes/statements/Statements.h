#include "Node.h"
#include <vector>
#include "Expressions.h"
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

