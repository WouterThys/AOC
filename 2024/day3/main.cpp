#include <iostream>
#include <algorithm>
#include <set>

#include <fstream>
#include <sstream>
#include <string>

#include <regex>

using namespace std;

string read_input(string filename) 
{
    ifstream instream(filename);
    stringstream buffer;

    if (instream.good()) 
    {
        buffer << instream.rdbuf();
    }

    return buffer.str();
}

int main (int argc, char* argv[]) 
{
    cout << "hallo day 3" << endl;

    //string input = read_input("/home/thys/source/aoc/2024/day3/test.txt");
    string input = read_input("/home/thys/source/aoc/2024/day3/data.txt");

    // Regular expression pattern to match 'mul(a,b)' where a and b are 1-3 digit integers
    regex p1(R"(mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don't\(\))");

    // Create a regex iterator to find matches
    auto p1_begin = sregex_iterator(input.begin(), input.end(), p1);
    auto p1_end = sregex_iterator();

    int sum = 0;

    bool enabled = true;

    cout << "Matches found!" << endl;
    for (sregex_iterator i = p1_begin; i != p1_end; ++i) {
        smatch match = *i;
        string p1_match_str = match.str();
        
        if (p1_match_str == "do()") 
        {
            enabled = true;
            continue;
        }

        if (p1_match_str == "don't()") 
        {
            enabled = false;
            continue;
        }

        if (enabled) 
        {
            // Regular expression to extract the two numbers
            std::regex p2(R"((\d{1,3}),(\d{1,3}))");

            // Create a regex iterator to find matches
            auto p2_begin = std::sregex_iterator(p1_match_str.begin(), p1_match_str.end(), p2);
            auto p2_end = std::sregex_iterator();

            for (std::sregex_iterator i = p2_begin; i != p2_end; ++i) {
                //std::cout << (*i)[1] << " * " << (*i)[2] << "\n";

                sum += stoi ( (*i)[1].str() ) * stoi ((*i)[2].str() );
            }
        }

    }

    cout << "Sum: " << sum << endl;

    return 0;
}


