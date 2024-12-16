#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <bitset>
#include <cmath>

using namespace std;

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 10
#define H W
#else
#define DATA "data.txt"
#define W 130
#define H W
#endif

enum Operation 
{
    Mul,
    Add
};

struct Question 
{
    vector<uint64_t> numbers;
    uint64_t answer {0};
};

struct IAddLsb 
{
    virtual char inc(const char ch, bool& carry) = 0;
    virtual int base() = 0;
};

struct BinaryAdd : public IAddLsb
{
    char inc(const char ch, bool& carry) override 
    {
        carry = false;
        if (ch == ' ') 
        {
            return '+';
        }
        else if (ch == '+') 
        {
            return '*';
        }
        else 
        {
            carry = true;
            return '+';
        }
    }

    int base() override 
    {
        return 2;
    }
};

struct TernaryAdd : public IAddLsb
{
    char inc(const char ch, bool& carry) override 
    {
        carry = false;
        if (ch == ' ') 
        {
            return '+';
        }
        else if (ch == '+') 
        {
            return '*';
        }
        else 
        {
            carry = true;
            return '+';
        }
    }

    int base() override 
    {
        return 3;
    }
};

class Data 
{
public:
    Data() 
    {

    }

    void addLine(string line) 
    {
        std::istringstream lineStream(line);
        uint64_t key;
        char colon;

        // Read the integer key before ':'
        lineStream >> key >> colon;

        if (colon != ':') 
        {
            std::cerr << "Invalid format in line: " << line << std::endl;
            return;
        }

        // Read the integers after ':'
        Question q;
        q.answer = key;
        uint64_t value;
        while (lineStream >> value) 
        {
            q.numbers.push_back(value);
        }
        questions.push_back(q);
    }

    void printCombination(const Question& q, int combination) 
    {
        for (int i = 0; i < q.numbers.size(); i++) 
        {
            if (i == q.numbers.size()-1) 
            {
                cout << q.numbers[i];
            }
            else 
            {
                int operation = (combination & (0x1 << i));
                if (operation == 0) 
                {
                    cout << q.numbers[i] << " * ";
                }
                else 
                {
                    cout << q.numbers[i] << " + ";
                }
            }
        }
        cout << endl;
    }

    bool tryCombination(const Question& q, string combination) 
    {
        uint64_t lastResult = q.numbers[0];
        for (int i = 1, c = 0; i < q.numbers.size(); i++, c++) 
        {
            char op = combination[c];
            if (op == '*') 
            {
                lastResult = q.numbers[i] * lastResult;
            }
            else 
            {
                lastResult = q.numbers[i] + lastResult;
            }

            if (lastResult > q.answer) 
            {
                // Early break 
                return false;
            }
        }

        return lastResult == q.answer;
    }

    string createCombination(const Question& q, int combination) 
    {
        string result = "";
        for (int i = 1, c = 0; i < q.numbers.size(); i++, c++) 
        {
            int operation = (combination & (0x1 << c));
            if (operation == 0) 
            {
                result += "*";
            }
            else 
            {
                result += "+";
            }
        }

        return result;
    }

    void nextCombination(string& current, IAddLsb* adder) 
    {
        int index = 0;
        bool carry = false;
        do 
        {
            // Prepend empty if needed
            if (current.size() <= index) 
            {
                current = " " + current;
            }
            current[index] = adder->inc(current[index], carry);
            index++;
        }
        while(carry);
    }

    void run(IAddLsb* adder) 
    {
        for (const auto& q : questions) 
        {
            int numCombinations = pow(adder->base(), q.numbers.size() - 1);
            string comb = string(q.numbers.size(), '+');
            comb[0] = ' ';
            for (int c = 0; c < numCombinations; c++) 
            {
                nextCombination(comb, adder);

                if (tryCombination(q, comb)) 
                {
                    possible.push_back(q);
                    printCombination(q, c);
                    break;
                }
            }
        }

        cout << "There are " << possible.size() << " possibilities" << endl;
        uint64_t sum = 0;
        for (const auto& q : possible) 
        {
            sum += q.answer;
        }
        cout << "Sum: " << sum << endl;
    }

    size_t size() const 
    {
        return questions.size();
    }

private:
    vector<Question> questions;
    vector<Question> possible;
};

unique_ptr<Data> read_input(string filename) 
{
    ifstream infile(filename);
    if (infile.good()) 
    {
        auto ptr = unique_ptr<Data>(new Data());
        string line;
        while (getline(infile, line)) 
        {   
            ptr->addLine(line);
        }
        infile.close();
        return move(ptr);
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
        infile.close();
        return nullptr;
    }
}

// 1300503637440
// 2147483647

void part1() 
{
    auto data = read_input("/home/waldo/source/aoc/2024/day7/" DATA);
    auto adder = BinaryAdd();
    data->run(&adder);
}

void part2() 
{
    //auto data = read_input("/home/waldo/source/aoc/2024/day7/" DATA);

}

int main(int argc, char* argv[]) 
{
    cout  << "Hallo day 7" << endl;
#ifdef TEST
    cout  << "TEST TEST TEST" << endl;
#endif

    part1();
    //part2();

    return 0;
}
