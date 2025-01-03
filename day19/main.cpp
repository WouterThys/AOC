#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W (6+1)
#define H W
#define N 12
#else
#define DATA "data.txt"
#define W (70+1)
#define H W
#define N 1024
#endif

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

struct Towel 
{
    std::string pattern;
    size_t size;
};

class Towels 
{
public:

    Towels() 
    {

    }

    void init(std::string input) 
    {
        std::istringstream istr(input);
        std::string line;

        // First part with towel defs
        std::getline(istr, line);

        std::istringstream tstr(line);
        std::string defined;
        while (std::getline(tstr, defined, ',')) 
        {
            trim(defined);
            Towel t { defined, defined.length() };
            existing.push_back(t);
        }

        // Desired patterns
        while (std::getline(istr, line)) 
        {   
            if (!line.empty()) 
            {
                Towel t { line, line.length() };
                desired.push_back(t);
            }
        }

        // Sort
        std::sort(existing.begin(), existing.end(), [](const Towel& a, const Towel& b) 
        {
            return a.size > b.size;
        });

    }

    void solve() 
    {
        size_t total = 0;
        for (auto& pattern : desired) 
        {
            int count = 0;
            search(pattern, 0, count);
            std::cout << pattern.pattern << " has " << count << " possible" << std::endl;
            total += count;
        }

        std::cout << "There are " << total << " possible" << std::endl;
    }

    void search(Towel& s, size_t ptr, int& count) 
    {
        if (ptr >= s.size) 
        {
            count++;
            return;
        }
        for (uint16_t i = 0; i < existing.size(); i++) 
        {
            const auto& towel = existing[i];
            if (match(s.pattern, towel.pattern, ptr)) 
            {
                search(s, ptr + towel.size, count);
            }
        }
    }

    bool match(const std::string& input, const std::string& with, size_t start) 
    {
        size_t match = 0;
        for (size_t i = 0; i < input.length()-start && i < with.length(); i++) 
        {
            if (input[start + i] != with[i]) 
            {
                return false;
            }
            else 
            {
                match++;
            }
        }
        // Full match (and not less...)
        return match == with.size();
    }

    void print() const 
    {
        std::cout << "We got " << existing.size() << " existing towels and " << desired.size() << " towels" << std::endl; 
    }

private:

    std::vector<Towel> existing;
    std::vector<Towel> desired;

};

std::unique_ptr<Towels> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Towels>(new Towels());
        ptr->init(buffer.str());
        return std::move(ptr);
    }   
    else 
    {
        std::cout << "Bad input file " << filename << std::endl;
        return nullptr;
    }
}


void part1() 
{
    auto ptr = read_input("../day19/" DATA);
    ptr->print();
    ptr->solve();
}

void part2() 
{
    auto ptr = read_input("../day19/" DATA);

}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 19" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
