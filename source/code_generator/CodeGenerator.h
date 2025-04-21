#include "DeclarationsAndProgram.h"
#include "Expressions.h"
#include "Statements.h"
#include "CallExpressionAndVariables.h"
#include <fstream>
#pragma once

class CodeGenerator 
{
private:

    Program* root;
    std::ofstream file;

    void emit(const std::string& text);

    void genProgram(Program* program);

    void genFunction(FunctionDeclaration* functionDeclaration);

    void genStatement(Statement* statement);

    void genExpression(Expression* expression);

public:

    explicit CodeGenerator(Program* root, const std::string& fileName);

    void generate();

};

