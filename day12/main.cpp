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
#define W 5
#define H W
#else
#define DATA "data.txt"
#define W 140
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

    const uint16_t x;
    const uint16_t y;
    const char p; // Plant

    Location(uint16_t _x, uint16_t _y, char _p) 
        : x {_x}
        , y {_y}
        , p {_p}
    {

    } 

    bool visited = false;
    int perm = 0;

    bool isValid() const 
    {
        return valid(x, y);
    }

    char print() const 
    {
        return p;
    }

    void visit() 
    {
        visited = true;
    }

    bool isVisited() const 
    {
        return visited;
    }

    void setPerm(std::vector<Location*> nbs, std::vector<Location>& data, const bool discount) 
    {
        if (!discount) 
        {
            perm = 4 - nbs.size();
        }
        else 
        {
            const auto s = nbs.size();
            if (s == 0) 
            {
                perm = 4;
            }
            else if (s == 1) 
            {
                perm = 2; // 1 neigbour -> 2 corners
            }
            else if (s == 2) 
            {
                if ((nbs[0]->x == nbs[1]->x) || (nbs[0]->y == nbs[1]->y)) 
                {
                    perm = 0; // aAa of vertical -> 0
                }
                else // L U R D
                {
                    const auto& nb = nbs[0]; // First nb is L or U
                    // a A  A a -> same y
                    // x a  a x -> diff x
                    if (nb->y == y) 
                    {
                        int check = index(nb->x, nbs[1]->y);
                        if (data[check].p == p) 
                        {
                            perm = 1;
                        }
                        else 
                        {
                            perm = 2;
                        }
                    }
                    // a x   A a
                    // A a   a x 
                    else 
                    {
                        int check = index(nbs[1]->x, nb->y);
                        if (data[check].p == p) 
                        {
                            perm = 1;
                        }
                        else 
                        {
                            perm = 2;
                        }
                    }
                }
            }
            else if (s == 3)
            {
                // x a x    a x            x a
                // a A a    A a   a A a    a A
                //          a x   x a x    x a
                perm = 2;

                // x a x
                // a A a
                if (nbs[0]->y == y && nbs[1]->x == x && nbs[2]->y == y) 
                {
                    int check = index(x-1, y-1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                    check = index(x+1, y-1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                }
                // a x
                // A a
                // a x
                else if (nbs[0]->x == x && nbs[1]->y == y && nbs[2]->x == x) 
                {
                    int check = index(x+1, y-1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                    check = index(x+1, y+1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                }
                // a A a
                // x a x
                else if (nbs[0]->y == y && nbs[1]->y == y && nbs[2]->x == x) 
                {
                    int check = index(x-1, y+1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                    check = index(x+1, y+1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                }
                // x a
                // a A
                // x a
                else if (nbs[0]->y == y && nbs[1]->x == x && nbs[2]->x == x) 
                {
                    int check = index(x-1, y-1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                    check = index(x-1, y+1);
                    if (data[check].p == p) 
                    {
                        perm--;
                    }
                }
                
            }
            else 
            {
                // x a x
                // a A a
                // x a x
                perm = 4;
                if (data[index(x-1, y-1)].p == p) 
                {
                    perm--;
                }
                if (data[index(x-1, y+1)].p == p) 
                {
                    perm--;
                }
                if (data[index(x+1, y-1)].p == p) 
                {
                    perm--;
                }
                if (data[index(x+1, y+1)].p == p) 
                {
                    perm--;
                }
            }
        }
    }

    int getPerm() 
    {
        return perm;
    }

    void reset() 
    {
        visited = false;
        perm = 0;
    }

};

class Map 
{
public:
    Map(std::string input) 
    {
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

                data.push_back(Location{x, y, v});
                read_pos++;
            }
        }
    }

    void run(bool discount)
    {
        int area = 0;
        int perm = 0;
        int price = 0;
        int total = 0;

        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                auto i = index(x, y);
                auto& loc = data[i];
                if (!loc.isVisited()) 
                {
                    area = 0;
                    perm = 0;
                    price = 0;

                    recurse(loc, area, perm, discount);
                    price = area * perm;
                    total += price;
#ifdef TEST
                    std::cout << "Plant " << loc.p << ": area=" << area << ", perm=" << perm << " -> " << price << std::endl;
#endif
                }
            }
        }

        std::cout << "Total: " << total << std::endl;
    }

    void recurse(Location& loc, int& area, int& permiter, bool discount) 
    {
        loc.visit();
        std::vector<Location*> nbs;
        findNeighbours(loc, nbs);
        loc.setPerm(nbs, data, discount);
        
        area++;
        auto temp = loc.getPerm();
        permiter += temp;

        for(auto& nb : nbs) 
        {
            if (!nb->isVisited()) 
            {
                recurse(*nb, area, permiter, discount);
            }
        }
    } 

    void findNeighbours(const Location& loc, std::vector<Location*>& neighbours)
    {
        // Left
        if (loc.x > 0) 
        {
            auto i = index(loc.x-1, loc.y);
            if (data[i].p == loc.p) 
            {
                neighbours.push_back(&(data[i]));
            }
        }
        // Up
        if (loc.y > 0) 
        {
            auto i = index(loc.x, loc.y-1);
            if (data[i].p == loc.p) 
            {
                neighbours.push_back(&data[i]);
            }
        }
        // Right
        if (loc.x < W-1) 
        {
            auto i = index(loc.x+1, loc.y);
            if (data[i].p == loc.p) 
            {
                neighbours.push_back(&data[i]);
            }
        }
        // Down
        if (loc.y < H-1) 
        {
            auto i = index(loc.x, loc.y+1);
            if (data[i].p == loc.p) 
            {
                neighbours.push_back(&data[i]);
            }
        }
    }

    void reset() 
    {
        // for (uint16_t i : endIndexes) 
        // {
        //     data[i].reset();
        // }
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

    std::vector<Location> data;

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
    auto map = read_input("../day12/" DATA);
#ifdef TEST
    map->print();
#endif

    map->run(false);


}

void part2() 
{
    auto map = read_input("../day12/" DATA);
#ifdef TEST
    map->print();
#endif
    map->run(true);
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 12" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


