#include "Parser.h"

void Parser::process() 
{

	currentToken = lexer.nextToken();

}

bool Parser::checkToken(TokenType type) const 
{

	return currentToken.type == type;

}

bool Parser::matcher(TokenType type) 
{

	if (!checkToken(type)) return false;
	process();
	return true;

}

void Parser::expectManagement(TokenType type, const char* message) 
{

	if (!checkToken(type)) throw std::runtime_error(message);
	process();

}

int Parser::priorityManagement(TokenType type)
{

	switch (type) 
	{

	case TokenType::EQUAL_EQUAL:
	case TokenType::NEGATIVE_EQUAL: return 1;
	case TokenType::PLUS:
	case TokenType::MINUS: return 2;
	case TokenType::STAR:
	case TokenType::SLASH: return 3;
	default: return 0;
		
	}

}

std::unique_ptr<Program> Parser::parseProgram() 
{

	auto programPtr = std::make_unique<Program>();

	while (!checkToken(TokenType::END_OF_FILE))
		programPtr->functions.push_back(parseFunction());

	return programPtr;

}

std::unique_ptr<FunctionDeclaration> Parser::parseFunction() 
{

	expectManagement(TokenType::KEY_WORD_FUNCTION, "expected 'function'");
	if (!checkToken(TokenType::IDENTIFIER)) throw std::runtime_error("expected function name");

	std::string name(currentToken.startPtr, currentToken.length);
	process();

	expectManagement(TokenType::LPAREN, "expected '('");
	expectManagement(TokenType::RPAREN, "expected ')'"); 

	auto body = parseBlock();

	return std::make_unique<FunctionDeclaration>(name, std::move(body));

}

std::unique_ptr<BlockStatement> Parser::parseBlock()
{

	expectManagement(TokenType::LBRACE, "expected '{'");
	auto parsedBlock = std::make_unique<BlockStatement>();

	while (!checkToken(TokenType::RBRACE))
		parsedBlock->statements.push_back(parseStatement());

	expectManagement(TokenType::RBRACE, "expected '}'");

	return parsedBlock;

}

std::unique_ptr<Statement> Parser::parseStatement()
{

	if (checkToken(TokenType::KEY_WORD_IF)) return parseIf();
	if (checkToken(TokenType::KEY_WORD_RETURN)) return parseReturn();
	if (checkToken(TokenType::LBRACE)) return parseBlock();

	auto parsedExpression = parseExpression();
	expectManagement(TokenType::SEMICOLON, "expected ';'");

	return std::make_unique<ExpressionStatement>(std::move(parsedExpression));

}

std::unique_ptr<Statement> Parser::parseIf() 
{

	expectManagement(TokenType::KEY_WORD_IF, "expected 'if'");
	expectManagement(TokenType::LPAREN, "expected '('");

	auto parsedCondition = parseExpression();
	expectManagement(TokenType::RPAREN, "expected ')'");

	auto parsedThenC = parseStatement();
	std::unique_ptr<Statement> elseC = nullptr;
	if (matcher(TokenType::KEY_WORD_ELSE)) elseC = parseStatement();

	return std::make_unique<IfStatement>(std::move(parsedCondition), std::move(parsedThenC), std::move(elseC));

}

std::unique_ptr<Statement> Parser::parseReturn() 
{

	expectManagement(TokenType::KEY_WORD_RETURN, "expected 'return'");

	std::unique_ptr<Expression> parsedValue = nullptr;
	if (!checkToken(TokenType::SEMICOLON)) parsedValue = parseExpression();

	expectManagement(TokenType::SEMICOLON, "expected ';'");

	return std::make_unique<ReturnStatement>(std::move(parsedValue));

}

std::unique_ptr<Expression> Parser::parseExpression(int minPrecedence) 
{

	auto left = parsePrimary();

	while (true)
	{

		int precedence = priorityManagement(currentToken.type);
		if (precedence < minPrecedence) break;

		TokenType op = currentToken.type;
		process();

		auto right = parseExpression(precedence + 1);
		left = std::make_unique<BinaryExpression>(op, std::move(left), std::move(right));

	}

	return left;

}

std::unique_ptr<Expression> Parser::parsePrimary() 
{

	if (checkToken(TokenType::IDENTIFIER)) 
	{

		std::string name(currentToken.startPtr, currentToken.length);
		process();

		return std::make_unique<IdentifierExpression>(name);

	}

	if (checkToken(TokenType::INT)) 
	{

		std::string text(currentToken.startPtr, currentToken.length);
		process();

		return std::make_unique<IntegerLitExpression>(text);

	}

	if (checkToken(TokenType::FLOAT)) 
	{

		std::string text(currentToken.startPtr, currentToken.length);
		process();

		return std::make_unique<FloatLitExpression>(text);

	}

	if (matcher(TokenType::LPAREN)) 
	{

		auto resultExpression = parseExpression();
		expectManagement(TokenType::RPAREN, "expected ')'");

		return resultExpression;

	}

	throw std::runtime_error("unexpected token in primary expression");

}

Parser::Parser(Lexer& lexer): lexer(lexer)
{

	process();

}
