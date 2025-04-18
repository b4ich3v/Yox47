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

	throw std::runtime_error("expected type specifier");

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
		checkToken(TokenType::KEY_WORD_CHAR))
		return parseVariableDeclaration();          

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
		if (precedence == 0 || precedence < minPrecedence) break;

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

		if (checkToken(TokenType::LPAREN)) 
		{

			process();                                  
			std::vector<std::unique_ptr<Expression>> arguments;

			if (!checkToken(TokenType::RPAREN)) 
			{

				do { arguments.push_back(parseExpression()); } while (matcher(TokenType::COMMA));

			}

			expectManagement(TokenType::RPAREN, "expected ')'");
			return std::make_unique<CallExpression>(name, std::move(arguments));

		}

		return std::make_unique<IdentifierExpression>(std::string(name));

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

	std::unique_ptr<Expression> init = nullptr;
	if (matcher(TokenType::ASSIGN)) init = parseExpression();
	expectManagement(TokenType::SEMICOLON, "';'");

	return std::make_unique<VariableDeclaration>(variableName, variableType, std::move(init));

}

Parser::Parser(Lexer& lexer): lexer(lexer)
{

	process();

}
