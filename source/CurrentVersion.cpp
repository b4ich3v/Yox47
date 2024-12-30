#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <stdexcept>

const std::string DIGITS = "0123456789";

const std::string TOKEN_TYPE_INT = "INT";
const std::string TOKEN_TYPE_FLOAT = "FLOAT";
const std::string TOKEN_TYPE_LPAREN = "LPAREN";
const std::string TOKEN_TYPE_RPAREN = "RPAREN";
const std::string TOKEN_TYPE_MINUS = "MINUS";
const std::string TOKEN_TYPE_PLUS = "PLUS";
const std::string TOKEN_TYPE_MULT = "MULT";
const std::string TOKEN_TYPE_DIV = "DIV";
const std::string TOKEN_TYPE_EOF = "EOF";

const char END_SYMBOL = '\0';

class Position 
{
public:

    int index;
    int line;
    int column;
    std::string fn;
    std::string ftxt;

    Position() : index(-1), line(0), column(-1), fn(""), ftxt("") {}

    Position(int inputIndex, int inputLine, int inputColumn, const std::string& inputFn, const std::string& inputFtxt)
        : index(inputIndex), line(inputLine), column(inputColumn), fn(inputFn), ftxt(inputFtxt) {}

    void advance(char current_char = '\0')
    {

        index += 1;
        column += 1;

        if (current_char == '\n')
        {

            line += 1;
            column = 0;

        }

    }

    Position copy() const 
    {

        return Position(index, line, column, fn, ftxt);

    }

};

class Error 
{
protected:

    std::string name;
    std::string details;
    Position positionStart;
    Position positionEnd;

public:

    Error() : name(""), details(""), positionStart(), positionEnd() {}
    Error(const std::string& inputName, const std::string& inputDetails, const Position& start, const Position& end)
        : name(inputName), details(inputDetails), positionStart(start), positionEnd(end) {}

    virtual std::string asString() const
    {

        std::stringstream ss;
        ss << name << ": " << details << "\n";
        ss << "File " << positionStart.fn << ", line " << positionStart.line + 1 << ", column " << positionStart.column + 1;
        ss << "\n\n" << stringWithArrows();
        return ss.str();

    }

    std::string stringWithArrows() const 
    {

        std::stringstream ss;
        std::string text = positionStart.ftxt;
        int lineNumber = positionStart.line;

        std::vector<std::string> lines;
        std::stringstream textStream(text);
        std::string line;

        while (std::getline(textStream, line, '\n')) lines.push_back(line);

        if (lineNumber >= static_cast<int>(lines.size())) 
        {

            ss << "No context available.";
            return ss.str();

        }

        std::string errorLine = lines[lineNumber];
        ss << errorLine << "\n";
        std::string caretLine = "";

        for (int i = 0; i < positionStart.column && i < static_cast<int>(errorLine.size()); i++)
        {

            if (errorLine[i] == '\t') caretLine += "\t";
            else caretLine += " ";

        }

        caretLine += "^";
        ss << caretLine;

        return ss.str();

    }

};

class IllegalCharError : public Error 
{
public:

    IllegalCharError(const std::string& details, const Position& start, const Position& end)
        : Error("Illegal Character", details, start, end) {}

};

class InvalidSyntaxError : public Error 
{
public:

    InvalidSyntaxError(const Position& start, const Position& end, const std::string& details = "")
        : Error("Invalid Syntax", details, start, end) {}

};

class Token
{
private:

    std::string type;
    double value;

public:

    Position positionStart;
    Position positionEnd;

    Token() : type("NONE"), value(0), positionStart(), positionEnd() {}

    Token(const std::string& inputType, double inputValue = 0, const Position& start = Position(), const Position& end = Position())
        : type(inputType), value(inputValue), positionStart(start), positionEnd(end) {}

    std::string getType() const { return type; }

    double getValue() const { return value; }

    void printToken() const 
    {

        if (type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT) std::cout << type << ":" << value;       
        else std::cout << type;
            
    }

    friend std::ostream& operator << (std::ostream& os, const Token& tok)
    {

        if (tok.type == TOKEN_TYPE_INT || tok.type == TOKEN_TYPE_FLOAT) os << tok.type << ":" << tok.value;  
        else os << tok.type;
            
        return os;

    }

};

struct LexerResult 
{
public:

    std::vector<Token> tokens;
    std::unique_ptr<Error> error = nullptr;

    void printTokens() const 
    {

        for (const auto& token : tokens)
        {

            token.printToken();
            std::cout << " ";

        }

        std::cout << std::endl;

    }

    void printError() const 
    {

        if (error) std::cout << error->asString() << std::endl;
            
    }

};

class Node 
{
public:

    virtual void printNode() const = 0;

};

class NumberNode : public Node
{
public:

    Token token;

    NumberNode(const Token& input) : token(input) {}

    void printNode() const override 
    {

        std::cout << token;

    }

};

class BinOpNode : public Node
{
public:

    std::shared_ptr<Node> leftNode;
    Token operationToken;
    std::shared_ptr<Node> rightNode;

    BinOpNode(const std::shared_ptr<Node>& left, const Token& op, const std::shared_ptr<Node>& right)
        : leftNode(left), operationToken(op), rightNode(right) {}

    void printNode() const override 
    {

        std::cout << "(";
        leftNode->printNode();
        std::cout << ", " << operationToken << ", ";
        rightNode->printNode();
        std::cout << ")";

    }

};

class UnaryOpNode : public Node 
{
public:

    Token operationToken;
    std::shared_ptr<Node> node;

    UnaryOpNode(const Token& op, const std::shared_ptr<Node>& node_)
        : operationToken(op), node(node_) {}

    void printNode() const override 
    {

        std::cout << "(" << operationToken << ", ";
        node->printNode();
        std::cout << ")";

    }

};

class ParseResult
{
public:

    std::shared_ptr<Node> node = nullptr;
    std::unique_ptr<Error> error = nullptr;

    std::shared_ptr<Node> registerNode(const std::shared_ptr<Node>& resultNode)
    {

        if (resultNode) node = resultNode;
        return resultNode;

    }

    void registerError(std::unique_ptr<Error> resultNode)
    {

        if (resultNode) error = std::move(resultNode);
            
    }

    bool hasError() const 
    {

        return error != nullptr;

    }

    void success(const std::shared_ptr<Node>& resultNode) 
    {

        node = resultNode;

    }

    void failure(std::unique_ptr<Error> resultNode) 
    {

        error = std::move(resultNode);

    }

};

class ParserClass 
{ 
private:

    std::vector<Token> tokens;
    int tokenIndex;
    Token currentToken;

public:

    ParserClass(const std::vector<Token>& inputTokens)
        : tokens(inputTokens), tokenIndex(-1), currentToken() { advance(); }

    void advance() 
    {

        tokenIndex += 1;
        if (tokenIndex < static_cast<int>(tokens.size())) currentToken = tokens[tokenIndex];

    }

    std::pair<std::shared_ptr<Node>, std::unique_ptr<Error>> parse()
    {

        ParseResult result;
        auto node = expr(result); 

        if (!result.hasError() && currentToken.getType() != TOKEN_TYPE_EOF) 
        {
            
            return { nullptr, std::make_unique<InvalidSyntaxError>(
                tokens[tokenIndex].positionStart, tokens[tokenIndex].positionEnd,
                "Expected '+', '-', '*', '/', or EOF"
            ) };

        }

        return { result.node, std::move(result.error) };

    }

private:

    std::shared_ptr<Node> factor(ParseResult& result) 
    {

        Token token = currentToken;

        if (token.getType() == TOKEN_TYPE_PLUS || token.getType() == TOKEN_TYPE_MINUS)
        {

            advance(); 
            auto factorNode = factor(result);
            if (result.hasError()) return nullptr;
            auto node = std::make_shared<UnaryOpNode>(token, factorNode);
            result.registerNode(node); 
            return node;

        }

        if (token.getType() == TOKEN_TYPE_INT || token.getType() == TOKEN_TYPE_FLOAT)
        {

            advance(); 
            auto node = std::make_shared<NumberNode>(token);
            result.registerNode(node); 
            return node;

        }

        if (token.getType() == TOKEN_TYPE_LPAREN) 
        {

            advance();
            auto exprNode = expr(result);

            if (result.hasError()) return nullptr;

            if (currentToken.getType() == TOKEN_TYPE_RPAREN) 
            {

                advance(); 
                result.registerNode(exprNode);
                return exprNode;

            }
            else 
            {
              
                result.failure(std::make_unique<InvalidSyntaxError>(
                    currentToken.positionStart, currentToken.positionEnd,
                    "Expected ')'"
                ));
                return nullptr;

            }

        }

        result.failure(std::make_unique<InvalidSyntaxError>(
            token.positionStart, token.positionEnd,
            "Expected number or '('"
        ));
        return nullptr;

    }

    std::shared_ptr<Node> term(ParseResult& result)
    {

        auto left = factor(result);
        if (result.hasError()) return nullptr;

        while (currentToken.getType() == TOKEN_TYPE_MULT || currentToken.getType() == TOKEN_TYPE_DIV)
        {

            Token operationToken = currentToken;
            advance();
            auto right = factor(result);

            if (result.hasError()) return nullptr;

            auto binaryOperationToken = std::make_shared<BinOpNode>(left, operationToken, right);
            result.registerNode(binaryOperationToken); 
            left = binaryOperationToken;

        }

        return left;

    }

    std::shared_ptr<Node> expr(ParseResult& result) 
    {

        auto left = term(result);

        if (result.hasError()) return nullptr;

        while (currentToken.getType() == TOKEN_TYPE_PLUS || currentToken.getType() == TOKEN_TYPE_MINUS)
        {

            Token operationToken = currentToken;
            advance();
            auto right = term(result);

            if (result.hasError()) return nullptr;

            auto binaryOperationToken = std::make_shared<BinOpNode>(left, operationToken, right);
            result.registerNode(binaryOperationToken); 
            left = binaryOperationToken;

        }

        return left;

    }

};

class LexerClass 
{ 
private:

    std::string text;
    Position position;
    char currentSymbol;
    std::string fileName;

public:

    LexerClass(const std::string& inputText, const std::string& fn = "input")
        : text(inputText), position(-1, 0, -1, fn, inputText), fileName(fn)
    {

        advance();

    }

    void advance()
    {

        position.advance(currentSymbol);

        if (position.index < static_cast<int>(text.size()))  currentSymbol = text[position.index];   
        else currentSymbol = END_SYMBOL;
           
    }

    bool isDigit(char ch)
    {

        return std::isdigit(static_cast<unsigned char>(ch));

    }

    Token makeNumber()
    {

        std::string strNumber = "";
        int dotCount = 0;

        Position startPosition = position.copy();

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

        std::string type = TOKEN_TYPE_FLOAT;
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

            Position end_pos = position.copy();
            throw IllegalCharError("Invalid number format: " + strNumber, startPosition, end_pos);

        }
        catch (const std::out_of_range& e)
        {

            Position end_pos = position.copy();
            throw IllegalCharError("Number out of range: " + strNumber, startPosition, end_pos);

        }

        return Token(type, value, startPosition, position.copy());

    }

    LexerResult makeTokens()
    {

        LexerResult result;

        try 
        {

            while (currentSymbol != END_SYMBOL)
            {

                if (currentSymbol == ' ' || currentSymbol == '\t' || currentSymbol == '\n' || currentSymbol == '\r')
                {

                    advance();

                }
                else if (isDigit(currentSymbol))
                {

                    result.tokens.push_back(makeNumber());

                }
                else if (currentSymbol == '+')
                {

                    Token tok(TOKEN_TYPE_PLUS, 0, position.copy(), position.copy());
                    result.tokens.push_back(tok);
                    advance();

                }
                else if (currentSymbol == '-')
                {

                    Token tok(TOKEN_TYPE_MINUS, 0, position.copy(), position.copy());
                    result.tokens.push_back(tok);
                    advance();

                }
                else if (currentSymbol == '*')
                {

                    Token tok(TOKEN_TYPE_MULT, 0, position.copy(), position.copy());
                    result.tokens.push_back(tok);
                    advance();

                }
                else if (currentSymbol == '/')
                {

                    Token tok(TOKEN_TYPE_DIV, 0, position.copy(), position.copy());
                    result.tokens.push_back(tok);
                    advance();

                }
                else if (currentSymbol == '(')
                {

                    Token tok(TOKEN_TYPE_LPAREN, 0, position.copy(), position.copy());
                    result.tokens.push_back(tok);
                    advance();

                }
                else if (currentSymbol == ')')
                {

                    Token tok(TOKEN_TYPE_RPAREN, 0, position.copy(), position.copy()); 
                    result.tokens.push_back(tok);
                    advance();

                }
                else
                {

                    char current = currentSymbol;
                    Position startPosition = position.copy();
                    advance();
                    Position endPosition = position.copy();

                    result.error = std::make_unique<IllegalCharError>(std::string(1, current), startPosition, endPosition);
                    return result;

                }

            }

            Token eofTok(TOKEN_TYPE_EOF, 0, position.copy(), position.copy());
            result.tokens.push_back(eofTok);

        }
        catch (const IllegalCharError& e)
        {

            result.error = std::make_unique<IllegalCharError>(e);

        }

        return result;

    }

};

std::pair<std::shared_ptr<Node>, std::unique_ptr<Error>> run(const std::string& text, const std::string& fileName = "input")
{

    LexerClass lexer(text, fileName);
    LexerResult lexResult = lexer.makeTokens();

    if (lexResult.error) return { nullptr, std::move(lexResult.error) };
       
    ParserClass parser(lexResult.tokens);
    auto parseResult = parser.parse();

    return parseResult;

}

int main()
{

    std::string input;

    while (true)
    {

        std::cout << "Yox47 > ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        auto parseResult = run(input, "<stdin>");
        std::shared_ptr<Node> ast = parseResult.first;
        std::unique_ptr<Error> error = std::move(parseResult.second);

        if (error) std::cout << error->asString() << std::endl;
        else
        {

            std::shared_ptr<NumberNode> numberNode = std::dynamic_pointer_cast<NumberNode>(ast);
            if (numberNode) 
            {

                numberNode->printNode();
                std::cout << std::endl;
                continue;

            }

            std::shared_ptr<BinOpNode> binaryOperationNode = std::dynamic_pointer_cast<BinOpNode>(ast);
            if (binaryOperationNode) 
            {

                binaryOperationNode->printNode();
                std::cout << std::endl;
                continue;

            }

            std::shared_ptr<UnaryOpNode> unaryOperationNode = std::dynamic_pointer_cast<UnaryOpNode>(ast);
            if (unaryOperationNode)
            {

                unaryOperationNode->printNode();
                std::cout << std::endl;
                continue;

            }

            std::cout << "Parsed AST is empty or of unknown type." << std::endl;

        }

    }

    return 0;

}
