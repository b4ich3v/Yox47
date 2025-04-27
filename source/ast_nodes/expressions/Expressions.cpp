#include "Expressions.h"

IdentifierExpression::IdentifierExpression(const std::string& name): Expression(NodeType::IDENTIFIER), name(name) {}

IntegerLitExpression::IntegerLitExpression(const std::string& text): Expression(NodeType::INT_LITERAL), text(text) {}

FloatLitExpression::FloatLitExpression(const std::string& text): Expression(NodeType::FLOAT_LITERAL), text(text) {}

BinaryExpression::BinaryExpression(const TokenType& oper, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right):
	Expression(NodeType::BINARY_EXPRESSION), oper(oper), left(std::move(left)), right(std::move(right)) {}

CharLitExpression::CharLitExpression(const std::string& text): 
	Expression(NodeType::CHAR_LITERAL), text(text) {}

BoolLitExpression::BoolLitExpression(bool value): 
	Expression(NodeType::BOOL_LITERAL), value(value) {}

UnaryExpression::UnaryExpression(TokenType oper, std::unique_ptr<Expression> expression): 
	Expression(NodeType::UNARY_EXPRESSION), oper(oper), operand(std::move(expression)) {}

BoxLiteral::BoxLiteral(std::vector<std::unique_ptr<Expression>> elements): 
	Expression(NodeType::BOX_LITERAL), elements(std::move(elements)) {}

IndexExpression::IndexExpression(std::unique_ptr<Expression> base,
	std::unique_ptr<Expression> index): 
	Expression(NodeType::INDEX_EXPRESSION), base(std::move(base)), index(std::move(index)) {}

AssignmentExpression::AssignmentExpression(std::unique_ptr<Expression> target,
	std::unique_ptr<Expression> value):
	Expression(NodeType::ASSIGNMENT_EXPRESSION),
	target(std::move(target)), value(std::move(value)) {}
