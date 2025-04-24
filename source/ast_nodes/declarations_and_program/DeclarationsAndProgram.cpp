#include "DeclarationsAndProgram.h"

FunctionDeclaration::FunctionDeclaration(std::string name, VariableType returnType,
	std::unique_ptr<BlockStatement> body) :
	Node(NodeType::FUNCTION_DECLARATION), name(std::move(name)),
	returnType(returnType), body(std::move(body)) {}

Program::Program(): Node(NodeType::PROGRAM) {}
