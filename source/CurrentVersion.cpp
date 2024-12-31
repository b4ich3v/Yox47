#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <cmath>

const char END_SYMBOL = '\0';

enum class TokenType
{

    INT,
    FLOAT,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    EOF_TOKEN,
    NONE

};

class Position
{
public:

    int index;
    int line;
    int column;
    std::string fileName;   
    std::string fileText; 

    Position()
        : index(-1), line(0), column(-1), fileName(""), fileText("")
    {}

    Position(int inputIndex, int inputLine, int inputColumn, const std::string& inputFileName, const std::string& inputFileText)
        : index(inputIndex), line(inputLine), column(inputColumn), fileName(inputFileName), fileText(inputFileText)
    {}

    void advance(char currentChar = '\0')
    {

        index += 1;
        column += 1;

        if (currentChar == '\n')
        {

            line += 1;
            column = 0;

        }

    }

    Position copy() const
    {

        return Position(index, line, column, fileName, fileText);

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

    Error(const std::string& inputName,
        const std::string& inputDetails,
        const Position& start,
        const Position& end)
        : name(inputName), details(inputDetails),
        positionStart(start), positionEnd(end)
    {}

    virtual ~Error() = default;

    virtual std::string asString() const
    {

        std::stringstream ss;

        ss << name << ": " << details << "\n"
            << "File " << positionStart.fileName
            << ", line " << (positionStart.line + 1)
            << ", column " << (positionStart.column + 1)
            << "\n\n" << stringWithArrows();

        return ss.str();

    }

    std::string stringWithArrows() const
    {

        std::stringstream ss;
        std::string text = positionStart.fileText;
        int lineNumber = positionStart.line;

        std::vector<std::string> lines;
        {

            std::stringstream textStream(text);
            std::string line;
            while (std::getline(textStream, line, '\n')) lines.push_back(line);
                
        }

        if (lineNumber < 0 || lineNumber >= (int)lines.size())
        {

            ss << "No context available.";
            return ss.str();

        }

        std::string errorLine = lines[lineNumber];
        ss << errorLine << "\n";

        std::string caretLine = "";

        for (int i = 0; i < positionStart.column && i < (int)errorLine.size(); i++)
        {

            if (errorLine[i] == '\t') caretLine += "\t";
            else caretLine += " ";

        }

        caretLine += "^";
        ss << caretLine;

        return ss.str();

    }

    std::string getName() const { return name; }

    std::string getDetails() const { return details; }

    Position getPosStart() const { return positionStart; }

    Position getPosEnd() const { return positionEnd; }

};

class IllegalCharError : public Error
{
public:

    IllegalCharError(const std::string& inputDetails,
        const Position& start,
        const Position& end)
        : Error("Illegal Character", inputDetails, start, end)
    {}

};

class InvalidSyntaxError : public Error
{
public:

    InvalidSyntaxError(const Position& start,
        const Position& end,
        const std::string& inputDetails = "")
        : Error("Invalid Syntax", inputDetails, start, end)
    {}

};

class RTError : public Error
{
public:

    std::string contextName;

    RTError(const std::string& inputName,
        const std::string& inputDetails,
        const Position& start,
        const Position& end)
        : Error(inputName, inputDetails, start, end),
        contextName(inputName)
    {}

    std::string asString() const override
    {
        
        std::stringstream ss;

        ss << name << ": " << details << "\n"
            << "File " << positionStart.fileName
            << ", line " << (positionStart.line + 1)
            << ", column " << (positionStart.column + 1)
            << "\n\n" << stringWithArrows();

        return ss.str();
    }

};

class Token
{
private:

    TokenType type;
    double value; 

public:

    Position positionStart;
    Position positionEnd;

    Token()
        : type(TokenType::NONE), value(0),
        positionStart(), positionEnd()
    {}

    Token(TokenType inputType,
        double inputValue,
        const Position& start,
        const Position& end)
        : type(inputType), value(inputValue),
        positionStart(start), positionEnd(end)
    {}

    TokenType getType() const { return type; }

    double getValue() const { return value; }

    friend std::ostream& operator << (std::ostream& os, const Token& token)
    {

        switch (token.type) 
        {
        case TokenType::INT:
            os << "INT:" << (int)token.value;
            break;
        case TokenType::FLOAT:
            os << "FLOAT:" << token.value;
            break;
        case TokenType::PLUS:
            os << "PLUS";
            break;
        case TokenType::MINUS:
            os << "MINUS";
            break;
        case TokenType::MULT:
            os << "MULT";
            break;
        case TokenType::DIV:
            os << "DIV";
            break;
        case TokenType::LPAREN:
            os << "LPAREN";
            break;
        case TokenType::RPAREN:
            os << "RPAREN";
            break;
        case TokenType::EOF_TOKEN:
            os << "EOF";
            break;
        default:
            os << "NONE";
            break;
        }

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

        for (auto& t : tokens) 
        {

            std::cout << t << " ";

        }

        std::cout << std::endl;

    }

};

class Lexer
{
private:

    std::string fileName;
    std::string text;
    Position position;
    char currentChar;

public:

    Lexer(const std::string& inputText,
        const std::string& fn = "input")
        : text(inputText),
        position(-1, 0, -1, fn, inputText),
        fileName(fn)
    {

        advance();

    }

    void advance()
    {

        position.advance(currentChar);

        if (position.index < (int)text.size()) currentChar = text[position.index];
        else  currentChar = END_SYMBOL;

    }

    bool isDigit(char c)
    {

        return (bool)std::isdigit((unsigned char)c);

    }

    Token makeNumber()
    {

        std::string numStr;
        int dotCount = 0;

        Position startPos = position.copy();

        while (currentChar != END_SYMBOL &&
            (isDigit(currentChar) || currentChar == '.'))
        {
            if (currentChar == '.') 
            {

                if (dotCount == 1) break; 
                dotCount += 1;
                numStr.push_back('.');

            }
            else numStr.push_back(currentChar);

            advance();

        }

        TokenType type = TokenType::FLOAT;
        double value = 0.0;

        try 
        {

            if (dotCount == 0) 
            {

                type = TokenType::INT;
                value = std::stoi(numStr);

            }
            else 
            {

                value = std::stod(numStr);

            }

        }
        catch (const std::invalid_argument&) 
        {

            Position endPos = position.copy();
            throw IllegalCharError("Invalid number format: " + numStr,
                startPos, endPos);

        }
        catch (const std::out_of_range&) 
        {

            Position endPos = position.copy();
            throw IllegalCharError("Number out of range: " + numStr,
                startPos, endPos);

        }

        return Token(type, value, startPos, position.copy());

    }

    LexerResult makeTokens()
    {

        LexerResult result;

        try 
        {

            while (currentChar != END_SYMBOL) 
            {

                if (currentChar == ' ' || currentChar == '\t' ||
                    currentChar == '\n' || currentChar == '\r')
                {

                    advance();

                }
                else if (isDigit(currentChar))
                {

                    Token numTok = makeNumber();
                    result.tokens.push_back(numTok);

                }
                else if (currentChar == '+') 
                {

                    Token t(TokenType::PLUS, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else if (currentChar == '-') 
                {

                    Token t(TokenType::MINUS, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else if (currentChar == '*')
                {

                    Token t(TokenType::MULT, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else if (currentChar == '/') 
                {

                    Token t(TokenType::DIV, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else if (currentChar == '(') 
                {

                    Token t(TokenType::LPAREN, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else if (currentChar == ')') 
                {

                    Token t(TokenType::RPAREN, 0.0, position.copy(), position.copy());
                    result.tokens.push_back(t);
                    advance();

                }
                else 
                {

                    char ch = currentChar;
                    Position startPos = position.copy();
                    advance();
                    Position endPos = position.copy();
                    result.error = std::make_unique<IllegalCharError>(
                        std::string(1, ch), startPos, endPos
                    );

                    return result;

                }

            }

            Token eofToken(TokenType::EOF_TOKEN, 0.0,
                position.copy(), position.copy());
            result.tokens.push_back(eofToken);

        }
        catch (const IllegalCharError& e) 
        {

            result.error = std::make_unique<IllegalCharError>(
                e.getDetails(), e.getPosStart(), e.getPosEnd()
            );

        }

        return result;

    }

};

class NumberNode;

class BinaryOperationNode;

class UnaryOperationNode;

class NodeVisitor
{
public:

    virtual ~NodeVisitor() {}

    virtual void visit(NumberNode& node) = 0;

    virtual void visit(BinaryOperationNode& node) = 0;

    virtual void visit(UnaryOperationNode& node) = 0;

};

class Node
{
public:

    virtual ~Node() = default;

    virtual void accept(NodeVisitor& visitor) = 0;

    virtual void printNode() const = 0;

};

class NumberNode : public Node
{
public:

    Token token;

    NumberNode(const Token& t) : token(t) {}

    void accept(NodeVisitor& visitor) override
    {

        visitor.visit(*this);

    }

    void printNode() const override
    {

        std::cout << token;

    }

};

class BinaryOperationNode : public Node
{
public:

    std::shared_ptr<Node> left;
    Token operation;
    std::shared_ptr<Node> right;

    BinaryOperationNode(const std::shared_ptr<Node>& inputLeft,
        const Token& tokenOperation,
        const std::shared_ptr<Node>& inputRight)
        : left(inputLeft), operation(tokenOperation), right(inputRight)
    {}

    void accept(NodeVisitor& visitor) override
    {

        visitor.visit(*this);

    }

    void printNode() const override
    {

        std::cout << "(";
        left->printNode();
        std::cout << ", " << operation << ", ";
        right->printNode();
        std::cout << ")";

    }

};

class UnaryOperationNode : public Node
{
public:

    Token operation;
    std::shared_ptr<Node> node;

    UnaryOperationNode(const Token& tokenOperation, const std::shared_ptr<Node>& inputNode)
        : operation(tokenOperation), node(inputNode)
    {}

    void accept(NodeVisitor& visitor) override
    {

        visitor.visit(*this);

    }

    void printNode() const override
    {

        std::cout << "(" << operation << ", ";
        node->printNode();
        std::cout << ")";

    }

};

class ParseResult
{
public:

    std::shared_ptr<Node> node = nullptr;
    std::unique_ptr<Error> error = nullptr;

    bool hasError() const 
    {

        return (error != nullptr);

    }

    void success(const std::shared_ptr<Node>& inputNode)
    {

        node = inputNode;

    }

    void failure(std::unique_ptr<Error> inputError)
    {

        error = std::move(inputError);

    }

};

class Parser
{
private:

    std::vector<Token> tokens;
    int tokenIndex;
    Token currentToken;

public:

    Parser(const std::vector<Token>& inputTokens)
        : tokens(inputTokens), tokenIndex(-1)
    {

        advance();

    }

    void advance()
    {

        tokenIndex += 1;

        if (tokenIndex < (int)tokens.size())
        {

            currentToken = tokens[tokenIndex];

        }

    }

    std::pair<std::shared_ptr<Node>, std::unique_ptr<Error>> parse()
    {

        ParseResult result;
        auto rootNode = expr(result);

        if (!result.hasError() && currentToken.getType() != TokenType::EOF_TOKEN)
        {

            result.failure(std::make_unique<InvalidSyntaxError>(
                currentToken.positionStart,
                currentToken.positionEnd,
                "Expected '+', '-', '*', '/', or EOF"
            ));

        }

        return { result.node, std::move(result.error) };

    }

private:

    std::shared_ptr<Node> factor(ParseResult& result)
    {

        Token current = currentToken;

        if (current.getType() == TokenType::PLUS || current.getType() == TokenType::MINUS)
        {

            advance();
            auto factorNode = factor(result);
            if (result.hasError()) return nullptr;

            auto node = std::make_shared<UnaryOperationNode>(current, factorNode);

            result.success(node);
            return node;

        }
        else if (current.getType() == TokenType::INT || current.getType() == TokenType::FLOAT)
        {

            advance();
            auto node = std::make_shared<NumberNode>(current);

            result.success(node);
            return node;

        }
        else if (current.getType() == TokenType::LPAREN)
        {

            advance();
            auto expressionNode = expr(result);
            if (result.hasError()) return nullptr;

            if (currentToken.getType() == TokenType::RPAREN) 
            {

                advance();
 
                result.success(expressionNode);
                return expressionNode;
            }
            else 
            {

                auto error = std::make_unique<InvalidSyntaxError>(
                    currentToken.positionStart,
                    currentToken.positionEnd,
                    "Expected ')'"
                );

                result.failure(std::move(error));
                return nullptr;

            }

        }

        auto error = std::make_unique<InvalidSyntaxError>(
            current.positionStart,
            current.positionEnd,
            "Expected int, float, +, - или '('"
        );
        result.failure(std::move(error));

        return nullptr;

    }

    std::shared_ptr<Node> term(ParseResult& result)
    {

        auto left = factor(result);
        if (result.hasError()) return nullptr;

        while (currentToken.getType() == TokenType::MULT ||
            currentToken.getType() == TokenType::DIV)
        {

            Token operationToken = currentToken;
            advance();
            auto right = factor(result);
            if (result.hasError()) return nullptr;

            auto binaryOperation = std::make_shared<BinaryOperationNode>(left, operationToken, right);
            left = binaryOperation; 

        }

        result.success(left);
        return left;

    }

    std::shared_ptr<Node> expr(ParseResult& result)
    {

        auto left = term(result);
        if (result.hasError()) return nullptr;

        while (currentToken.getType() == TokenType::PLUS ||
            currentToken.getType() == TokenType::MINUS)
        {

            Token operationToken = currentToken;
            advance();
            auto right = term(result);

            if (result.hasError()) return nullptr;

            auto binaryOperation = std::make_shared<BinaryOperationNode>(left, operationToken, right);
            left = binaryOperation;

        }

        result.success(left);
        return left;

    }

};

class Number
{
public:

    double value;
    Position positionStart;
    Position positionEnd;
    std::string context;

    Number() : value(0), positionStart(), positionEnd(), context("") {}

    Number(double v) : value(v), positionStart(), positionEnd(), context("") {}

    Number setPosition(const Position& start, const Position& end)
    {

        positionStart = start;
        positionEnd = end;
        return *this;

    }

    Number setContext(const std::string& ctx)
    {

        context = ctx;
        return *this;

    }

    Number addedTo(const Number& other) const
    {

        return Number(this->value + other.value);

    }

    Number subbedBy(const Number& other) const
    {

        return Number(this->value - other.value);

    }

    Number multedBy(const Number& other) const
    {

        return Number(this->value * other.value);

    }

    std::pair<Number, std::unique_ptr<Error>> dived_by(const Number& other) const
    {

        if (other.value == 0) 
        {

            return 
            {

                Number(),
                std::make_unique<RTError>(
                    "Runtime Error",
                    "Division by zero",
                    other.positionStart,
                    other.positionEnd
                )

            };

        }

        return { Number(this->value / other.value), nullptr };

    }

    std::string toString() const
    {

        if (std::floor(value) == value)
        {

            return std::to_string((int)value);

        }
        else 
        {

            return std::to_string(value);

        }

    }

};

class RTResult
{
public:

    double value = 0.0;
    std::unique_ptr<Error> error = nullptr;

    RTResult(const RTResult&) = delete;

    RTResult& operator = (const RTResult&) = delete;

    RTResult() = default;

    RTResult(RTResult&& other) noexcept
        : value(other.value), error(std::move(other.error))
    {}

    RTResult& operator = (RTResult&& other) noexcept
    {

        if (this != &other)
        {

            value = other.value;
            error = std::move(other.error);

        }

        return *this;

    }

    double registerValue(double inputValue)
    {
        
        return inputValue;

    }

    bool hasError() const 
    {

        return (error != nullptr);

    }

    RTResult& success(double inputValue)
    {

        value = inputValue;
        return *this;

    }

    RTResult& failure(std::unique_ptr<Error> inputError)
    {

        error = std::move(inputError);
        return *this;

    }

};

class Context
{
public:

    std::string displayName;
    std::shared_ptr<Context> parent;
    Position parentEntryPosition;

    Context(const std::string& inputName,
        std::shared_ptr<Context> parentContext = nullptr,
        const Position& parentPos = Position())
        : displayName(inputName), parent(parentContext),
        parentEntryPosition(parentPos)
    {}

};

class Interpreter : public NodeVisitor
{
private:

    RTResult result;
    std::shared_ptr<Context> context;

public:

    Interpreter(std::shared_ptr<Context> inputContext)
        : context(inputContext)
    {}

    RTResult interpret(const std::shared_ptr<Node>& root)
    {

        result = RTResult();
        if (root) root->accept(*this);

        return std::move(result);

    }

    void visit(NumberNode& inputNode) override
    {

        Number numberVal(inputNode.token.getValue());
        numberVal.setPosition(inputNode.token.positionStart, inputNode.token.positionEnd);
        numberVal.setContext(context->displayName);

        result.success(numberVal.value);

    }

    void visit(BinaryOperationNode& inputNode) override
    {

        Interpreter leftInterpreter(context);
        RTResult leftResult = leftInterpreter.interpret(inputNode.left);

        if (leftResult.hasError()) 
        {

            result = std::move(leftResult); 
            return;

        }

        double leftValue = result.registerValue(leftResult.value);

        Interpreter rightInterpreter(context);
        RTResult rightResult = rightInterpreter.interpret(inputNode.right);

        if (rightResult.hasError()) 
        {

            result = std::move(rightResult);
            return;

        }

        double rightValue = result.registerValue(rightResult.value);
        double outValue = 0.0;
        TokenType operationType = inputNode.operation.getType();

        if (operationType == TokenType::PLUS) outValue = leftValue + rightValue;
        else if (operationType == TokenType::MINUS) outValue = leftValue - rightValue;
        else if (operationType == TokenType::MULT) outValue = leftValue * rightValue;
        else if (operationType == TokenType::DIV) 
        {
            
            auto divResult = Number(leftValue).dived_by(Number(rightValue));

            if (divResult.second) 
            {

                result.failure(std::move(divResult.second));
                return;

            }

            outValue = divResult.first.value;

        }
        else
        {

            result.failure(std::make_unique<RTError>(
                "Runtime Error",
                "Unknown binary operator",
                inputNode.operation.positionStart,
                inputNode.operation.positionEnd
            ));

            return;

        }

        result.success(outValue);

    }

    void visit(UnaryOperationNode& inputNode) override
    {
      
        Interpreter subInterpreter(context);
        RTResult subResult = subInterpreter.interpret(inputNode.node);

        if (subResult.hasError()) 
        {

            result = std::move(subResult);
            return;

        }

        double value = subResult.value;

        if (inputNode.operation.getType() == TokenType::MINUS) result.success(-value);
        else if (inputNode.operation.getType() == TokenType::PLUS) result.success(+value);
        else 
        {

            result.failure(std::make_unique<RTError>(
                "Runtime Error",
                "Unknown unary operator",
                inputNode.operation.positionStart,
                inputNode.operation.positionEnd
            ));

        }

    }

};

std::pair<double, std::unique_ptr<Error>> run(const std::string& text,
    const std::string& fileName = "input")
{

    Lexer lexer(text, fileName);
    LexerResult lexResult = lexer.makeTokens();

    if (lexResult.error)
    {

        return { 0.0, std::move(lexResult.error) };

    }

    Parser parser(lexResult.tokens);
    auto parseOutput = parser.parse(); 

    if (parseOutput.second) 
    {

        return { 0.0, std::move(parseOutput.second) };

    }

    std::shared_ptr<Node> root = parseOutput.first;

    std::shared_ptr<Context> context = std::make_shared<Context>("<program>");
    Interpreter interpreter(context);
    RTResult result = interpreter.interpret(root);

    if (result.hasError()) return { 0.0, std::move(result.error) };
    else return { result.value, nullptr };

}

int main()
{

    while (true) 
    {

        std::string input;
        std::cout << "Yox47 > ";

        if (!std::getline(std::cin, input)) 
        {

            std::cout << "\nExiting REPL.\n";
            break;

        }

        if (input.empty()) continue;

        auto runResult = run(input, "<stdin>");
        double value = runResult.first;
        std::unique_ptr<Error> error = std::move(runResult.second);

        if (error) std::cout << error->asString() << "\n";
        else std::cout << value << "\n";

    }

    return 0;

}
