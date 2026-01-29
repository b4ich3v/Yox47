#include "Statements.h"
#pragma once

enum class VariableType: uint8_t { Int, Float, Char, Bool, Box, Void};

struct VariableDeclaration: public Statement {
public:

    std::string name;
    VariableType type;
    std::unique_ptr<Expression> init;

    VariableDeclaration(const std::string& name, VariableType type,
        std::unique_ptr<Expression> init);

};

struct Parameter {
public:

    std::string name;
    VariableType type;

};

struct CallExpression: public Expression {
public:

    std::string callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(const std::string& callee,
        std::vector<std::unique_ptr<Expression>> arguments);

};

struct CastExpression: public Expression {
public:

    VariableType targetType;
    std::unique_ptr<Expression> value;

    CastExpression(VariableType target, std::unique_ptr<Expression> value);

};