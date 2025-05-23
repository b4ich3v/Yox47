#include "Node.h"
#include "Statements.h"
#include "CallExpressionAndVariables.h"
#pragma once

struct FunctionDeclaration: public Node
{
public:

    std::string name;
    std::vector<Parameter> parameters;
    VariableType returnType;
    std::unique_ptr<BlockStatement> body;

    FunctionDeclaration(std::string name, VariableType returnType,
        std::unique_ptr<BlockStatement> body);

};

struct Program: public Node
{
public:

    std::vector<std::unique_ptr<FunctionDeclaration>> functions;

    Program();

};

