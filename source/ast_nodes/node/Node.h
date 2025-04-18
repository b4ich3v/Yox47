#include <iostream>
#pragma once

enum class NodeType : uint8_t
{

    IDENTIFIER = 0,
    INT_LITERAL = 1,
    FLOAT_LITERAL = 2,
    BINARY_EXPRESSION = 3,

    EXPRESSION_STATMENT = 4,
    RETURN_STATEMENT = 5,
    IF_STATEMENT = 6,
    BLOCK_STATMENT = 7,

    FUNCTION_DECLARATION = 8,
    PROGRAM = 9

};

struct Node 
{
public:

    NodeType type;

    explicit Node(NodeType type);

    virtual ~Node() = default;

};

