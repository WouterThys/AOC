#include <iostream>
#include <string>
#include <algorithm>

void first() 
{

}

void second() 
{

}

int main (int argc, char* archv[]) 
{
    std::string problem = "1";
    if (argc > 1) 
    {
        problem = std::string(archv[1]);
    }
    
    std::cout << "Let's get started on " << problem << std::endl;

    if (problem == "1") 
    {
        first();
    }
    else 
    {
        second();
    }

    return 0;
}


