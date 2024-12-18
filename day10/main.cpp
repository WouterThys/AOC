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

class Map 
{
public:
    Map(std::string input) 
    {

    }

    void print() const 
    {

    }

private:
};

std::unique_ptr<Map> read_input(const std::string filename) 
{
    std::ifstream instream(filename);
    std::stringstream buffer;

    if (instream.good()) 
    {
        buffer << instream.rdbuf();
        auto map = std::unique_ptr<Map>(new Map(buffer.str()));
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
    auto map = read_input("/home/thys/source/aoc/2024/day10/" DATA);
#ifdef TEST
    map->print();
#endif

}

void part2() 
{
    auto map = read_input("/home/thys/source/aoc/2024/day10/" DATA);
#ifdef TEST
    map->print();
#endif

}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 10" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


