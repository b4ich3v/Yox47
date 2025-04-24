#include "Lexer.h"
#include "DeclarationsAndProgram.h"
#include "Statements.h"
#include "Expressions.h"
#include "CallExpressionAndVariables.h"
#pragma once

class Parser
{
private:

    Lexer& lexer;
    Token currentToken;

    void process();

    bool checkToken(TokenType type) const;

    bool matcher(TokenType type);

    void expectManagement(TokenType type, const char* message);

    static int priorityManagement(TokenType type);

    VariableType parseType();

    std::unique_ptr<FunctionDeclaration> parseFunction();

    std::unique_ptr<BlockStatement> parseBlock();

    std::unique_ptr<Statement> parseStatement();

    std::unique_ptr<Statement> parseIf();

    std::unique_ptr<Statement> parseReturn();

    std::unique_ptr<Expression> parseExpression(int minPrecedence = 0);

    std::unique_ptr<Expression> parsePrimary();

    std::unique_ptr<Statement> parseVariableDeclaration();

    std::unique_ptr<VariableDeclaration> parseVariableDeclarationNoSemi();

    std::unique_ptr<Statement> parseWhile();

    std::unique_ptr<Statement> parseChoose();

    std::unique_ptr<Statement> parseFor();

    std::unique_ptr<Expression> finishPostfix(std::unique_ptr<Expression> expression);

public:

    explicit Parser(Lexer& lexer);

    std::unique_ptr<Program> parseProgram();

};
