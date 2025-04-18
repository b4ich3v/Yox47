#include <iostream>
#pragma once

enum class NodeType : uint8_t
{

    IDENTIFIER = 0,
    INT_LITERAL = 1,
    FLOAT_LITERAL = 2,
    BINARY_EXPRESSION = 3,

    EXPRESSION_STATEMENT = 4,
    RETURN_STATEMENT = 5,
    IF_STATEMENT = 6,
    BLOCK_STATEMENT = 7,

    CHAR_LITERAL = 8,
    CALL_EXPRESSION = 9,
    VARIABLE_DECLARATION = 10,
    FUNCTION_DECLARATION = 11,
    PROGRAM = 12

};

struct Node 
{
public:

    NodeType type;

    explicit Node(NodeType type);

    virtual ~Node() = default;

};

