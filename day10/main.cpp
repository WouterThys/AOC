#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <regex>
#include <cctype>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 8
#define H W
#else
#define DATA "data.txt"
#define W 45
#define H W
#endif

#define ASCII_TO_CHAR(v) (v-48)
#define CHAR_TO_ASCII(c) (c+48)

static uint32_t index(uint16_t x, uint16_t y) 
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
    L, R, U, D
};


/* Movements for all directions, use combination like
    for (i..4) {
     x = rowNum[i];
     y = colNum[i];
    }
 */
constexpr int rowNum[] = {-1,  0,  1,  0};
constexpr int colNum[] = { 0, -1,  0,  1};
                         //L   U   R   D

struct Location {

    uint16_t x;
    uint16_t y;
    uint8_t h;

    bool isValid() const 
    {
        return valid(x, y);
    }

    char print() const 
    {
        return CHAR_TO_ASCII(h);
    }

};

struct Node 
{
    uint32_t dist;
    uint32_t pos;
};

bool operator<(const Node& n1, const Node& n2) 
{
    /* Priority queue always returns highest value so sort from high -> low */
    if (n1.dist != n2.dist) return n1.dist > n2.dist;
    if (n1.pos != n2.pos) return n1.pos > n2.pos;
    return false;
}

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

    void run(bool distinctScore)
    {
        int sum = 0;
        for (const auto index : startIndexes) 
        {
            int score = 0;
            walkRoute(index, score, distinctScore);

            reset();

            sum += score;
        }

        std::cout << "Total score: " << sum << std::endl;
    }

    void walkRoute(uint32_t srcNdx, int& score, bool distinctScore)
    {

        std::priority_queue<Node> q;
        std::vector<uint32_t> dist(W*H, UINT32_MAX);
        std::vector<uint32_t> found;

        const auto& src = data[srcNdx];
        dist[srcNdx] = 0;

        q.push({ .dist=0, .pos=srcNdx });

        while (!q.empty()) 
        {
            const auto node = q.top();
            const auto pos = data[node.pos];
            q.pop();

            if (pos.h == 9) 
            {
                if (distinctScore) 
                {
                    // Only when not already been here increment
                    if (std::find(found.begin(), found.end(), node.pos) == found.end()) 
                    {
                        // Increment score, do not add to queue because this is dst
                        score++;
                        found.push_back(node.pos);
                    }
                }
                else 
                {
                    score++;
                }
            }

            for (int i = 0; i < 4; i++) 
            {
                uint16_t nx = pos.x + rowNum[i];
                uint16_t ny = pos.y + colNum[i];

                if (valid(nx, ny)) 
                {
                    const auto  nNdx = index(nx, ny);
                    const auto& nPos = data[nNdx];

                    if (nPos.h == (pos.h + 1)) 
                    {
                        // Only move on when h + 1
                        dist[nNdx] = node.dist + 1;
                        q.push({ .dist=dist[nNdx], .pos=nNdx });
                    }
                }
            }
        }
    }

    void reset() 
    {

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
    std::vector<uint32_t> startIndexes;
    std::vector<uint32_t> endIndexes;

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
    map->run(true);
}

void part2() 
{
    auto map = read_input("../day10/" DATA);
#ifdef TEST
    map->print();
#endif
    map->run(false);
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


