#include <iostream>
#pragma once

enum class NodeType : uint8_t {
    IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,
    BOX_LITERAL,

    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    CALL_EXPRESSION,
    INDEX_EXPRESSION,
    ASSIGNMENT_EXPRESSION,

    EXPRESSION_STATEMENT,
    RETURN_STATEMENT,
    IF_STATEMENT,
    FOR_STATEMENT,
    WHILE_STATEMENT,
    BREAK_STATEMENT,
    BLOCK_STATEMENT,
    CHOOSE_STATEMENT,

    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,

    PROGRAM
};

struct Node {
public:
    NodeType type;
    
    explicit Node(NodeType type);
    virtual ~Node() = default;
};
