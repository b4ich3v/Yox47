#include "Statements.h"

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression): 
	Statement(NodeType::EXPRESSION_STATMENT), expression(std::move(expression)) {}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> value): 
	Statement(NodeType::RETURN_STATEMENT), value(std::move(value)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> condition, 
	std::unique_ptr<Statement> thenC, std::unique_ptr<Statement> elseC):
	Statement(NodeType::IF_STATEMENT), condition(std::move(condition)), thenConsequence(std::move(thenC)), elseConsequence(std::move(elseC)) {}

BlockStatement::BlockStatement(): Statement(NodeType::BLOCK_STATMENT) {}
