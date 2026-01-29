#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"
#include "CodeGenerator.h"

int main() {
    std::ifstream file("test.txt");
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    {
        Lexer debug(source.c_str(), source.size());
        std::cout << "-----------------------------------" << std::endl;

        while (true) {
            Token currentToken = debug.nextToken();
            std::cout << Lexer::tokenName(currentToken.type) << "  " << 
                std::string(currentToken.startPtr, currentToken.length) << std::endl;

            if (currentToken.type == TokenType::END_OF_FILE) break;
        }
        std::cout << "-----------------------------------" << std::endl;
    }

    try {
        Lexer lexer(source.c_str(), source.size());   
        Parser parser(lexer);
        auto ast = parser.parseProgram();
        std::cout << "Parsing code passed!" << std::endl;

        SemanticChecker checker;
        checker.check(ast.get());
        std::cout << "Semantic checking code passed!" << std::endl;

        CodeGenerator generator(ast.get(), "out.asm");
        generator.generate();
        std::cout << "Generating code passed!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
