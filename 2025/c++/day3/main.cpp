#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>

using namespace std;

struct BatteryLine
{
    vector<uint8_t> joltages;
};

// static uint61_t someFunc(const uint8_t totalDigits, const vector<uint8_t> list, size_t currentPos )
// {
//     run from left to right over the list

//     should be recursive

//     pass number of digits

//     check if still enough numbers to create

//     return current * 10;
// }

static void findHighestDigit(const vector<uint8_t> list, size_t startPos, size_t dontLookAtLast, uint8_t* highest, size_t* pos)
{
    *highest = 0;
    *pos = 0;

    uint8_t endPos = list.size() - dontLookAtLast;
    for (int j = startPos; j < endPos; ++j)
    {
        if (list[j] > *highest) 
        {
            *highest = list[j];
            *pos = j;
        }
    }
}

void first(vector<BatteryLine> input)
{
    uint32_t sum = 0;

    // This works: 17092
    // for (const auto &bl : input)
    // {
    //     uint8_t highestFirstDigit = 0;
    //     uint8_t highest = 0;
    //     for (int i = 0; i < bl.joltages.size() - 1; ++i)
    //     {
    //         if (bl.joltages[i] <= highestFirstDigit)
    //         {
    //             // If first digit is already smaller than the highest number first
    //             // digit we don't need to go on
    //             continue;
    //         }
    //         for (int j = i + 1; j < bl.joltages.size(); ++j)
    //         {
    //             uint8_t number = (bl.joltages[i] * 10) + bl.joltages[j];
    //             if (number > highest)
    //             {
    //                 highest = number;
    //                 highestFirstDigit = bl.joltages[i];
    //             }
    //         }
    //     }
    //     sum += highest;
    // }

    for (const auto &bl : input)
    {
        uint8_t highest = 0;
        uint8_t digit = 0;
        size_t pos = 0;
        size_t digitsLeft = 2;

        while (digitsLeft > 0) 
        {
            findHighestDigit(bl.joltages, pos, digitsLeft-1, &digit, &pos);
            highest = (highest * 10) + digit;
            pos++;
            digitsLeft--;
        }

        sum += highest;

    }

    cout << "Part one: " << sum << endl;
}

void second(vector<BatteryLine> input)
{
    uint64_t sum = 0;

    for (const auto &bl : input)
    {
        uint64_t highest = 0;
        uint8_t digit = 0;
        size_t pos = 0;
        size_t digitsLeft = 12;

        //cout << "Highest: ";

        while (digitsLeft > 0) 
        {
            findHighestDigit(bl.joltages, pos, digitsLeft-1, &digit, &pos);
            highest = (highest * 10) + digit;
            //cout << to_string(digit);
            pos++;
            digitsLeft--;
        }
        //cout << endl;
        sum += highest;

    }

    cout << "Part two: " << sum << endl;
}

vector<BatteryLine> read_input(string filename)
{
    vector<BatteryLine> input;
    ifstream infile(filename);

    if (infile.good())
    {
        string inputLine;
        while (getline(infile, inputLine))
        {
            BatteryLine batteryLine;
            for (int i = 0; i < inputLine.length(); ++i)
            {
                batteryLine.joltages.push_back(inputLine[i] - '0');
            }
            input.push_back(batteryLine);
        }
    }
    else
    {
        cout << "Bad input file " << filename << endl;
    }
    return input;
}

int main(int argc, char *archv[])
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
