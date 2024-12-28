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
#define W 8
#define H 4
#else
#define DATA "data.txt"
#define W 45
#define H W
#endif

#define ASCII_TO_CHAR(v) (v-48)
#define CHAR_TO_ASCII(c) (c+48)

static uint16_t index(uint16_t x, uint16_t y) 
{
    return (x + (y * W));
}

static bool valid(uint16_t x, uint16_t y) 
{
    return (x < W && y < H);
}

/* could have made this smarter with 90 degrees further always ++ U -> R -> ...*/
enum Direction
{
    L, R, U, D, X
};

struct Location {

    uint16_t x;
    uint16_t y;
    uint8_t h;

    bool visited = false;

    bool isValid() const 
    {
        return valid(x, y);
    }

    char print() const 
    {
        return CHAR_TO_ASCII(h);
    }

    void visit() 
    {
        visited = true;
    }

    bool isVisited() const 
    {
        return visited;
    }

    void reset() 
    {
        visited = false;
    }

};

class Map 
{
public:
    Map(std::string input) 
    {
        data = std::unique_ptr<Location[]>(new Location[W*H]);
        int read_pos = 0;
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                char v = input[read_pos];
                while(v == '\n' || v == '\r') 
                {
                    read_pos++;
                    v = input[read_pos];
                }

                auto i = index(x, y);

                // To use the test values
                if (v == '.') 
                {
                    v = '1';
                }

                data[i].x = x;
                data[i].y = y;
                data[i].h = ASCII_TO_CHAR(v);
                read_pos++;

                if (v == '0') 
                {
                    startIndexes.push_back(i);
                }
                else if (v == '9') 
                {
                    endIndexes.push_back(i);
                }
            }
        }
    }

    void run()
    {
        int sum = 0;
        for (const auto index : startIndexes) 
        {
            auto& loc = data[index];
            int score = 0;
            walkRoute(loc, score);
#ifdef TEST
            std::cout << "Score at [" << loc.x << "," << loc.y << "] is " << score << std::endl;
#endif
            reset();

            sum += score;
        }

        std::cout << "Total score: " << sum << std::endl;
    }

    void walkRoute(Location& loc, int& score)
    {
        auto neighbours = std::vector<uint16_t>();
        findNeighbours(loc, loc.h+1, neighbours);

        if (neighbours.size() > 0) 
        {
            for (auto& nbIndex : neighbours) 
            {
                auto& nb = data[nbIndex];
                if (nb.h == 9) 
                {
                    if (!nb.isVisited()) 
                    {
                        // done
                        nb.visit();
                        score++;
                    }
                }
                else 
                {
                    // keep on walking
                    walkRoute(nb, score);
                }
            }
        }
        else 
        {
            // Not arrived at a '9'
        }

    }

    void findNeighbours(const Location& loc, uint8_t height, std::vector<uint16_t>& neighbours) const
    {
        // Left
        if (loc.x > 0) 
        {
            auto i = index(loc.x-1, loc.y);
            if (data[i].h == height) 
            {
                neighbours.push_back(i);
            }
        }
        // Up
        if (loc.y > 0) 
        {
            auto i = index(loc.x, loc.y-1);
            if (data[i].h == height) 
            {
                neighbours.push_back(i);
            }
        }
        // Right
        if (loc.x < W) 
        {
            auto i = index(loc.x+1, loc.y);
            if (data[i].h == height) 
            {
                neighbours.push_back(i);
            }
        }
        // Down
        if (loc.y < H) 
        {
            auto i = index(loc.x, loc.y+1);
            if (data[i].h == height) 
            {
                neighbours.push_back(i);
            }
        }
    }

    void reset() 
    {
        for (uint16_t i : endIndexes) 
        {
            data[i].reset();
        }
    }

    void print() const 
    {
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                auto i = index(x, y);
                std::cout << data[i].print() << "";
            }
            std::cout << std::endl;
        }
    }

private:

    std::unique_ptr<Location[]> data;
    std::vector<uint16_t> startIndexes;
    std::vector<uint16_t> endIndexes;

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
    auto map = read_input("../day10/" DATA);
#ifdef TEST
    map->print();
#endif

    map->run();


}

void part2() 
{
    auto map = read_input("../day10/" DATA);
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

    part1();
    //part2();


    return 0;
}


