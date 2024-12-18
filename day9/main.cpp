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

struct Sloth
{
    int64_t id; // -1 for empty
    uint16_t size; // Original size of the file

    bool empty() const 
    {
        return id < 0;
    }

    void occupy(const Sloth& s) 
    {
        id = s.id;
        size = s.size;
    }

    void clear() 
    {
        id = -1;
    }

    void print() 
    {
        if (id >= 0) 
        {
            std::cout << id;// << " ";
        } 
        else 
        {
            std::cout << ".";
        }
    }

};

// 0099811188827773336446555566..............
// 0099811188827773336446555566..............

//              2,500,000,000
// First is not 875363466

class DiskMap 
{
public:
    DiskMap(std::string input) 
    {
        original = input;
        int64_t id = 0;
        for (int64_t i = 0; i < input.length(); i++) 
        {
            char c = input[i];
            uint16_t value = ASCII_TO_CHAR(c);
            if (isdigit(c)) 
            {
                if (i % 2 == 1) 
                {
                    // Even -> empty space
                    emptySloths += value;
                    for (int64_t j = 0; j < value; j++) 
                    {
                        sloths.push_back(Sloth{-1, value});
                    }
                }
                else 
                {
                    filledSloths += value;
                    for (int64_t j = 0; j < value; j++) 
                    {
                        sloths.push_back(Sloth{id, value});
                    }
                    id++;
                }
            }
        }

        std::cout << id << std::endl;
        std::cout << "Memory with " << sloths.size() << " sloths. " << filledSloths << " filled, " << emptySloths << " empty" << std::endl;
    }

    void remapDisk() 
    {
        // Loop backwards
        const auto size = sloths.size();
        for (uint64_t i = sloths.size()-1; i >= 0; i--) 
        {
            auto& sloth = sloths[i];
            if (sloth.empty()) 
            {
                consecutiveEmptyFromEnd++;
                continue;
            }

            auto firstEmptyNdx = firstEmpty();
            if (firstEmptyNdx >= i) 
            {
                std::cout << "Done!!" << std::endl;
                break;
            }
            sloths[firstEmptyNdx].occupy(sloth);
            sloth.clear();

#ifdef TEST
            print();
#endif

            // Update
            filledUpTo = firstEmptyNdx + 1;
            consecutiveEmptyFromEnd++;
        }
    }

    void remapDiskNoFragmentation() 
    {
        // Loop backwards
        const auto size = sloths.size();
        for (int64_t i = sloths.size()-1; i >= 0;) 
        {
            auto& sloth = sloths[i];
            
            if (sloth.empty()) 
            {
                i--;
                consecutiveEmptyFromEnd++;
                continue;
            }

            auto firstEmptyNdx = firstEmpty(sloth.size);
            if (firstEmptyNdx >= i) // First empty is behind current search index..
            {
                i -= sloth.size;;
                continue;
            }

            // Fill empty sloths with id, update empty size, clear moved onces
            auto emptySlothSize = sloths[firstEmptyNdx].size;
            uint16_t x;
            for (x = 0; x < sloth.size; x++) 
            {
                sloths[firstEmptyNdx + x].occupy(sloth);
#ifdef TEST
                //print();
#endif
            }
            // Go on!
            for (x; x < emptySlothSize; x++) 
            {
                sloths[firstEmptyNdx + x].size -= sloth.size;
            }
            for (x = 0; x < sloth.size; x++) 
            {
                sloths[i - x].clear();
#ifdef TEST
                //print();
#endif
            }

            i -= sloth.size;

#ifdef TEST
            print();
#endif

            // Update: find first empty slot
            for (int j = filledUpTo; j < size; j++) 
            {
                if (sloths[j].empty()) 
                {
                    filledUpTo = j;
                    break;
                }
            }
            // Update: just a little larger than i to stop looping but conditions to break still work
            consecutiveEmptyFromEnd = size - i;

            if (filledUpTo > i) 
            {
                // No need to go on..
                break;
            }
        }
    }

    uint64_t firstEmpty() const 
    {
        uint64_t loopUntil = sloths.size() - consecutiveEmptyFromEnd;
        for (uint64_t i = filledUpTo; i < loopUntil; i++) 
        {
            if (sloths[i].empty()) 
            {
                return i;
            }
        }
        return loopUntil;
    }

    uint64_t firstEmpty(uint16_t size) const 
    {
        uint64_t loopUntil = sloths.size() - consecutiveEmptyFromEnd;
        for (uint64_t i = filledUpTo; i < loopUntil; i++) 
        {
            if (sloths[i].empty() && sloths[i].size >= size) 
            {
                return i;
            }
        }
        return loopUntil;
    }

    uint64_t checksum() 
    {
        uint64_t result = 0;
        for (uint64_t i = 0; i < sloths.size(); i++) 
        {
            const auto& s = sloths[i];
            if (!s.empty()) 
            {
                result += (static_cast<uint64_t>(s.id) * i);
            }
            
        }
        return result;
    }

    void print() 
    {
        for (uint64_t i = 0; i < sloths.size(); i++) 
        {
            sloths[i].print();
        }
        std::cout << std::endl;
    }

    bool hasGaps() 
    {
        bool shouldBeEmpty = false;
        for (uint64_t i = 0; i < sloths.size(); i++) 
        {
            const auto& s = sloths[i];
            if (shouldBeEmpty && !s.empty()) 
            {
                return true;
            }
            shouldBeEmpty = s.empty();
        }
        return false;
    }

private:
    std::string original;
    std::vector<Sloth> sloths;

    uint64_t filledSloths {0};   // number of filled sloths
    uint64_t emptySloths {0};    // number of empty sloths
    uint64_t filledUpTo {0};     // lowest index from which we are sure everything is filled (so everything before this is filled up)
    uint64_t consecutiveEmptyFromEnd{0}; // minimum number of empty sloths from end
};

std::unique_ptr<DiskMap> read_input(const std::string filename) 
{
    std::ifstream instream(filename);
    std::stringstream buffer;

    if (instream.good()) 
    {
        buffer << instream.rdbuf();
        auto map = std::unique_ptr<DiskMap>(new DiskMap(buffer.str()));
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
    auto map = read_input("/home/thys/source/aoc/2024/day9/" DATA);
#ifdef TEST
    map->print();
#endif
    map->remapDisk();
    std::cout << "Checksum: " << map->checksum() << std::endl;
}

void part2() 
{
    auto map = read_input("/home/thys/source/aoc/2024/day9/" DATA);
#ifdef TEST
    map->print();
#endif
    map->remapDiskNoFragmentation();
    std::cout << "Checksum: " << map->checksum() << std::endl;
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 9" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


