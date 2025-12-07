#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <unordered_set>
#include <vector>
#include <string>

#include "../utils.hpp"

using namespace std;

enum class Operator 
{
    Add = 0,
    Mul
};

class Problem
{
public:
    vector<uint64_t> &getNumbers() 
    {
        return numbers;
    }

    void setOperator(Operator op) 
    {
        this->op = op;
    }

    uint64_t execute() const
    {
        uint64_t result = op == Operator::Add ? 0 : 1;
        for (const auto n : numbers) 
        {
            if (op == Operator::Add) 
            {
                result += n;
            }
            else 
            {
                result *= n;
            }
        }
        return result;
    }


private:
    vector<uint64_t> numbers;
    Operator op;
};

void first(vector<Problem> input)
{
    uint64_t sum = 0;

    for (const auto problem : input)
    {
        sum += problem.execute();
    }

    cout << "Day one: " << sum << endl;
}

void second(vector<Problem> input)
{
    uint64_t sum = 0;
    

    cout << "Day two: " << sum << endl;
}

vector<Problem> read_input_one(string filename)
{
    ifstream infile(filename);
    vector<Problem> result;
    if (infile.good())
    {
        string line;
        string parts;
        bool first = true;
        
        while (getline(infile, line))
        {
            trim(line);
            auto is = istringstream(line);

            if (first) 
            {
                while(getline(is, parts, ' ')) 
                {
                    if (parts.empty()) continue;

                    Problem p;
                    p.getNumbers().push_back(stoul(parts));
                    result.push_back(p);
                }

                first = false;
            }
            else if (line[0] != '*' && line[0] != '+') 
            {
                int i = 0;

                // Numbers
                while(getline(is, parts, ' '))  
                {
                    if (parts.empty()) continue;
                    result[i].getNumbers().push_back(stoul(parts));
                    i++;
                }
            }
            else 
            {
                int i = 0;

                // Operator
                while(getline(is, parts, ' ')) 
                {
                    if (parts.empty()) continue;
                    result[i].setOperator(parts[0] == '*' ? Operator::Mul : Operator::Add);
                    i++;
                }
            }

        }

        return result;
    }
    else
    {
        cout << "Bad input file " << filename << endl;
        return result;
    }
}

vector<Problem> read_input_two(string filename)
{
    ifstream infile(filename);
    vector<Problem> result;
    if (infile.good())
    {
        string line;
        vector<string> read;
        
        while (getline(infile, line))
        {
            read.push_back(line);
        }

        // Add the first problem
        auto currentProblem = Problem();

        // numbers
        for (int i = 0; i < line.length(); ++i) 
        {
            uint64_t number = 0;
            for (int j = 0; j < read.size() -1; ++j) 
            {
                char digit = read[j][i];
                if (digit != ' ') 
                {
                    // Append digits
                    number = (number * 10) + (digit - '0');
                }
            }

            // It were only spaces -> start a new problem
            if (number == 0) 
            {
                result.push_back(currentProblem);
                currentProblem = Problem();
            }
            // Ok, add number to the current problem
            else 
            {
                currentProblem.getNumbers().push_back(number);
            }
        }

        // Add the last one
        result.push_back(currentProblem);

        // operators
        int i = 0;
        string parts;
        auto is = istringstream(read.at(read.size() - 1));
        while(getline(is, parts, ' ')) 
        {
            if (parts.empty()) continue;
            result[i].setOperator(parts[0] == '*' ? Operator::Mul : Operator::Add);
            i++;
        }

        return result;
    }
    else
    {
        cout << "Bad input file " << filename << endl;
        return result;
    }
}

int main(int argc, char *archv[])
{
    std::string inputFile = "test.txt";
    if (argc > 1)
    {
        inputFile = std::string(archv[1]);
    }

    std::cout << "Let's get started on " << inputFile << std::endl;

    first(read_input_one(inputFile));
    first(read_input_two(inputFile));

    return 0;
}
