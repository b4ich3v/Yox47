#include "Statements.h"

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression) :
	Statement(NodeType::EXPRESSION_STATEMENT), expression(std::move(expression)) {}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> value):
	Statement(NodeType::RETURN_STATEMENT), value(std::move(value)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> condition,
	std::unique_ptr<Statement> thenC, std::unique_ptr<Statement> elseC):
	Statement(NodeType::IF_STATEMENT), condition(std::move(condition)), thenConsequence(std::move(thenC)), elseConsequence(std::move(elseC)) {}

BlockStatement::BlockStatement(): Statement(NodeType::BLOCK_STATEMENT) {}

BreakStatement::BreakStatement(): Statement(NodeType::BREAK_STATEMENT) {}

WhileStatement::WhileStatement(std::unique_ptr<Expression>  condition,
	std::unique_ptr<Statement>  body):
	Statement(NodeType::WHILE_STATEMENT), condition(std::move(condition)),
	body(std::move(body)) {}

ForStatement::ForStatement(std::unique_ptr<Statement>  init,
	std::unique_ptr<Expression> condition,
	std::unique_ptr<Expression> post,
	std::unique_ptr<Statement> body):
	Statement(NodeType::FOR_STATEMENT), init(std::move(init)),
	condition(std::move(condition)), post(std::move(post)), body(std::move(body)) {}

CaseClause::CaseClause(std::unique_ptr<Expression> test, std::unique_ptr<Statement> body):
	test(std::move(test)), body(std::move(body)) {}

ChooseStatement::ChooseStatement(std::unique_ptr<Expression> expression,
	std::vector<CaseClause> cases,
	std::unique_ptr<Statement> defaultCase):
	Statement(NodeType::CHOOSE_STATEMENT), expression(std::move(expression)),
	cases(std::move(cases)), defaultCase(std::move(defaultCase)) {}
