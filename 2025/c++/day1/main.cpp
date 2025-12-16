#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>

using namespace std;

struct Directions 
{
    char dir;
    int amount;
};

void first(vector<Directions> input) 
{
    int position = 50;
    int count = 0;

    for (const auto& d : input) 
    {
        std::cout << d.amount << " " << d.dir << std::endl;

        if (d.dir == 'R') 
        {
            position = position + d.amount;
        }
        else 
        {
            // Turing left is the same as turning right 100 - amount
            position = position + (100 - (d.amount % 100));
        }
        position = position % 100; // Wrap around

        if (position == 0) 
        {
            count ++;
        }

    }

    cout << "Part one: " << count << std::endl;
}

    static int checkZeroClicks(int startValue, Directions d)
    {
        // Every 100 in distance gives a full rotation and returns to the start value.
        int zeroCount = d.amount / 100;

        // The remainder will determine the new end position.
        int remainder = d.amount  % 100;
        
        int newValue = d.dir == 'R' ? startValue + remainder : startValue - remainder;

        if (startValue == 0)
            return zeroCount;
        else if (newValue <= 0)
            zeroCount++;
        else if (newValue >= 100)
            zeroCount++;

        return zeroCount;
    }


void second(vector<Directions> input) 
{
    int position = 50;
    int current = 0;
    int count = 0;

    for (const auto& d : input) 
    {
        
// Part one: 1055
// Part two: 6386 - 51

        count += checkZeroClicks(position, d);

        if (d.dir == 'R') 
        {
            position = position + d.amount;
        }
        else 
        {
            // Turing left is the same as turning right 100 - amount
            position = (position + (100 - (d.amount % 100)));
        }
        position = position % 100; // Wrap around

    }

    cout << "Part two: " << count << " - " << position << endl;
}

vector<Directions> read_input(string filename) 
{
    vector<Directions> input;
    ifstream infile(filename);

    if (infile.good()) 
    {
        string line;
        while (infile >> line)
        {
            Directions directions { .dir = line[0], .amount = stoi(line.substr(1)) };
            input.push_back(directions);
        }
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
    }
    return input;
};

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


