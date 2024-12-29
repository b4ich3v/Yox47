#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

const std::string DIGITS = "0123456789";

const std::string TOKEN_TYPE_INT = "INT";
const std::string TOKEN_TYPE_FLOAT = "FLOAT";
const std::string TOKEN_TYPE_DOUBLE = "DOUBLE";
const std::string TOKEN_TYPE_BOOL = "BOOL";

const std::string TOKEN_TYPE_LPAREN = "LPAREN";
const std::string TOKEN_TYPE_RPAREN = "RPAREN";

const std::string TOKEN_TYPE_MINUS = "MINUS";
const std::string TOKEN_TYPE_PLUS = "PLUS";
const std::string TOKEN_TYPE_MULT = "MULT";
const std::string TOKEN_TYPE_DIV = "DIV";

const char END_SYMBOL = '$';

class Error
{
private:

    std::string name;
    std::string details;
    int position;

public:

    Error();
    Error(const std::string& inputName, const std::string& inputDetails, int inputPosition = -1);
    std::string asString() const;

};

class IllegalCharError : public Error
{
public:

    IllegalCharError(const std::string& details, int position = -1);

};

class Token
{
private:

    std::string type;
    double value;

public:

    Token(const std::string& inputType, double inputValue);
    Token(const std::string& inputType);
    void printToken() const;
    std::string getType() const;
    double getValue() const;

};

struct LexerResult
{
public:

    std::vector<Token> tokens;
    std::unique_ptr<Error> error = nullptr;

    void printMethod() const;

};

class Lexer
{
private:

    std::string text;
    int currentPosition = -1;
    char currentSymbol = END_SYMBOL;

public:

    Lexer(const std::string& inputText);
    void advance();
    bool isDigit(char ch);
    Token makeNumber();
    LexerResult makeTokens();

};

LexerResult run(const std::string& text);
