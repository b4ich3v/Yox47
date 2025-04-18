#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstring>
#include <unordered_map>
#pragma once

enum class TokenType : uint8_t 
{

    NO_TYPE = 0,

    END_OF_FILE = 1,
    IDENTIFIER = 2,
    INT = 3,
    FLOAT = 4,
   
    KEY_WORD_IF = 5, 
    KEY_WORD_ELSE = 6, 
    KEY_WORD_FUNCTION = 7, 
    KEY_WORD_RETURN = 8,
    KEY_WORD_INT = 9,
    KEY_WORD_FLOAT = 10,
    KEY_WORD_CHAR = 11,

    COLON = 12,
    CHAR_LITERAL = 13,

    PLUS = 14,
    MINUS = 15,
    STAR = 16,
    SLASH = 17,
    LPAREN = 18,
    RPAREN = 19,
    LBRACE = 20,
    RBRACE = 21,
    SEMICOLON = 22,
    COMMA= 23,

    EQUAL_EQUAL = 24,
    NEGATIVE_EQUAL = 25,
    ASSIGN = 26

};

static std::unordered_map<std::string, TokenType> hashedKeywords =
{

    {"if", TokenType::KEY_WORD_IF},
    {"else", TokenType::KEY_WORD_ELSE},
    {"function", TokenType::KEY_WORD_FUNCTION},
    {"return", TokenType::KEY_WORD_RETURN},
    {"int",   TokenType::KEY_WORD_INT},
    {"float", TokenType::KEY_WORD_FLOAT},
    {"char",  TokenType::KEY_WORD_CHAR},

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

