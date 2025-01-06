#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 15
#define H W
#define N 100
#else
#define DATA "data.txt"
#define W 141
#define H W
#define N 100
#endif

static uint32_t index(uint16_t x, uint16_t y) 
{
    return (x + (y * W));
}

static bool valid(uint16_t x, uint16_t y) 
{
    return (x < W && y < H);
}

enum Direction
{
    L, U, R, D
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

struct Position
{
    uint16_t x;
    uint16_t y;
    char v;

    Position() : x{0}, y{0}
    {

    }

    Position(uint16_t& x, uint16_t& y, char& v) : x{x}, y{y}, v{v}
    {

    }

    Position(const uint16_t x, const uint16_t y, const char v) : x{x}, y{y}, v{v}
    {

    }

    // no copy 
    Position(Position& other) = delete;
    Position(Position&& other) = default;
    Position & operator=(const Position&) = delete;

    void init(uint16_t x, uint16_t y, char v)
    {
        this->x = x;
        this->y = y;
        this->v = v;
    }

    bool isWall() const 
    {
        return v == '#';
    }

    bool isEmpty() const 
    {
        return v == '.' || v == 'c';
    }

    char print() const
    {
        return v;
    }

    void cheat() 
    {
        v = 'c';
    }

    void reset() 
    {
        v = '#';
    }

};

/// @brief Used for Dijkstra algo
/// https://github.com/ra101/Maze-Solver-Cpp/blob/master/dijkstra.h
struct Node 
{
    uint32_t dist;
    uint32_t pos; // Index of position in data list
};

struct Cheat 
{
    uint32_t start;
    uint32_t wall;
    uint32_t end;
};

bool operator<(const Node& n1, const Node& n2) 
{
    /* Priority queue always returns highest value so sort from high -> low */
    if (n1.dist != n2.dist) return n1.dist > n2.dist;
    if (n1.pos != n2.pos) return n1.pos > n2.pos;
    return false;
}

class Maze 
{
public:

    Maze() { }

    void init(const std::string& input) 
    {
        data = std::unique_ptr<Position[]>(new Position[W*H]);
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

                if (v == 'S') 
                {
                    startIndex = i;
                    v = '.';
                }
                if (v == 'E') 
                {
                    endIndex = i;
                    v = '.';
                }

                data[i].init(x, y, v);
                read_pos++;
            }
        }
    
        route = std::vector<uint32_t>(W*H, UINT32_MAX);
    }

    void initCheats() 
    {
        cheatPositions.clear();
        dijkstra(getStartNdx(), getEndNdx(), true);
    }

    uint32_t findRoute() 
    {
        auto res = dijkstra(getEndNdx(), getStartNdx(), false);
#ifdef TEST
        system("clear");
        print(&route);
        std::cout << std::endl;
#endif
        return res;
    }

    uint32_t runCheat(const Cheat& cheat) 
    {
        return dijkstra(getStartNdx(), getEndNdx(), false, &cheat);
    }

    int dijkstra(uint32_t start, uint32_t end, bool findCheats, const Cheat* cheat = nullptr) 
    {
        Position& src = data[start];
        Position& dst = data[end];

        // A queue
        std::priority_queue<Node> q;

        // Create a vector for distances and initialize all distances as infinite (INT16_MAX)
        std::vector<uint32_t> dist(W*H, UINT32_MAX);

        // Insert source itself in priority queue and initialize its distance as 0.
        auto ndx = index(src.x,src.y);
        q.push({ .dist=0, .pos=ndx });
        dist[ndx] = 0;

        /* Looping till priority queue becomes empty (or all distances are not finalized) */
        while (!q.empty()) 
        {

#ifdef TEST
            system("clear");
            print(&dist, cheat);
            std::cout << std::endl;
#endif

            // The first vertex in pair is the minimum distance vertex, extract it from priority queue.
            const auto  node = q.top();
            const auto& pos = data[node.pos];
            q.pop();

            if (findCheats) 
            {
                checkCheatFromPos(pos, dist);
            }
            else if (cheat == nullptr) 
            {
                // Update route
                route[node.pos] = node.dist;
            }

            // Check cheat, and if cheat should be applied here
            if (cheat && node.pos == cheat->start) 
            {
                const auto  cNdx = cheat->end;
                const auto& cPos = data[cNdx];

                /* ./day_20  11.59s user 0.01s system 99% cpu 11.634 total */

                // // Check if end reached
                // if (cPos.x == dst.x && cPos.y == dst.y) 
                // {
                //     // End reached!!
                //     std::cout << "Early out with cheat!!!!" << std::endl;
                //     return dist[node.pos] + 1;
                // }

                // // This should always be true?
                // if (dist[cNdx] > dist[node.pos] + 1) 
                // {
                //     // Updating distance of v
                //     dist[cNdx] = dist[node.pos] + 1;
                //     q.push({ .dist=dist[cNdx], .pos=cNdx });
                // }

                /* Double as fast: ./day_20  5.59s user 0.01s system 99% cpu 5.606 total */
                return dist[node.pos] + route[cNdx];
            }
            else 
            {
                // Loop neighbors
                for (int i = 0; i < 4; i++) 
                {
                    uint16_t nx = pos.x + rowNum[i];
                    uint16_t ny = pos.y + colNum[i];

                    if (valid(nx, ny)) 
                    {
                        auto nInd = index(nx, ny);
                        auto& nPos = data[nInd];

                        if (!nPos.isWall()) 
                        {
                            // Check if end reached
                            if (nPos.x == dst.x && nPos.y == dst.y) 
                            {
                                // End reached!!
                                //std::cout << "Early out" << std::endl;
                                return dist[node.pos] + 1;
                            }

                            // If there is shorted path to v through u.
                            if (dist[nInd] > dist[node.pos] + 1) 
                            {
                                // Updating distance of v
                                dist[nInd] = dist[node.pos] + 1;
                                q.push({ .dist=dist[nInd], .pos=nInd });
                            }
                        }
                    }
                }
            }
        }

        return -1;
    }

    void checkCheatFromPos(const Position& pos, const std::vector<uint32_t>& dist)
    {
        // Move over the wall in each of the positions

        // 1. move 1 position and check if it is a wall
        // 2. move 1 position in that same direction and check if it is '.'
        // 3. also check that this found position is not already visited -> make sure we are not adding loops

        for (int i = 0; i < 4; i++) 
        {
            uint16_t nx = pos.x + rowNum[i];
            uint16_t ny = pos.y + colNum[i];

            if (valid(nx, ny)) 
            {
                // Wall index and position
                auto wNdx = index(nx, ny);
                auto& wPos = data[wNdx];

                if (wPos.isWall()) 
                {
                    // Proceed one step further
                    nx = nx + rowNum[i];
                    ny = ny + colNum[i];

                    if (valid(nx, ny)) 
                    {
                        // Possible cheat index and position
                        auto cNdx = index(nx, ny);
                        auto& cPos = data[cNdx];

                        if (cPos.isEmpty()) 
                        {
                            auto pNdx = index(pos.x, pos.y);

                            auto pDist = dist[pNdx];
                            auto cDist = dist[cNdx];

                            // If this is the case than we went back on the path
                            if (cDist > pDist) 
                            {
                                // Valid cheat!!
                                cheatPositions.push_back(Cheat{.start=pNdx, .wall=wNdx, .end=cNdx});
                            }
                        }
                    }
                }
            }
        }
    }

    size_t getNrCheats() const 
    {
        return cheatPositions.size();
    }

    std::vector<Cheat>& getCheatPositions() 
    {
        return cheatPositions;
    } 

    void setCheat(uint32_t pos) 
    {
        data[pos].cheat();
    }

    void reset(uint32_t pos) 
    {
        data[pos].reset();
    }

    void print(std::vector<uint32_t>* dist, const Cheat* cheat = nullptr) const 
    {
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                auto i = index(x, y);
                if (i == startIndex) 
                {
                    std::cout << "S ";
                }
                else if (i == endIndex) 
                {
                    std::cout << "E ";
                }
                else if (cheat && i == cheat->wall) 
                {
                    std::cout << "1 ";
                }
                else if (cheat && i == cheat->end) 
                {
                    std::cout << "2 ";
                }
                else if (dist) 
                {
                    auto d = dist->at(i);
                    if (d < UINT32_MAX) 
                    {
                        //std::cout << "O ";        
                        if (d < 10) 
                        {
                            std::cout << d << " ";
                        }
                        else 
                        {
                            std::cout << d << "";
                        }
                    }
                    else 
                    {
                        std::cout << data[i].print() << " ";
                    }
                }
                else 
                {
                    std::cout << data[i].print() << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    uint32_t getStartNdx() const 
    {
        return startIndex;
    }

    uint32_t getEndNdx() const 
    {
        return endIndex;
    }

private:

    std::unique_ptr<Position[]> data;
    uint32_t startIndex{0};
    uint32_t endIndex{0};

    std::vector<uint32_t> route;
    std::vector<Cheat> cheatPositions; // Map with cheat positions (start -> end)
    
};

std::unique_ptr<Maze> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Maze>(new Maze());
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
    auto ptr = read_input("../day20/" DATA);
    // Run backwards to set route
    ptr->initCheats();

    int withoutCheat = ptr->findRoute();
    int save = 0;
    int time = 0;
    std::map<uint32_t, uint32_t> costs;

    std::cout << "No cheats: " << withoutCheat << std::endl;

    const auto& cheats = ptr->getCheatPositions();
    //std::vector<Cheat> cheats { Cheat{.start=index(8,7), .wall=index(8,8), .end=index(8,9)} };
    //std::vector<Cheat> cheats { Cheat{.start=index(7,1), .wall=index(8,1), .end=index(9,1)} };
    for (const auto& cheat : cheats) 
    {
        time = ptr->runCheat(cheat) + 1;
        save = withoutCheat - time;
        costs[save]++;
    }

    int total = 0;
    for (const auto& pair : costs) 
    {
        //std::cout << "There are " << pair.second << " cheats that save " << pair.first << " picoseconds." << std::endl;
        if (pair.first >= 100) 
        {
            total += pair.second;
        }
    }

    std::cout << "Total bigger than 100: " << total << std::endl;

}

void part2() 
{
    auto ptr = read_input("../day20/" DATA);

    // Naive

    // for (int i = N+1; i < ptr->max(); ++i) 
    // {
    //     int res = ptr->dijkstra();
    //     if (res < 0) 
    //     {
    //         std::cout << "First block at " << i << " ";
    //         ptr->printBlock(i);
    //         return;
    //     }
    // }

}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 20" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
