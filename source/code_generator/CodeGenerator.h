#include "DeclarationsAndProgram.h"
#include "Expressions.h"
#include "Statements.h"
#include "CallExpressionAndVariables.h"
#include <fstream>
#include <string>
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
