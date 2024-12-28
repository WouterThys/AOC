#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <unordered_map>
#include <regex>
#include <cctype>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 12
#define H W
#else
#define DATA "data.txt"
#define W 50
#define H W
#endif

#define ASCII_TO_CHAR(v) (v-48)
#define CHAR_TO_ASCII(c) (c+48)


class Stoner 
{
public:
    Stoner(std::string input) 
    {
        std::istringstream lineStream(input);

        uint64_t value;
        while (lineStream >> value) 
        {
            stones.push_back(value);
        }
    }

    void iterate(int times) 
    {
        std::vector<uint64_t> temp;
        for (int i = 0; i < times; i++) 
        {
            temp.clear();
            runOnce(stones, temp);
            stones = temp;
#ifdef TEST
        print(stones);
#endif
        }

        std::cout << "After " << times << " times, there are " << stones.size() << " stones" << std::endl;
    }

    void runOnce(std::vector<uint64_t>& input, std::vector<uint64_t>& output) 
    {
        int64_t out1, out2 = 0;
        for(const auto val : input) 
        {
            apply(val, out1, out2);
            output.push_back(out1);
            if (out2 >= 0) 
            {
                output.push_back(out2);
            }
        }
    }

    void apply(const uint64_t val, int64_t& out1, int64_t& out2) const
    {
        out1 = -1;
        out2 = -1;

        // Helper
        std::string strVal = std::to_string(val);

        // Rule 1: If the stone is engraved with the number 0, it is replaced by a stone engraved with the number 1
        if (val == 0) 
        {
            out1 = 1;
        }
        // If the stone is engraved with a number that has an even number of digits, it is replaced by two stones
        else if (strVal.length() %2 == 0)
        {
            out1 = std::stoull( strVal.substr(0, strVal.length() / 2) );
            out2 = std::stoull( strVal.substr(strVal.length() / 2, strVal.length()) );
        }
        // The old stone's number multiplied by 2024 is engraved on the new stone
        else 
        {
            out1 = val * 2024;
        }
    }

    void print() const 
    {
        print(stones);
    }

    void print(const std::vector<uint64_t>& v) const 
    {
        for (const auto val : v) 
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

private:
    std::vector<uint64_t> stones;
};

std::unique_ptr<Stoner> read_input(const std::string filename) 
{
    std::ifstream instream(filename);
    std::stringstream buffer;

    if (instream.good()) 
    {
        buffer << instream.rdbuf();
        auto map = std::unique_ptr<Stoner>(new Stoner(buffer.str()));
        return std::move(map);
    }
    else 
    {
        std::cout << "Invalid input " << filename << std::endl;
         return nullptr;
    }
}

void part1() 
{
    auto map = read_input("../day11/" DATA);
    map->iterate(25);

#ifdef TEST
    map->print();
#endif
}

void part2() 
{
//     auto map = read_input("../day11/" DATA);
// #ifdef TEST
//     map->print();
// #endif
//     map->remapDiskNoFragmentation();
//     std::cout << "Checksum: " << map->checksum() << std::endl;
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 11" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();


    return 0;
}


