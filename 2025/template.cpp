#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>

using namespace std;

void first(unique_ptr<int> input) 
{

}

void second(unique_ptr<int> input) 
{

}

unique_ptr<int> read_input(string filename) 
{
    ifstream infile(filename);
    stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto board = unique_ptr<Floor>(new Floor());
        if (single) 
        {
            board->initSingle(buffer.str());
        }
        else 
        {
            board->initDouble(buffer.str());
        }
        return std::move(board);
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }
    return make_unique<int>(10);
}

int main (int argc, char* archv[]) 
{
    std::string inputFile = "test.txt";
    if (argc > 1) 
    {
        inputFile = std::string(archv[1]);
    }
    
    std::cout << "Let's get started on " << inputFile << std::endl;

    auto input = read_input(inputFile);

    if (inputFile.contains("1")) 
    {
        first(std::move(input));
    }
    else 
    {
        second(std::move(input));
    }

    return 0;
}


