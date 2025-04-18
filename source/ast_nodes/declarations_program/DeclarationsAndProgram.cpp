#include "DeclarationsAndProgram.h"

FunctionDeclaration::FunctionDeclaration(std::string name, 
	std::unique_ptr<BlockStatement> body):
	Node(NodeType::FUNCTION_DECLARATION), name(name), body(std::move(body)) {}

Program::Program(): Node(NodeType::PROGRAM) {}
