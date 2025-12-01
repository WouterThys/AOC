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

// not 0: 456 steps, 124 turns -> 124456
// 538

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 17
#define H W
#else
#define DATA "data.txt"
#define W 141
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

/* could have made this smarter with 90 degrees further always ++ U -> R -> ...*/
enum Direction
{
    L, U, D, R
};

/* Movements for all directions */
                         //L  U  D  R
constexpr int rowNum[] = {-1, 0, 0, 1};
constexpr int colNum[] = {0, -1, 1, 0};

struct Position
{
    uint16_t x;
    uint16_t y;
    char v;
    bool visited = false;

    Position() : x{0}, y{0}, v{'.'}
    {

    }

    // no copy 
    Position(Position& other) = delete;
    Position(Position&& other) = delete;
    Position & operator=(const Position&) = delete;

    void init(uint16_t x, uint16_t y, char v)
    {
        this->x = x;
        this->y = y;
        this->v = v;
    }

    char print() const
    {
        if (visited) 
        {
            return 'x';
        }
        return v;
    }

    bool isEmpty() const 
    {
        return v == '.';
    }

    bool isWall() const 
    {
        return v == '#';
    }

    void visit() 
    {
        visited = true;
    }

    void clear() 
    {
        visited = false;
    }

};

// A Data Structure for queue used in BFS
struct Node
{
    Position& pt;  // The coordinates of a cell
    Direction d;
    uint32_t steps;
    uint32_t turns;

    void move(const Direction nd) 
    {
        if (nd == d) 
        {
            steps++;
        }
        else 
        {
            // Cannot go back so if not same direction this is a turn
            steps++;
            turns++;
        }

        d = nd;
    }

    uint64_t score() const
    {
        return (1000 * turns) + steps;
    }
};

bool nodeSorter(Node const& lhs, Node const& rhs)
{
    return lhs.score() < rhs.score();
};

class Maze 
{
public:

    Maze() { }

    void init(const std::string& input) 
    {
        // Read the map
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
                    start.x = x;
                    start.y = y;
                    v = '.';
                }
                if (v == 'E') 
                {
                    dest.x = x;
                    dest.y = y;
                    v = '.';
                }

                data[i].init(x, y, v);
                read_pos++;
            }
        }
    }

    void solve() 
    {
        // https://www.geeksforgeeks.org/shortest-path-in-a-binary-maze/

        // Mark the source cell as visited
        auto& src = data[index(start.x, start.y)];
        src.visit();

        // Create a queue for BFS
        std::queue<Node> q;

        // Distance of source cell is 0
        Node s = { .pt = src, .d = R, 0, 0};
        q.push(s);  // Enqueue source cell

        // Do a BFS starting from source cell
        while (!q.empty())
        {
            Node& curr = q.front();
            Position& pt = curr.pt;

            // If we have reached the destination cell, we are done
            // As all paths are searched together, the first hit will also be the shortest
            if (pt.x == dest.x && pt.y == dest.y) 
            {
                q.pop();
                paths.push_back(curr);
                continue;
            }

            // Otherwise dequeue the front cell in the queue and enqueue its adjacent cells
            q.pop();

            for (uint16_t i = 0; i < 4; i++) 
            {
                uint16_t nx = pt.x + rowNum[i];
                uint16_t ny = pt.y + colNum[i];

                // Valid, and not yet visited
                if (valid(nx, ny)) 
                {
                    auto& pos = data[index(nx, ny)];
                    bool isDest = pos.x == dest.x && pos.y == dest.y;
                    if (isDest || (pos.isEmpty() && !pos.visited)) 
                    {
                        pos.visit();
                        Node n = { .pt = pos, .d = curr.d, .steps = curr.steps, .turns = curr.turns }; // copy
                        n.move((Direction)i);
                        q.push(n);
                    }
                }
            }
        
#ifdef TEST
            system("clear");
            print();
            std::cout << std::endl;
#endif
        
        }
    }

    uint64_t count() 
    {
        uint64_t lowest = UINT64_MAX;

        for (int i = 0; i < paths.size(); i++) 
        {
            auto score = paths[i].score();
            if (score < lowest) 
            {
                lowest = score;
            }

//#ifdef TEST
            std::cout << i << ": " << paths[i].steps << " steps, " << paths[i].turns << " turns -> " << score << std::endl;
//#endif
        }

        return lowest;
    }

    void print() const 
    {
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                if (start.x == x && start.y == y) 
                {
                    std::cout << "S";
                }
                else if (dest.x == x && dest.y == y) 
                {
                    std::cout << "E";
                }
                else 
                {
                    auto i = index(x, y);
                    std::cout << data[i].print() << "";
                }
            }
            std::cout << std::endl;
        }
    }

private:

    std::unique_ptr<Position[]> data;
    std::vector<Direction> moves;
    Position start;
    Position dest;

    std::vector<Node> paths;
};

std::unique_ptr<Maze> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto board = std::unique_ptr<Maze>(new Maze());
        board->init(buffer.str());
        return std::move(board);
    }   
    else 
    {
        std::cout << "Bad input file " << filename << std::endl;
        return nullptr;
    }
}


void part1() 
{
    auto maze = read_input("../day16/" DATA);
    maze->solve();
//#ifdef TEST
    maze->print();
//#endif
    auto count = maze->count();
    std::cout  << "Result: " << count << std::endl;
}

void part2() 
{
    auto maze = read_input("../day16/" DATA);
    maze->solve();

#ifdef TEST
    maze->print();
#endif    
    auto count = maze->count();
    std::cout  << "Result: " << count << std::endl;
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 15" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
