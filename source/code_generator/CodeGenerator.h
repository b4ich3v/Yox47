#include "DeclarationsAndProgram.h"
#include "Expressions.h"
#include "Statements.h"
#include "CallExpressionAndVariables.h"
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
    int currentStackOffset;

    std::stack<std::string> breakLabels;
    std::unordered_map<std::string, int> localOffsets;
    std::unordered_map<std::string, std::string> floatPool;
    
    void generateLine(const std::string& text);

    void generateProgram(Program* program);

    void generateFunction(FunctionDeclaration* functionDeclaration);

    void generateStatement(Statement* statement);

    void generateExpression(Expression* expression);

public:

    explicit CodeGenerator(Program* root, const std::string& fileName);

    void generate();

    static const char* typeTag(NodeType type);

};
