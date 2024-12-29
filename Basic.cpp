#include "Basic.h"

Error::Error() : name(""), details(""), position(-1) {}

Error::Error(const std::string& inputName, const std::string& inputDetails, int inputPosition)
	: name(inputName), details(inputDetails), position(inputPosition) {}

std::string Error::asString() const
{

    if (position >= 0) return name + " at position " + std::to_string(position) + ": " + details;
    else return name + ": " + details;

}

IllegalCharError::IllegalCharError(const std::string& details, int position)
    : Error("Illegal Character", details, position) {}

Token::Token(const std::string& inputType, double inputValue) : type(inputType), value(inputValue) {}

Token::Token(const std::string& inputType) : type(inputType), value(0) {}

void Token::printToken() const
{

    if (type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT || type == TOKEN_TYPE_DOUBLE) std::cout << type << ": " << value;
    else std::cout << type;

}

std::string Token::getType() const { return type; }

double Token::getValue() const { return value; }

void LexerResult::printMethod() const 
{

    for (const auto& token : tokens)
    {

        token.printToken();
        std::cout << " ";

    }

}

Lexer::Lexer(const std::string& inputText) : text(inputText) { advance(); }

void Lexer::advance()
{

    currentPosition += 1;
    if (currentPosition < static_cast<int>(text.size()))
        currentSymbol = text[currentPosition];
    else
        currentSymbol = END_SYMBOL;

}

bool Lexer::isDigit(char ch)
{

    return std::isdigit(static_cast<unsigned char>(ch));

}

Token Lexer::makeNumber()
{

    std::string strNumber = "";
    int dotCount = 0;

    while (currentSymbol != END_SYMBOL && (isDigit(currentSymbol) || currentSymbol == '.'))
    {

        if (currentSymbol == '.')
        {

            if (dotCount == 1) break;
            dotCount += 1;
            strNumber += '.';

        }
        else
        {

            strNumber += currentSymbol;

        }

        advance();

    }

    std::string type = TOKEN_TYPE_DOUBLE;

    if (currentSymbol == 'f' || currentSymbol == 'F')
    {

        type = TOKEN_TYPE_FLOAT;
        advance();

    }
    else if (currentSymbol == 'd' || currentSymbol == 'D')
    {

        type = TOKEN_TYPE_DOUBLE;
        advance();

    }

    double value = 0;

    try
    {

        if (dotCount == 0)
        {

            type = TOKEN_TYPE_INT;
            value = std::stoi(strNumber);

        }
        else
        {

            value = std::stod(strNumber);

        }

    }
    catch (const std::invalid_argument& e)
    {

        throw IllegalCharError("Invalid number format: " + strNumber, currentPosition);

    }
    catch (const std::out_of_range& e)
    {

        throw IllegalCharError("Number out of range: " + strNumber, currentPosition);

    }

    return Token(type, value);

}

LexerResult Lexer::makeTokens()
{

    LexerResult result;

    while (currentSymbol != END_SYMBOL)
    {

        if (currentSymbol == ' ' || currentSymbol == '\t')
        {

            advance();

        }
        else if (isDigit(currentSymbol))
        {

            try
            {

                result.tokens.push_back(makeNumber());

            }
            catch (const IllegalCharError& e)
            {

                result.error = std::make_unique<IllegalCharError>(e);
                break;

            }

        }
        else if (currentSymbol == '+')
        {

            result.tokens.emplace_back(TOKEN_TYPE_PLUS);
            advance();

        }
        else if (currentSymbol == '-')
        {

            result.tokens.emplace_back(TOKEN_TYPE_MINUS);
            advance();

        }
        else if (currentSymbol == '*')
        {

            result.tokens.emplace_back(TOKEN_TYPE_MULT);
            advance();

        }
        else if (currentSymbol == '/')
        {

            result.tokens.emplace_back(TOKEN_TYPE_DIV);
            advance();

        }
        else if (currentSymbol == '(')
        {

            result.tokens.emplace_back(TOKEN_TYPE_LPAREN);
            advance();

        }
        else if (currentSymbol == ')')
        {

            result.tokens.emplace_back(TOKEN_TYPE_RPAREN);
            advance();

        }
        else
        {

            char current = currentSymbol;
            int errorPosition = currentPosition;
            advance();

            result.error = std::make_unique<IllegalCharError>(std::string(1, current), errorPosition);
            break;
        }

    }

    return result;

}

LexerResult run(const std::string& text)
{

    Lexer lexer(text);
    LexerResult tokens = lexer.makeTokens();
    return tokens;

}