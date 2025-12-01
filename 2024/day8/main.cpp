#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <unordered_map>
#include <regex>

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

// First answer:

static bool valid(int x, int y)
{
    return x >= 0 && x < W && y >= 0 && y < H;
}

static int key(int x, int y) 
{
    return x + (y * W);
}

struct Location 
{
    int x;
    int y;

    bool operator==(const Location& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Location& rhs) const
    {
        return !(*this == rhs);
    }

    Location operator+(const Location& rhs) const
    {
        return Location{x + rhs.x, y + rhs.y};
    }

    Location operator-(const Location& rhs) const
    {
        return Location{x - rhs.x, y - rhs.y};
    }

    int index() const
    {
        return key(x, y);
    }

    bool isValid() const 
    {
        return valid(x, y);
    }

    Location distance(const Location& other) const
    {
        return Location{other.x - x, other.y - y};
    }

};

struct Antenna 
{
    Location loc;
    char f; // Frequency
};

class Map 
{
public:
    Map(std::string input) 
    {
        std::istringstream f(input);
        std::string line;

        int x { 0 };
        int y { 0 };
        while (std::getline(f, line)) 
        {
            x = 0;
            for (char c : line) 
            {
                if (c != '.') 
                {
                    Antenna a { Location { x, y }, c };
                    antennas.emplace(key(x, y), a);
                }
                x++;
            }
            y++;
        }
    }

    const Antenna* getAntenna(const Location loc) const
    {
        auto found = antennas.find(loc.index());
        if (found != antennas.end()) 
        {
            return &(*found).second;
        }
        else 
        {
            return nullptr;
        }
    }

    const Antenna* getAntinode(const Location loc) const
    {
        auto found = antinodes.find(loc.index());
        if (found != antinodes.end()) 
        {
            return &(*found).second;
        }
        else 
        {
            return nullptr;
        }
    }

    void runWithoutHarmonics() 
    {
        for (const auto& pair : antennas) 
        {
            findAntiNodes(pair.second, true);
        }
        std::cout << "There are " << antinodes.size() << " anti nodes" << std::endl;
    }

    void runWithHarmonics() 
    {
        for (const auto& pair : antennas) 
        {
            findAntiNodes(pair.second, false);
        }
        std::cout << "There are " << antinodes.size() << " anti nodes" << std::endl;
    }

    void findAntiNodes(const Antenna& a, bool onlyOnce)
    {
        for (const auto& pair : antennas) 
        {
            const auto& other = pair.second;

            // Different location, same frequency
            if (other.loc != a.loc && other.f == a.f) 
            {
                if (!onlyOnce) 
                {
                    // Add the node overlapping with the other one
                    antinodes.emplace(other.loc.index(), other);
                }

                auto dist = a.loc.distance(other.loc);
                Location loc = a.loc - dist;

                while(loc.isValid()) 
                {
                    // Maybe this was already a node?
                    auto existing = getAntinode(loc);
                    if (existing == nullptr) 
                    {
                        auto anti = Antenna{ loc, '#' };
                        antinodes.emplace(loc.index(), anti);
                    }
                    if (onlyOnce) 
                    {
                        break;
                    }
                    else 
                    {
                        // Only search in one direction, there will be the 'reverse'
                        // match between antennas that should create the other direction..
                        loc = loc - dist;
                    }
                }

            }
            
        }
    }

    void print() const 
    {
        for (int y = 0; y < H; y++) 
        {
            for (int x = 0; x < W; x++) 
            {
                const auto antenna = getAntenna(Location{x, y});
                const auto anti = getAntinode(Location{x, y});
                if (antenna) 
                {
                    std::cout << antenna->f << " ";
                }
                else if (anti) 
                {
                    std::cout << anti->f << " ";
                }
                else 
                {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }
    }

private:
    std::unordered_map<int, Antenna> antennas;

    std::unordered_map<int, Antenna> antinodes;
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
    auto map = read_input("/home/thys/source/aoc/2024/day8/" DATA);

    map->runWithoutHarmonics();
    map->print();
}

void part2() 
{
    auto map = read_input("/home/thys/source/aoc/2024/day8/" DATA);

    map->runWithHarmonics();
    map->print();
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 6" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


