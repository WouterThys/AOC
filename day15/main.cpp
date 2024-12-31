#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

using namespace std;

//#define TEST
#define PART 2

#ifdef TEST
#define DATA "test.txt"
#define W (PART*10)
#define H 10
#else
#define DATA "data.txt"
#define W (PART*50)
#define H 50
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
    L, U, R, D
};

struct Position
{
    uint16_t x;
    uint16_t y;
    char v;

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

    void makeObstacle() 
    {
        v = 'O';
    }

    void makeEmpty() 
    {
        v = '.';
    }

    char print() const
    {
        return v;
    }

    bool canVisitHere() const 
    {
        return isEmpty();
    }

    bool isEmpty() const 
    {
        return v == '.';
    }

    bool isWall() const 
    {
        return v == '#';
    }

    bool isObstacle() const 
    {
        return v == 'O' || v == '[' || v == ']';
    }

    void toggle() 
    {
        if (v == '[') 
        {
            v = ']';
        }
        else if (v == ']') 
        {
            v = '[';
        }
        else 
        {
            std::cout << "ERROR This is not an obstacle" << std::endl;
        }
    }

    uint16_t indexOfOther() const 
    {
        if (v == '[') 
        {
            return index(x+1, y);
        }
        else if (v == ']') 
        {
            return index(x-1, y);
        }
        else 
        {
            std::cout << "This is not an obstacle" << std::endl;
            return UINT16_MAX;
        }
    }
};

struct Robot
{
    uint16_t x;
    uint16_t y;

    void visit(const Position& pos) 
    {
        x = pos.x;
        y = pos.y;
    }

    bool isHere(uint16_t x, uint16_t y) const
    {
        return this->x == x && this-> y == y;
    }

};

class Floor 
{
public:

    Floor() { }

    void initSingle(const string& input) 
    {
        // Read the map
        data = unique_ptr<Position[]>(new Position[W*H]);
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

                if (v == '@') 
                {
                    robot.x = x;
                    robot.y = y;
                    
                    v = '.';
                    startIndex = i;
                }

                data[i].init(x, y, v);
                read_pos++;
            }
        }

        initMovements(input);
    }

    void initDouble(const string& input) 
    {
        // Read the map
        data = unique_ptr<Position[]>(new Position[W*H]);
        int read_pos = 0;
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x += 2) 
            {
                char v = input[read_pos];
                while(v == '\n' || v == '\r') 
                {
                    read_pos++;
                    v = input[read_pos];
                }

                auto i = index(x, y);

                if (v == '@') 
                {
                    robot.x = x;
                    robot.y = y;
                    
                    v = '.';
                    startIndex = i;
                }
                if (v == 'O') 
                {
                    data[i].init(x, y, '[');
                    data[i+1].init(x+1, y, ']');
                }
                else 
                {
                    data[i].init(x, y, v);
                    data[i+1].init(x+1, y, v);
                }
                read_pos++;
            }
        }

        initMovements(input);
    }

    void initMovements(const string& input) 
    {
        // Read the directions
        istringstream f(input.substr(H+1));
        string line;    
        while (std::getline(f, line)) 
        {
            for (char c : line) 
            {
                switch (c)
                {
                case '<': moves.push_back(L); break;
                case '^': moves.push_back(U); break;
                case '>': moves.push_back(R); break;
                case 'v': moves.push_back(D); break;
                default: break;
                }
            }
        }
    }

    uint16_t nextPositionIndex(const Position& loc, const Direction d)
    {
        uint16_t nx = loc.x;
        uint16_t ny = loc.y;
        switch (d)
        {
            case L: nx--; break;
            case R: nx++; break;
            case U: ny--; break;
            case D: ny++; break;
        }
        return index(nx, ny);
    }

    Position& nextPosition(const Position& loc, const Direction d)
    {
        uint16_t index = nextPositionIndex(loc, d);
        return data[index];
    }

    void moveRobot() 
    {
        for (const auto& d : moves) 
        {
#ifdef TEST
            system("clear");
            print();
#endif

            // Current position
            const auto& cPos = data[index(robot.x, robot.y)];

            // New position
            auto& nPos = nextPosition(cPos, d);

            if (nPos.canVisitHere()) 
            {
                // Ok, move here
                robot.visit(nPos);
                continue;
            }
            if (nPos.isObstacle()) 
            {
                if (PART == 1) 
                {
                    Position* emptyPos = findEmptyPosition(&nPos, d);

                    // If empty found make it obstacle and move to nPos
                    if (emptyPos) 
                    {
                        emptyPos->makeObstacle();
                        nPos.makeEmpty();
                        robot.visit(nPos);
                    }
                }
                else 
                {
                    if (d == L || d == R) 
                    {
                        Position* emptyPos = findEmptyPosition(&nPos, d);
                        if (emptyPos) 
                        {
                            if (d == L) 
                            {
                                emptyPos->v = '[';
                                for (int x = emptyPos->x+1; x < nPos.x; x++) 
                                {
                                    data[index(x, nPos.y)].toggle();
                                }
                            }
                            else 
                            {
                                emptyPos->v = ']';
                                for (int x = emptyPos->x-1; x > nPos.x; x--) 
                                {
                                    data[index(x, nPos.y)].toggle();
                                }
                            }
                            nPos.makeEmpty();
                            robot.visit(nPos);
                        }
                    }
                    else 
                    {
                        set<uint16_t> pos;
                        if (recurse(&nPos, d, pos)) 
                        {
                            if (d == D) 
                            {
                                auto iter = pos.end();
                                while (iter != pos.begin())
                                {
                                    --iter;

                                    auto& val = data[*iter];
                                    auto& next = data[index(val.x, val.y+1)];

                                    next.v = val.v;
                                    val.makeEmpty();

                                    // print();
                                    // std::cout << std::endl;
                                }
                            }
                            else 
                            {
                                auto iter = pos.begin();
                                while (iter != pos.end())
                                {
                                    auto& val = data[*iter];
                                    auto& next = data[index(val.x, val.y-1)];

                                    next.v = val.v;
                                    val.makeEmpty();

                                    // print();
                                    // std::cout << std::endl;
                                    iter++;
                                }
                            }
                            
                            nPos.makeEmpty();
                            robot.visit(nPos);
                        }
                    }
                }
            }
        }
    }

    Position* findEmptyPosition(Position* nPos, const Direction d) 
    {
        // Get next pos
        auto oPos = &nextPosition(*nPos, d);

        // Wall = stop
        if (oPos->isWall()) 
        {
            return nullptr;
        }
        // Empty = found
        if (oPos->isEmpty()) 
        {
            // Found!!
            return oPos;
        }
        // Otherwise recurse
        return findEmptyPosition(oPos, d);
    }

    bool recurse(Position* p, const Direction d, set<uint16_t>& positions) 
    {
        auto p1 = &nextPosition(*p, d);
        auto other = &data[p->indexOfOther()];
        auto p2 = &nextPosition(*other, d);
        
        positions.emplace(index(p->x, p->y));
        positions.emplace(index(other->x, other->y));
        
        if (p1->isWall() || p2->isWall()) 
        {
            return false;
        }
        if (p1->isEmpty() && p2->isEmpty()) 
        {
            // Ok!
        }
        else 
        {
            if (p1->isObstacle()) 
            {
                if (!recurse(p1, d, positions)) 
                {
                    return false;
                }
            }
            if (p2->isObstacle()) 
            {
                if (!recurse(p2, d, positions)) 
                {
                    return false;
                }
            }
        }
        return true;
    }

    uint32_t count() const 
    {
        uint32_t count = 0;
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                const auto& d = data[index(x, y)];
                if (d.isObstacle()) 
                {
                    if (PART == 1) 
                    {
                        count += ( 100 * d.y + d.x );
                    }
                    else 
                    {
                        if (d.v == '[') 
                        {
                            count += ( 100 * d.y + d.x );
                        }
                    }
                }
            }
        }
        return count;
    }

    void print() const 
    {
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                if (robot.isHere(x, y)) 
                {
                    cout << "@" << "";
                }
                else 
                {
                    auto i = index(x, y);
                    cout << data[i].print() << "";
                }
            }
            cout << endl;
        }
    }



private:
    Robot robot;
    unique_ptr<Position[]> data;
    vector<Direction> moves;
    uint16_t startIndex;
};

unique_ptr<Floor> read_input(string filename, bool single) 
{
    ifstream infile(filename);
    stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto board = unique_ptr<Floor>(new Floor());
        if (single) 
        {
            board->initSingle(buffer.str());
        }
        else 
        {
            board->initDouble(buffer.str());
        }
        return std::move(board);
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }
}


void part1() 
{
    auto floor = read_input("../day15/" DATA, true);
    floor->moveRobot();
#ifdef TEST
    floor->print();
#endif
    auto count = floor->count();
    cout  << "Result: " << count << endl;
}

void part2() 
{
    auto floor = read_input("../day15/" DATA, false);
    floor->moveRobot();

#ifdef TEST
    floor->print();
#endif    
    auto count = floor->count();
    cout  << "Result: " << count << endl;
}

int main(int argc, char* argv[]) 
{
    cout  << "Hallo day 15" << endl;
#ifdef TEST
    cout  << "TEST TEST TEST" << endl;
#endif

    if (PART == 1) 
    {
        part1();
    }
    else 
    {
        part2();
    }

    return 0;
}
