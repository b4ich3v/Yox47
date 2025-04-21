#include "ast_nodes/declarations_and_program/DeclarationsAndProgram.h"
#include "ast_nodes/expressions/Expressions.h"
#include "ast_nodes/statements/Statements.h"
#include "ast_nodes/call_expression_and_variables/CallExpressionAndVariables.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
#pragma once

class CodeGenerator 
{
private:

    Program* root;
    std::ofstream file;
    std::unordered_map<std::string, int> localOffsets;
    int currentStackOffset;

    void generateLine(const std::string& text);

    void generateProgram(Program* program);

    void generateFunction(FunctionDeclaration* functionDeclaration);

    void generateStatement(Statement* statement);

    void generateExpression(Expression* expression);

public:

    explicit CodeGenerator(Program* root, const std::string& fileName);

    void generate();

};
