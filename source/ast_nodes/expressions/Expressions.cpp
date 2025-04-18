#include "Expressions.h"

IdentifierExpression::IdentifierExpression(const std::string& name): Expression(NodeType::IDENTIFIER), name(name) {}

IntegerLitExpression::IntegerLitExpression(const std::string& text): Expression(NodeType::INT_LITERAL), text(text) {}

FloatLitExpression::FloatLitExpression(const std::string& text): Expression(NodeType::FLOAT_LITERAL), text(text) {}

BinaryExpression::BinaryExpression(const TokenType& oper, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right): 
	Expression(NodeType::BINARY_EXPRESSION), oper(oper), left(std::move(left)), right(std::move(right)) {}
