#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <memory>
#include <regex>

#include <chrono>
#include <thread>

using namespace std;

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 10
#define H W
#else
#define DATA "data.txt"
#define W 130
#define H W
#endif

const int GO_ON = 0;
const int OFF_BOARD = 1;
const int STUCK = 2;

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

static Direction turn90degrees(const Direction d)
{
    switch (d)
    {
    default:
    case L: return U; break;
    case R: return D; break;
    case U: return R; break;
    case D: return L; break;
    case X: return X; break;
    }
}

static Direction opposite(const Direction d)
{
    switch (d)
    {
    default:
    case L: return U; break;
    case R: return D; break;
    case U: return R; break;
    case D: return L; break;
    case X: return X; break;
    }
}

struct Position
{
    uint16_t x;
    uint16_t y;

    mutable char v { '.' };
    mutable bool passedL { false };
    mutable bool passedR { false };
    mutable bool passedD { false };
    mutable bool passedU { false };

    Position() : Position(0, 0, 0)
    {

    }

    Position(uint16_t x_, uint16_t y_, char v_) : x{x_}, y{y_}, v{v_}
    {

    }

    void reset() const
    {
        if (v == '#') 
        {
            v = '#';
        }
        else 
        {
            v = '.';
        }
        passedL = false;
        passedR = false;
        passedD = false;
        passedU = false;
    }

    char print() const
    {
        if (passedH() && passedV())
        {
            return '+';
        }
        else if (passedH() && !passedV())
        {
            return '-';
        }
        else if (!passedH() && passedV())
        {
            return '|';
        }
        else 
        {
            return v;
        }
    }

    bool isMarked() const
    {
        return passedH() || passedV();
    }

    bool isCrossed() const
    {
        return passedH() && passedV();
    }

    bool mark(const Direction d) const
    {
        bool ok = true;
        switch (d)
        {
        case U:
            if (passedU) ok = false;
            passedU = true;
            break;
        case D:
            if (passedD) ok = false;
            passedD = true;
            break;
        case L:
            if (passedL) ok = false;
            passedL = true;
            break;
        case R:
            if (passedR) ok = false;
            passedR = true;
            break;
        default:
            break;
        }
        return ok;
    }

    bool isEmpty() const 
    {
        return v == '.';
    }

    bool isObstacle() const 
    {
        return v == '#' || v == 'O';
    }

    bool isValid() const 
    {
        return valid(x, y);
    }

    bool passedH() const 
    {
        return passedL || passedR;
    }

    bool passedV() const 
    {
        return passedU || passedD;
    }

    void makeObstacle() const 
    {
        v = 'O';
    }

    void makeClear() const 
    {
        v = '.';
    }
};

struct Guard
{
    uint16_t x;
    uint16_t y;
    Direction d;

    ///
    /// \brief nextPosition this would be the ignorant next position the guard would move
    /// \param nx new x
    /// \param ny new y
    ///
    void nextPosition(uint16_t& nx, uint16_t& ny)
    {
        nx = x;
        ny = y;
        switch (d)
        {
            case L: nx--; break;
            case R: nx++; break;
            case U: ny--; break;
            case D: ny++; break;
        }
    }

    void visit(const Position& pos) 
    {
        x = pos.x;
        y = pos.y;
    }

    /// @brief calculate new direction depending on current direction
    /// always turn 90 degrees.
    void turn() 
    {
        d = turn90degrees(d);
    }

    bool isHere(uint16_t x, uint16_t y) const
    {
        return this->x == x && this-> y == y;
    }

    char print() const 
    {
        switch (d)
        {
            case L: return '<';
            case R: return '>';
            case U: return '^';
            case D: return 'v';
            default: return 'E';
        }
        return 0;
    }
};

class Floor 
{
public:

    Floor(const string input) 
    {
        auto array = unique_ptr<Position[]>(new Position[W*H]);
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

                if (v == '^') 
                {
                    guard.x = x;
                    guard.y = y;
                    guard.d = U;
                    v = '.';
                    //array[i].mark(U);
                    startIndex = i;
                }

                array[i].x = x;
                array[i].y = y;
                array[i].v = v;
                read_pos++;
            }
        }
    
        data = unique_ptr<const Position[]>(std::move(array));
    }

    void print() const 
    {
        for (uint16_t y = 0; y < H; y++) 
        {
            for (uint16_t x = 0; x < W; x++) 
            {
                if (guard.isHere(x, y)) 
                {
                    cout << guard.print() << "";
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

    int moveGuard() 
    {
        uint16_t nx = 0;
        uint16_t ny = 0;
        guard.nextPosition(nx, ny);

        const auto& curPos = data[index(guard.x, guard.y)];
        bool ok = curPos.mark(guard.d);

        if (!ok)
        {
            // Done, moved off the floor
#ifdef TEST
            cout << "Stuck!" << endl;
#endif
            return STUCK;
        }

        if (!valid(nx, ny))
        {
            // Done, moved off the floor
#ifdef TEST
            cout << "Moved off the board!" << endl;
#endif
            return OFF_BOARD;
        }

        const auto& newPos = data[index(nx, ny)];

        if (newPos.isEmpty()) 
        {
            // Ok, move there
            guard.visit(newPos);
            moves++;
        }
        else 
        {
            // Not ok, turn
            guard.turn();
        }

        return GO_ON;
    }

    uint16_t getMoves() const
    {
        return moves;
    }

    void countDistinct(uint16_t& count)
    {
        count = 0;
        for (uint16_t y = 0; y < H; y++)
        {
            for (uint16_t x = 0; x < W; x++)
            {
                auto i = index(x, y);
                const auto& p = data[i];
                if (p.isValid())
                {
                    if (p.isMarked()) 
                    {
                        count++;
                        if (i != startIndex) 
                        {
                            passedIndexes.push_back(i);
                        }
                    }
                }
            }
        }
    }

    void markAsObstacle(uint16_t index) 
    {
        data[index].makeObstacle();
    }

    void removeObstacle(uint16_t index) 
    {
        data[index].makeClear();
    }

    vector<uint16_t> getPassedPositions() 
    {
        return passedIndexes;
    }

    void reset()
    {
        for (uint16_t y = 0; y < H; y++)
        {
            for (uint16_t x = 0; x < W; x++)
            {
                auto i = index(x, y);
                data[i].reset();
            }
        }
        guard.x = data[startIndex].x;
        guard.y = data[startIndex].y;
        guard.d = U;
    }

private:
    Guard guard;
    unique_ptr<const Position[]> data;
    uint16_t moves {0};

    // helpers for part 2
    uint16_t startIndex;
    vector<uint16_t> passedIndexes;
};

unique_ptr<Floor> read_input(string filename) 
{
    ifstream infile(filename);
    stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto board = unique_ptr<Floor>(new Floor(buffer.str()));
        return std::move(board);
    }   
    else 
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }
}


vector<uint16_t> part1() 
{
    auto floor = read_input("/home/waldo/source/aoc/2024/day6/" DATA);

    bool canGoOn = true;
    do 
    {
#ifdef TEST

        system("clear");
        cout << "Moves: " << floor->getMoves() << endl;
        floor->print();
        //this_thread::sleep_for(chrono::milliseconds(100));
#endif
        canGoOn = floor->moveGuard() == GO_ON;
    }
    while (canGoOn);
    
    // Note: add +1 for the last step moving off the floor
    uint16_t count;
    floor->countDistinct(count);
    cout << "DONE: " << endl;
    cout << " - " << count << " positions passed" << endl;

    // For part 2
    return floor->getPassedPositions();
}

void part2(vector<uint16_t> positions) 
{
    auto floor = read_input("/home/waldo/source/aoc/2024/day6/" DATA);

    // On each of the visited places, set an obstacle and wait if we get stuck
    int obstacles = 0;

    for(const auto& p : positions) 
    {
#ifdef TEST
        system("clear");
#endif
        floor->markAsObstacle(p);

        int result = 0;
        do 
        {
            result = floor->moveGuard();
        }
        while (result == GO_ON);

        if (result == STUCK) 
        {
            obstacles++;
        }
#ifdef TEST
        cout << "Pt2 Moves: " << floor->getMoves() << endl;
        floor->print();
#endif
        floor->reset();
    }


    cout << "Obstacles: " << obstacles << endl;
}

int main(int argc, char* argv[]) 
{
    cout  << "Hallo day 6" << endl;
#ifdef TEST
    cout  << "TEST TEST TEST" << endl;
#endif

    auto positions = part1();
    part2(positions);


    return 0;
}
