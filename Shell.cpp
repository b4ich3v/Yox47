#include <iostream>
#include <string>
#include "Basic.h" 

int main()
{

    while (true)
    {

        std::string input;
        std::cout << "basic > ";
        std::getline(std::cin, input);

        LexerResult result = run(input);
        
        if (result.error != nullptr)
        {

            std::cout << result.error->asString();

        }
        else
        {

            result.printMethod();

        }

        std::cout << std::endl;

    }

    return 0;

}
