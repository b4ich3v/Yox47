#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#pragma once

enum class TokenType : uint8_t 
{

    NO_TYPE = -1,

    END_OF_FILE = 0,
    IDENTIFIER = 1,
    INT = 2,
    FLOAT = 3,
   
    KEY_WORD_IF = 4, 
    KEY_WORD_ELSE = 5, 
    KEY_WORD_FUNCTION = 6, 
    KEY_WORD_RETURN = 7,

    PLUS = 8,     
    MINUS = 9,    
    STAR = 10,     
    SLASH = 11,    
    LPAREN = 12,   
    RPAREN = 13,   
    LBRACE = 14,  
    RBRACE = 15,   
    SEMICOLON = 16,
    COMMA= 17, 

    EQUAL_EQUAL = 18, 
    NEGATIVE_EQUAL = 19,  
    ASSIGN = 20

};

static std::unordered_map<std::string, TokenType> hashedKeywords =
{

    {"if", TokenType::KEY_WORD_IF},
    {"else", TokenType::KEY_WORD_ELSE},
    {"function", TokenType::KEY_WORD_FUNCTION},
    {"return", TokenType::KEY_WORD_RETURN}

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

public:

    Lexer(const char* data);

    Token nextToken();

    void tokenize();

};

