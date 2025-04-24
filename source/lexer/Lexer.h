#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstring>
#include <unordered_map>
#pragma once

enum class TokenType : uint8_t
{

    NO_TYPE,

    INT,
    FLOAT,
    IDENTIFIER,
    CHAR_LITERAL,
    BOOL_LITERAL,

    KEY_WORD_IF,
    KEY_WORD_ELSE,
    KEY_WORD_CHOOSE,
    KEY_WORD_CASE,
    KEY_WORD_DEFAULT,
    KEY_WORD_FUNCTION,
    KEY_WORD_RETURN,
    KEY_WORD_INT,
    KEY_WORD_FLOAT,
    KEY_WORD_BOOL,
    KEY_WORD_BOX,
    KEY_WORD_CHAR,
    KEY_WORD_FOR,
    KEY_WORD_WHILE,
    KEY_WORD_BREAK,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL_EQUAL,
    NOT_EQUAL,
    ASSIGN,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    NEGATION,

    SEMICOLON,
    COLON,
    COMMA,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,

    END_OF_FILE

};

static std::unordered_map<std::string, TokenType> hashedKeywords =
{

    {"if", TokenType::KEY_WORD_IF},
    {"else", TokenType::KEY_WORD_ELSE},
    {"function", TokenType::KEY_WORD_FUNCTION},
    {"return", TokenType::KEY_WORD_RETURN},
    {"int", TokenType::KEY_WORD_INT},
    {"float", TokenType::KEY_WORD_FLOAT},
    {"char", TokenType::KEY_WORD_CHAR},
    {"for", TokenType::KEY_WORD_FOR},
    {"while", TokenType::KEY_WORD_WHILE},
    {"break", TokenType::KEY_WORD_BREAK},
    {"bool", TokenType::KEY_WORD_BOOL},
    {"true", TokenType::BOOL_LITERAL},
    {"false", TokenType::BOOL_LITERAL},
    {"box", TokenType::KEY_WORD_BOX},
    {"choose", TokenType::KEY_WORD_CHOOSE},
    {"case", TokenType::KEY_WORD_CASE},
    {"default", TokenType::KEY_WORD_DEFAULT}

};

struct Token
{
public:

    TokenType type = TokenType::NO_TYPE;
    const char* startPtr = nullptr;
    uint32_t length = 0;
    uint32_t currentLine = 0;
    uint32_t currentColumn = 0;

    Token() = default;

    Token(TokenType type, const char* start, uint32_t length, uint32_t currentLine, uint32_t currentColumn);

};

class Lexer
{
private:

    const char* beginPtr = nullptr;
    const char* currentPtr = nullptr;
    const char* endPtr = nullptr;

    std::vector<Token> tokens;
    uint32_t currentLine = 1;
    uint32_t currentColumn = 1;

    char peek();

    char peekNext();

    char process();

    void trim();

    bool match(char expectedSymbol);

    static bool isAlpha(char ch);

    static bool isAlnum(char ch);

    static bool isDigit(char ch);

    static TokenType keywordMatcher(const std::string& variableId);

    Token generateToken(TokenType type, const char* ptr);

    Token identifier(const char* ptr);

    Token number(const char* ptr);

    Token character(const char* ptr);

public:

    Lexer(const char* data, size_t len);

    Token nextToken();

    void tokenize();

};

