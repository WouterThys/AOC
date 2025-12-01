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

#define TEST

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
            stones[value] += 1;
        }
    }

    void iterate(int times) 
    {
        for (int i = 0; i < times; i++) 
        {
            blink(stones);
        }

        uint64_t sum = 0;
        for (const auto& pair : stones) 
        {
            sum += pair.second;
        }

        std::cout << "Result: " << sum << std::endl;
    }

    void blink(std::unordered_map<uint64_t, uint64_t>& stones) 
    {
        static std::unordered_map<uint64_t, uint64_t> nextStones;

        nextStones.clear();
        for(const auto& pair : stones) {
            const uint64_t& stone = pair.first;
            
            // '0' -> increment count for '1'
            if(stone == 0) 
            {
                nextStones[1] += pair.second;
                continue;
            }

            auto res = apply(stone);

            for(const uint64_t& val : res) 
            {
                nextStones[val] += pair.second;
            }
        }

        stones = nextStones;
    }

    std::vector<uint64_t> apply(const uint64_t val) const
    {
        std::vector<uint64_t> result;
        // Helper
        std::string strVal = std::to_string(val);
        // If the stone is engraved with a number that has an even number of digits, it is replaced by two stones
        if (strVal.length() %2 == 0)
        {
            result.push_back ( std::stoull( strVal.substr(0, strVal.length() / 2) ) );
            result.push_back ( std::stoull( strVal.substr(strVal.length() / 2, strVal.length()) ));
        }
        // The old stone's number multiplied by 2024 is engraved on the new stone
        else 
        {
            result.push_back ( val * 2024 );
        }
        return result;
    }

private:
    std::unordered_map<uint64_t, uint64_t> stones;
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
    map->iterate(75);

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


