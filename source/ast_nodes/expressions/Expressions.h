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

struct BinaryExpression: public Expression 
{
public:

    TokenType oper;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpression(const TokenType& oper, 
        std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
        
};

