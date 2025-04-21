#include "ast_nodes/node/Node.h"
#include "statements/Statements.h"
#include "ast_nodes/call_expression_and_variables/CallExpressionAndVariables.h"
#pragma once

struct FunctionDeclaration: public Node
{
public:

    std::string name;
    std::vector<Parameter> parameters;
    std::unique_ptr<BlockStatement> body;

    FunctionDeclaration(std::string name, std::unique_ptr<BlockStatement> body);

};

struct Program: public Node
{
public:

    std::vector<std::unique_ptr<FunctionDeclaration>> functions;

    Program();

};
