#include "DeclarationsAndProgram.h"
#include "Expressions.h"
#include "Statements.h"
#include "CallExpressionAndVariables.h"
#include "Semantic.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>
#pragma once

class CodeGenerator {
private:
    Program* root = nullptr;
    std::ofstream file;

    std::vector<std::unordered_map<std::string, VariableType>> varTypeScopes;
    std::unordered_map<std::string, VariableType> fnReturnTypes;
    int currentStackOffset;

    std::stack<std::string> breakLabels;
    std::vector<std::unordered_map<std::string, int>> localOffsetScopes;
    std::unordered_map<std::string, std::string> floatPool;
    
    bool isFloatExpression(Expression* expression);
    VariableType expressionValueType(Expression* expression) const;
    int findLocalOffset(const std::string& name) const;
    VariableType findVarType(const std::string& name) const;
    void enterScope();
    void leaveScope();
    static std::string floatLabel(const std::string& txt);
    void generateLine(const std::string& text);
    void generateProgram(Program* program);
    void generateFunction(FunctionDeclaration* functionDeclaration);
    void generateStatement(Statement* statement);
    void generateExpression(Expression* expression);

public:
    explicit CodeGenerator(Program* root, const std::string& fileName);

    void generate();
    static const char* typeTag(NodeType type);
    static std::string sanitize(const std::string& str);
};
