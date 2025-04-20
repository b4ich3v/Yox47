#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"

int main()
{

    std::ifstream file("test.txt");
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();         

    try 
    {

        Lexer lexer(source.c_str(), source.size());

        for (;;) 
        {        
            
            Token currentToken = lexer.nextToken();
            std::cout << (int)(currentToken.type) << "  ["
                << std::string(currentToken.startPtr, currentToken.length) << "]" << std::endl;;
            if (currentToken.type == TokenType::END_OF_FILE) break;

        }

        Parser parser(lexer);
        std::unique_ptr<Program> ast = parser.parseProgram();
        SemanticChecker checker;

        try 
        {

            checker.check(ast.get());
            std::cout << "OK!" << std::endl;

        }
        catch (const std::exception& e1)
        {

            std::cerr << "Semantic error: " << e1.what() << std::endl;
            return 1;

        }

    }
    catch (const std::exception& e2) 
    {

        std::cerr << "Parse error: " << e2.what() << std::endl;
        return 1;

    }

    return 0;

}
