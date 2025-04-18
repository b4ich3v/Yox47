#include "Lexer.h"

Token::Token(TokenType type, const char* startPtr, uint32_t length, uint32_t currentLine, uint32_t currentColumn):
	type(type), startPtr(startPtr), length(length), currentLine(currentLine), currentColumn(currentColumn) {}

Lexer::Lexer(const char* data)
{

	if (!data) throw std::logic_error("Invalid pointer data");

	beginPtr = data;
	endPtr = (data + (strlen(data)));

}

char Lexer::process() 
{

	if (currentPtr >= endPtr)  return '\0';

	char result = *currentPtr;
	currentPtr += 1;

	if (result == '\n')
	{

		currentLine += 1;
		currentColumn = 1;

	}
	else
	{

		currentColumn += 1;

	}

	return result;

}

bool Lexer::match(char expectedSymbol) 
{

	if (peek() != expectedSymbol) return false;

	process();
	return true;

}

Token Lexer::generateToken(TokenType type, const char* ptr) 
{

	if (!ptr) throw std::logic_error("Error");
	return Token(type, ptr, strlen(ptr), currentLine, currentColumn);

}

void Lexer::trim() 
{

	while (true) 
	{

		char currentSymbol = peek();

		switch (currentSymbol) 
		{

		case ' ': case '\t': case '\r': case '\n': process(); continue;
		case '/':

			if (peekNext() == '/') 
			{
				while (peek() != '\n' && peek() != '\0') process();
				continue;
			}

			break;

		default: break;

		}

		break;

	}

}

Token Lexer::nextToken() 
{

	trim();

	const char* tokenStart = currentPtr;
	char currentSymbol = process();

	switch (currentSymbol)
	{

	case '\0': return { TokenType::END_OF_FILE, currentPtr, 0, currentLine, currentColumn };
	case '+':  return generateToken(TokenType::PLUS, tokenStart);
	case '-':  return generateToken(TokenType::MINUS, tokenStart);
	case '*':  return generateToken(TokenType::STAR, tokenStart);
	case '/':  return generateToken(TokenType::SLASH, tokenStart);
	case '(':  return generateToken(TokenType::LPAREN, tokenStart);
	case ')':  return generateToken(TokenType::RPAREN, tokenStart);
	case '{':  return generateToken(TokenType::LBRACE, tokenStart);
	case '}':  return generateToken(TokenType::RBRACE, tokenStart);
	case ';':  return generateToken(TokenType::SEMICOLON, tokenStart);
	case ',':  return generateToken(TokenType::COMMA, tokenStart);
	case '!':  return generateToken(match('=') ? TokenType::NEGATIVE_EQUAL : TokenType::END_OF_FILE, tokenStart);
	case '=':  return generateToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN, tokenStart);
	default:

		if (isAlnum(currentSymbol) || currentSymbol == '_') return identifier(tokenStart);
		if (isDigit(currentSymbol)) return number(tokenStart);
		throw std::runtime_error("Unexpected character in lexer");

	}

}

Token Lexer::identifier(const char* ptr) 
{

	while (isAlnum(peek()) || peek() == '_') process();
	std::string variableId(ptr, currentPtr - ptr);
	return generateToken(keywordMatcher(variableId), ptr);

}

Token Lexer::number(const char* ptr) 
{

	bool isFloat = false;

	while (isDigit(peek())) process();

	if (peek() == '.' && isDigit(peekNext()))
	{

		isFloat = true;
		process(); 
		while (isDigit(peek())) process();

	}

	if (isFloat) return generateToken(TokenType::FLOAT, ptr);
	else return generateToken(TokenType::INT, ptr);

}

bool Lexer::isAlnum(char ch)
{

	return (ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z');

}

bool Lexer::isDigit(char ch) 
{

	return ch >= '0' && ch <= '9';

}

TokenType Lexer::keywordMatcher(const std::string& variableId)
{

	if (hashedKeywords.find(variableId) != hashedKeywords.end()) return hashedKeywords[variableId];
	return TokenType::IDENTIFIER;

}

char Lexer::peek() 
{

	if (currentPtr < endPtr) return *currentPtr;
	else return '\0';

}

char Lexer::peekNext()
{

	if (currentPtr + 1 < endPtr) return *(currentPtr + 1);
	else return '\0';

}

void Lexer::tokenize() 
{

	const char* iter = beginPtr;
	Token currentToken;

	while (iter != endPtr) 
	{

		currentToken = nextToken();
		tokens.push_back(currentToken);
		iter += 1;

	}

}
