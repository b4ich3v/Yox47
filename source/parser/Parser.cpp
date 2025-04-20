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

VariableType Parser::parseType()
{

	if (matcher(TokenType::KEY_WORD_INT)) return VariableType::Int;
	if (matcher(TokenType::KEY_WORD_FLOAT)) return VariableType::Float;
	if (matcher(TokenType::KEY_WORD_CHAR)) return VariableType::Char;
	if (matcher(TokenType::KEY_WORD_BOOL)) return VariableType::Bool;
	if (matcher(TokenType::KEY_WORD_BOX)) return VariableType::Box;

	throw std::runtime_error("expected type specifier");

}

int Parser::priorityManagement(TokenType type)
{

	switch (type)
	{

	case TokenType::LOGICAL_OR: return 1;
	case TokenType::LOGICAL_AND: return 2;
	case TokenType::ASSIGN: return 3;
	case TokenType::EQUAL_EQUAL:
	case TokenType::NOT_EQUAL: return 4;
	case TokenType::LESS:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER:
	case TokenType::GREATER_EQUAL: return 5;
	case TokenType::PLUS:
	case TokenType::MINUS: return 6;
	case TokenType::STAR:
	case TokenType::SLASH: return 7;
	case TokenType::LBRACKET: return 8;
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

	if (!checkToken(TokenType::IDENTIFIER))
		throw std::runtime_error("expected function name");

	std::string functionName(currentToken.startPtr, currentToken.length);
	process();

	expectManagement(TokenType::LPAREN, "expected '('");
	std::vector<Parameter> params;

	if (!checkToken(TokenType::RPAREN))
	{

		do
		{

			if (!checkToken(TokenType::IDENTIFIER))
				throw std::runtime_error("expected parameter name");

			std::string pName(currentToken.startPtr, currentToken.length);
			process();

			expectManagement(TokenType::COLON, "expected ':' after parameter name");
			VariableType pType = parseType();

			params.push_back({ pName, pType });

		} while (matcher(TokenType::COMMA));

	}

	expectManagement(TokenType::RPAREN, "expected ')'");
	auto body = parseBlock();

	auto functionNameResult = std::make_unique<FunctionDeclaration>(functionName, std::move(body));
	functionNameResult->parameters = std::move(params);

	return functionNameResult;

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

	if (checkToken(TokenType::KEY_WORD_INT) ||
		checkToken(TokenType::KEY_WORD_FLOAT) ||
		checkToken(TokenType::KEY_WORD_CHAR) ||
		checkToken(TokenType::KEY_WORD_BOOL) ||
		checkToken(TokenType::KEY_WORD_BOX))
		return parseVariableDeclaration();

	if (checkToken(TokenType::KEY_WORD_CHOOSE)) return parseChoose();
	if (checkToken(TokenType::KEY_WORD_IF)) return parseIf();
	if (checkToken(TokenType::KEY_WORD_RETURN)) return parseReturn();
	if (checkToken(TokenType::LBRACE)) return parseBlock();
	if (checkToken(TokenType::KEY_WORD_FOR)) return parseFor();
	if (checkToken(TokenType::KEY_WORD_WHILE)) return parseWhile();
	if (checkToken(TokenType::KEY_WORD_BREAK))
	{

		process();
		expectManagement(TokenType::SEMICOLON, "';'");

		return std::make_unique<BreakStatement>();

	}

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
		if (precedence == 0 || precedence < minPrecedence) break;

		TokenType oper = currentToken.type;
		process();

		int nextMin = (oper == TokenType::ASSIGN) ? precedence : precedence + 1;

		auto right = parseExpression(nextMin);

		left = std::make_unique<BinaryExpression>(oper,
			std::move(left), std::move(right));

	}

	return left;

}

std::unique_ptr<Expression> Parser::parsePrimary()
{

	if (matcher(TokenType::LBRACKET))
	{

		std::vector<std::unique_ptr<Expression>> elements;

		if (!checkToken(TokenType::RBRACKET))
		{

			do { elements.push_back(parseExpression()); } while (matcher(TokenType::COMMA));

		}

		expectManagement(TokenType::RBRACKET, "expected ']'");
		return std::make_unique<BoxLiteral>(std::move(elements));

	}

	if (checkToken(TokenType::IDENTIFIER))
	{

		std::string name(currentToken.startPtr, currentToken.length);
		process();

		std::unique_ptr<Expression> expression =
			std::make_unique<IdentifierExpression>(name);

		while (matcher(TokenType::LBRACKET))
		{

			auto index = parseExpression();

			expectManagement(TokenType::RBRACKET, "expected ']'");
			expression = std::make_unique<IndexExpression>(std::move(expression),
				std::move(index));

		}

		return expression;

	}

	if (checkToken(TokenType::NEGATION))
	{

		process();
		auto operand = parsePrimary();

		return std::make_unique<UnaryExpression>(TokenType::NEGATION,
			std::move(operand));

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

	if (checkToken(TokenType::BOOL_LITERAL))
	{

		bool value = (currentToken.length == 4);
		process();

		return std::make_unique<BoolLitExpression>(value);

	}

	if (checkToken(TokenType::CHAR_LITERAL))
	{

		std::string text(currentToken.startPtr, currentToken.length);
		process();

		return std::make_unique<CharLitExpression>(text);

	}

	if (matcher(TokenType::LPAREN))
	{

		auto expression = parseExpression();
		expectManagement(TokenType::RPAREN, "expected ')'");

		return expression;

	}

	throw std::runtime_error("unexpected token in primary expression");

}

std::unique_ptr<Statement> Parser::parseVariableDeclaration()
{

	VariableType variableType = parseType();

	if (!checkToken(TokenType::IDENTIFIER))
		throw std::runtime_error("expected variable name");

	std::string variableName(currentToken.startPtr, currentToken.length);
	process();

	while (matcher(TokenType::LBRACKET))
	{

		parseExpression();
		expectManagement(TokenType::RBRACKET, "expected ']'");

	}

	std::unique_ptr<Expression> init = nullptr;

	if (matcher(TokenType::ASSIGN))
	{

		if (matcher(TokenType::LBRACE))
		{

			std::vector<std::unique_ptr<Expression>> elements;

			if (!checkToken(TokenType::RBRACE))
			{

				do { elements.push_back(parseExpression()); } while (matcher(TokenType::COMMA));

			}

			expectManagement(TokenType::RBRACE, "'}'");
			init = std::make_unique<BoxLiteral>(std::move(elements));

		}
		else
		{

			init = parseExpression();

		}

	}

	expectManagement(TokenType::SEMICOLON, "';'");

	return std::make_unique<VariableDeclaration>(variableName, variableType, std::move(init));

}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclarationNoSemi()
{

	VariableType variableType = parseType();

	if (!checkToken(TokenType::IDENTIFIER))
		throw std::runtime_error("expected variable name");

	std::string variableName(currentToken.startPtr, currentToken.length);
	process();

	std::unique_ptr<Expression> parsedInit = nullptr;
	if (matcher(TokenType::ASSIGN))
		parsedInit = parseExpression();

	return std::make_unique<VariableDeclaration>(variableName, variableType, std::move(parsedInit));

}

std::unique_ptr<Statement> Parser::parseWhile()
{

	expectManagement(TokenType::KEY_WORD_WHILE, "'while'");
	expectManagement(TokenType::LPAREN, "'('");

	auto parsedCondition = parseExpression();

	expectManagement(TokenType::RPAREN, "')'");
	auto parsedBody = parseStatement();

	return std::make_unique<WhileStatement>(std::move(parsedCondition), std::move(parsedBody));

}

std::unique_ptr<Statement> Parser::parseFor()
{

	expectManagement(TokenType::KEY_WORD_FOR, "'for'");
	expectManagement(TokenType::LPAREN, "'('");

	std::unique_ptr<Statement> parsedInit = nullptr;

	if (!checkToken(TokenType::SEMICOLON))
	{

		if (checkToken(TokenType::KEY_WORD_INT) ||
			checkToken(TokenType::KEY_WORD_FLOAT) ||
			checkToken(TokenType::KEY_WORD_CHAR) ||
			checkToken(TokenType::KEY_WORD_BOOL) ||
			checkToken(TokenType::KEY_WORD_BOX))
			parsedInit = parseVariableDeclarationNoSemi();
		else
			parsedInit = std::make_unique<ExpressionStatement>(parseExpression());

	}

	expectManagement(TokenType::SEMICOLON, "';'");

	std::unique_ptr<Expression> parsedCondition = nullptr;

	if (!checkToken(TokenType::SEMICOLON))
		parsedCondition = parseExpression();

	expectManagement(TokenType::SEMICOLON, "';'");

	std::unique_ptr<Expression> parsedPost = nullptr;

	if (!checkToken(TokenType::RPAREN))
		parsedPost = parseExpression();

	expectManagement(TokenType::RPAREN, "')'");

	auto parsedBody = parseStatement();
	return std::make_unique<ForStatement>(std::move(parsedInit),
		std::move(parsedCondition), std::move(parsedPost), std::move(parsedBody));

}

std::unique_ptr<Statement> Parser::parseChoose()
{

	expectManagement(TokenType::KEY_WORD_CHOOSE, "expected 'choose'");
	expectManagement(TokenType::LPAREN, "expected '('");

	auto parsedExpression = parseExpression();

	expectManagement(TokenType::RPAREN, "expected ')'");
	expectManagement(TokenType::LBRACE, "expected '{'");

	std::vector<CaseClause> cases;
	std::unique_ptr<Statement> parsedDefaultStatement = nullptr;

	while (!checkToken(TokenType::RBRACE))
	{

		if (checkToken(TokenType::KEY_WORD_CASE))
		{

			process();
			auto parsedTestExpression = parseExpression();
			expectManagement(TokenType::COLON, "expected ':'");
			auto parsedBodyStatement = parseStatement();
			cases.emplace_back(std::move(parsedTestExpression), std::move(parsedBodyStatement));

		}
		else if (checkToken(TokenType::KEY_WORD_DEFAULT))
		{

			process();
			expectManagement(TokenType::COLON, "expected ':'");
			parsedDefaultStatement = parseStatement();

		}
		else
		{

			throw std::runtime_error("expected 'case' or 'default'");

		}

	}

	expectManagement(TokenType::RBRACE, "expected '}'");
	return std::make_unique<ChooseStatement>(std::move(parsedExpression), 
		std::move(cases), std::move(parsedDefaultStatement));

}

Parser::Parser(Lexer& lexer): lexer(lexer)
{

	process();

}
