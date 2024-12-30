#include <iostream>
#include <string>
#include "Yox47.h" 

int main()
{

    while (true)
    {

        std::string input;
        std::cout << "Yox47 > ";
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
