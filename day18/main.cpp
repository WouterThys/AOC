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



#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 7
#define H W
#else
#define DATA "data.txt"
#define W 70
#define H W
#endif

static uint16_t index(uint16_t x, uint16_t y) 
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
    bool corrupt_ = false;
    bool visited_ = false;

    Position() : x{0}, y{0}
    {

    }

    Position(uint16_t& x, uint16_t& y) : x{x}, y{y}
    {

    }

    Position(const uint16_t x, const uint16_t y) : x{x}, y{y}
    {

    }

    // no copy 
    Position(Position& other) = delete;
    Position(Position&& other) = default;
    Position & operator=(const Position&) = delete;

    void init(uint16_t x, uint16_t y)
    {
        this->x = x;
        this->y = y;
    }

    bool isCorrupt() const 
    {
        return corrupt_;
    }

    bool isVisited() const 
    {
        return visited_;
    }

    char print() const
    {
        if (visited_) 
        {
            return 'x';
        }
        else if (corrupt_) 
        {
            return '#';
        }
        return '.';
    }

    void corrupt() 
    {
        corrupt_ = true;
    }

    void visit() 
    {
        visited_ = true;
    }

    void clear() 
    {
        visited_ = false;
        corrupt_ = false;
    }

};

/// @brief Used for Dijkstra algo
/// https://github.com/ra101/Maze-Solver-Cpp/blob/master/dijkstra.h
struct Node 
{
    uint16_t dist;
    Position* pos;
};

bool operator<(const Node& n1, const Node& n2) 
{
    return n1.dist < n2.dist;
}


class Memory 
{
public:

    Memory() { }

    void init(const std::string& input) 
    {
        // Read the map
        data = std::unique_ptr<Position[]>(new Position[W*H]);

        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                auto i = index(x, y);
                data[i].init(x, y);
            }
        }
        
        std::istringstream istr(input);
        std::string line;
        while (std::getline(istr, line))
        {
            Position p;

            std::istringstream lStr(line);
            std::string nrStr;
            std::getline(lStr, nrStr, ',');
            p.x = stoi(nrStr);
            std::getline(lStr, nrStr);
            p.y = stoi(nrStr);
            p.corrupt();

            incoming.emplace_back(std::move(p));
        }
    }

    void solve() 
    {
        makeCorrupt(12);

        dijkstra(data[0], data[(W*H)-1]);

        
#ifdef TEST
            system("clear");
            print();
            std::cout << std::endl;
#endif
        
    }

    void dijkstra(Position& src, Position& dst) 
    {
        // A queue sorting the nodes by distance
        std::priority_queue<Node> q; // Node - iPair<dist,id>

        // Create a vector for distances and initialize all distances as infinite (INT16_MAX)
        std::vector<uint16_t> dist(W*H, INT16_MAX);

        // Insert source itself in priority queue and initialize its distance as 0.
        q.push({ .dist=0, .pos=&src });
        dist[index(src.x,src.y)] = 0;

        /* Looping till priority queue becomes empty (or all distances are not finalized) */
        while (!q.empty()) 
        {

#ifdef TEST
            system("clear");
            print();
            std::cout << std::endl;
#endif

            // The first vertex in pair is the minimum distance vertex, extract it from priority queue.
            auto& u = q.top();
            u.pos->visit();
            q.pop();

            auto uNdx = index(u.pos->x, u.pos->y);

            // Loop neighbors
            for (int i = 0; i < 4; i++) 
            {
                uint16_t nx = u.pos->x + rowNum[i];
                uint16_t ny = u.pos->y + colNum[i];

                if (valid(nx, ny)) 
                {
                    auto nInd = index(nx, ny);
                    auto& nPos = data[nInd];

                    if (!nPos.isVisited() && !nPos.isCorrupt()) 
                    {
                        // If there is shorted path to v through u.
                        if (dist[nInd] > dist[uNdx] + 1) 
                        {
                            // Updating distance of v
                            dist[nInd] = dist[uNdx] + 1;
                            q.push({ .dist=dist[nInd], .pos=&nPos });
                        }
                    }
                }
            }
        }

    }

    /// @brief Apply first x corruptions to the memory
    /// @param x number of corruptions to corrupt (when less than corruption list)
    void makeCorrupt(int x) 
    {
        for (int i = 0; i < x && i < incoming.size(); i++) 
        {
            const auto& c = incoming[i];
            data[index(c.x, c.y)].corrupt();
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

    std::unique_ptr<Position[]> data;
    std::vector<Position> incoming;
    
};

std::unique_ptr<Memory> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Memory>(new Memory());
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
    auto ptr = read_input("../day18/" DATA);
    ptr->solve();
}

void part2() 
{
    auto ptr = read_input("../day18/" DATA);
    ptr->solve();  
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 18" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
