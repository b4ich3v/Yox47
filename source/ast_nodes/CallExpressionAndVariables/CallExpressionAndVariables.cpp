#include "CallExpressionAndVariables.h"

VariableDeclaration::VariableDeclaration(const std::string& name, VariableType type,
    std::unique_ptr<Expression> init):
    Statement(NodeType::VARIABLE_DECLARATION), name(name), type(type),
    init(std::move(init)) {}

CallExpression::CallExpression(const std::string& callee,
    std::vector<std::unique_ptr<Expression>> arguments):
    Expression(NodeType::CALL_EXPRESSION), callee(callee), arguments(std::move(arguments)) {}
