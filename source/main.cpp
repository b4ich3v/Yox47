#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/Semantic.h"
#include "code_generator/CodeGenerator.h"

int main()
{

    std::ifstream file("test.txt");
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();         

    try 
    {

        Lexer lexer(source.c_str(), source.size());
        Parser parser(lexer);
        std::unique_ptr<Program> ast = parser.parseProgram();

        SemanticChecker checker;
        checker.check(ast.get());

        CodeGenerator generator(ast.get(), "out.asm");
        generator.generate();

        std::cout << "OK!" << std::endl;

    }
    catch (const std::exception& e) 
    {

        std::cerr << "Error: " << e.what() << std::endl;
        return 1;

    }

    return 0;

}
