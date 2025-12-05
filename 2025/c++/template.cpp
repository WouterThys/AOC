#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>

using namespace std;

struct Range 
{
    int start = 0;
    int stop = 0;
};

class Ingredients 
{
public:

    vector<Range>& getRanges() 
    {
        return ranges;
    }

    vector<int>& getIds() 
    {
        return ids;
    }

private:
    vector<Range> ranges;
    vector<int> ids;
};

void first(shared_ptr<Ingredients> input) 
{

}

void second(shared_ptr<Ingredients> input) 
{

}

shared_ptr<Ingredients> read_input(string filename) 
{
    ifstream infile(filename);

    if (infile.good()) 
    {
        auto ingredients = make_shared<Ingredients>();
       
        return ingredients;
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }

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
    first(input);
    second(input);

    return 0;
}


