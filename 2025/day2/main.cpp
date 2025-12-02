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
    uint64_t start;
    uint64_t stop;
};

void first(vector<Range> input) 
{
    string str;
    string pt1;
    string pt2;
    size_t len;
    uint64_t sum = 0;
    for (const auto& r : input) 
    {
        // Only need to do numbers with an even amount of digits, but
        // let's do this the brute way first

        for (uint64_t val = r.start; val <= r.stop; val++) 
        {
            str = std::to_string(val);
            len = str.length();
            if (len % 2 == 0) 
            {
                pt1 = str.substr(0, len / 2);
                pt2 = str.substr(len / 2);

                if (pt1 == pt2) 
                {
                    sum += val;

                    //cout << " Range (" << r.start << "," << r.stop << ") invalid: " << val << endl;
                }
            }
        }
    }

    cout << "Part one: " << sum << endl;
}

static size_t stringIsInside(string part, string str) 
{
    size_t partLen = part.length();
    size_t strLen = str.length();

    if (strLen % partLen != 0) 
    {
        return 0;
    }


    size_t timesInside = 0;
    for (int i = 0; i < strLen; i += partLen) 
    {
        for (int j = 0; j < partLen; ++j) 
        {
            if (str[i + j] != part[j]) 
            {
                return 0;
            }
        }
        timesInside++;
    }
    return timesInside;
}

void second(vector<Range> input) 
{
    string str;
    string pt1;
    size_t len;
    size_t times;
    uint64_t sum = 0;
    for (const auto& r : input) 
    {
        //cout << " Range (" << r.start << "," << r.stop << ") invalid: ";

        for (uint64_t val = r.start; val <= r.stop; val++) 
        {
            str = std::to_string(val);
            len = str.length();

            // Cannot be longer than half
            for (int i = 1; i <= len / 2; ++i) 
            {
                pt1 = str.substr(0, i);
                times = stringIsInside(pt1, str);
                if (times > 0) 
                {
                    sum += val;
                    //cout << val << " ";
                    goto OUTSTOP;
                }
            }

            OUTSTOP: continue;
        }
        //cout << endl;
    }

    cout << "Part two: " << sum << endl;
}

vector<Range> read_input(string filename) 
{
    vector<Range> input;
    ifstream infile(filename);

    if (infile.good()) 
    {
        string part;
        string values;
        while (std::getline(infile, part, ','))
        {
            auto split = stringstream(part);
            Range range;

            getline(split, values, '-');
            range.start = stoul(values);
            getline(split, values);
            range.stop = stoul(values);

            input.push_back(range);
        }
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
    }
    return input;
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

    //first(input);
    second(input);

   /*
Let's get started on ../2025/day2/input.txt
Part two: 25663320831
./2025/day2 ../2025/day2/input.txt  2.73s user 0.03s system 95% cpu 2.906 total
   */

    return 0;
}


