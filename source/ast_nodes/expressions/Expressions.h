#include "Node.h"
#include "Lexer.h"
#include <string>
#pragma once

struct Expression: public Node { using Node::Node; };

struct IdentifierExpression: public Expression
{
public:

    std::string name;

    IdentifierExpression(const std::string& name);

};

struct IntegerLitExpression: public Expression
{
public:

    std::string text;

    IntegerLitExpression(const std::string& text);

};

struct FloatLitExpression: public Expression
{
public:

    std::string text;

    FloatLitExpression(const std::string& text);

};

struct CharLitExpression: public Expression
{
public:

    std::string text;

    explicit CharLitExpression(const std::string& text);

};

struct BinaryExpression: public Expression
{
public:

    TokenType oper;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpression(const TokenType& oper,
        std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

};

struct UnaryExpression: public Expression
{
public:

    TokenType oper;
    std::unique_ptr<Expression> operand;

    UnaryExpression(TokenType op, std::unique_ptr<Expression> expression);

};

struct BoolLitExpression: public Expression
{
public:

    bool value;

    explicit BoolLitExpression(bool value);

};

struct BoxLiteral: public Expression
{
public:

    std::vector<std::unique_ptr<Expression>> elements;

    explicit BoxLiteral(std::vector<std::unique_ptr<Expression>> elements);

};

struct IndexExpression: public Expression
{
public:

    std::unique_ptr<Expression> base;
    std::unique_ptr<Expression> index;

    IndexExpression(std::unique_ptr<Expression> base,
        std::unique_ptr<Expression> index);

};

struct AssignmentExpression: public Expression
{
public:

    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;
    
    AssignmentExpression(std::unique_ptr<Expression> target,
        std::unique_ptr<Expression> value);

};
