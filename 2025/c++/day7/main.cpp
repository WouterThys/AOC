#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <numeric>
#include <memory>

#include <vector>
#include <string>
#include <array>

using namespace std;

struct Position 
{
    int x;
    int y;
    char v;

    Position() : x{-1},  y{-1}, v{'.'}
    {

    }

    Position(int x, int y, char v) : x{x},  y{y}, v{v}
    {

    }

    void mark() 
    {
        v = '|';
    }

    bool isEmpty() const
    {
        return v == '.';
    }

    bool isSplitter() const
    {
        return v == '^';
    }

};

ostream& operator<<(ostream& out, const Position& p) 
{
    return out << "Position{" << p.v << ":" << to_string(p.x) << "," << to_string(p.y) << "}";
}

// struct Beam 
// {

//     Beam(const Position& pos) : pos { pos } 
//     {

//     }

//     std::pair<Beam, Beam> run(Map& map) 
//     {
//         // TODO: run down and
//         // - just move down -> return yourself once
//         // - reach the end -> die, return empty
//         // - reach a splitter
//         //      - split in one or two, depending on the map

//         auto result = pair<Beam, Beam>(Beam(Position()), Beam(Position()));
//         auto newPos = map.get(pos.x, pos.y + 1);

//         if (newPos != nullptr) 
//         {

//         }
//         return 

//         //return Offspring();
//     }

//     Position pos;
// };

class Map 
{
public:
    Map()
    {

    }

    void print() 
    {
        cout << "Map: (" << to_string(W) <<  "," << to_string(H) << ") - " << positions.size() << endl;
        for (int y = 0; y < H; ++y) 
        {
            for (int x = 0; x < W; ++x) 
            {
                cout << get(x, y)->v;
            }
            cout << endl;
        }
    }

    size_t index(int x, int y) const
    {
        return (x + (y * W));
    }

    bool isValid(const Position* pos) const 
    {
        return pos != nullptr && isValid(pos->x, pos->y);
    }

    bool isValid(int x, int y) const
    {
        return  x >= 0 && y >= 0 &&
                x < W && y < H;
    }

    Position* get(int x, int y)
    {
        if (isValid(x, y)) 
        {
            return &positions[index(x,y)];
        }
        return nullptr;
    }

    vector<Position>& getPositions() 
    {
        return positions;
    }

    void setSize(int w, int h) 
    {
        W = w;
        H = h;
    }

    Position& getStart() 
    {
        return start;
    }

    void setStart(int x, int y) 
    {
        start.x = x;
        start.y = y;
        start.v = 'S';
    }

    int getW() const
    {
        return W;
    }

    int getH() const
    {
        return H;
    }

private:

    int W; /* width */
    int H; /* height */

    Position start;
    vector<Position> positions;

};


void first(shared_ptr<Map> map) 
{
    int count = 0;

    vector<Position> beams;
    vector<Position> newBeams;
    beams.push_back(map->getStart());

    // Could run map->height times, or just until all beams died?
    while (!beams.empty()) 
    {
        newBeams.clear();
        for (auto& beam : beams) 
        {
            auto down = map->get(beam.x, beam.y + 1);
            if (map->isValid(down)) 
            {
                if (down->isEmpty()) 
                {
                    // Add yourself back again.
                    down->mark();
                    newBeams.push_back(*down);
                }
                else if (down->isSplitter()) 
                {
                    auto left = map->get(down->x - 1, down->y);
                    bool splitLeft = false;
                    if (map->isValid(left) && left->isEmpty()) 
                    {
                        left->mark();
                        newBeams.push_back(*left);
                        splitLeft = true;
                    }
                    auto right = map->get(down->x + 1, down->y);
                    bool splitRight = false;
                    if (map->isValid(right) && right->isEmpty()) 
                    {
                        right->mark();
                        newBeams.push_back(*right);
                        splitRight = true;
                    }
                    if (splitLeft || splitRight) 
                    {
                        count++;
                    }
                }
            }
        }

        beams = newBeams;
        //map->print();
    }

    //map->print();
    cout << "Part one: " << count << endl;
}

void second(shared_ptr<Map> map) 
{
    uint64_t count = 0;

    auto beamCounts = vector<uint64_t>(map->getW());
    std::fill(beamCounts.begin(), beamCounts.end(), 0);
    auto start = map->getStart();
    beamCounts[start.x] = 1;

    for (int row = 1; row < map->getH(); ++row) 
    {
        for (int col = 0; col < map->getW(); ++col) 
        {
            auto p = map->get(col, row);
            if (p->isSplitter() && beamCounts[col] > 0) 
            {
                beamCounts[col - 1] += beamCounts[col];
                beamCounts[col + 1] += beamCounts[col];
                beamCounts[col] = 0;
            }
        }
    
        cout << "*" << row << ":\t";
        for (const auto bc : beamCounts) 
        {
            cout << to_string(bc) << " ";
        }
        cout << endl;

        // 531126034 is too low -> use uint64_t
        // 18446744071782656935 is too high -> accumulate does not do what I think...
        // 4404709551015
    
    }

    count = 0;
    for (const auto n : beamCounts) 
    {
        count += n;
    }

    cout << "Part two: " << count << endl;
}

shared_ptr<Map> read_input(string filename) 
{
    shared_ptr<Map> result = nullptr;
    ifstream infile(filename);
    if (infile.good()) 
    {
        result = make_shared<Map>();
        auto& positions = result->getPositions();

        int x = 0;
        int y = 0;

        string line;
        while (getline(infile, line)) 
        {
            x = 0;
            for (char c : line) 
            {
                positions.push_back(Position(x, y, c));

                if (c == 'S') 
                {
                    result->setStart(x, y);
                }

                x++;
            }
            y++;
        }
        result->setSize(x, y);
    }   
    else 
    {
        cout << "Bad input file: " << filename << endl;
        
    }
    return result;
}

int main (int argc, char* archv[]) 
{
    std::string inputFile = "test.txt";
    if (argc > 1) 
    {
        inputFile = std::string(archv[1]);
    }
    
    std::cout << "Let's get started on " << inputFile << std::endl;

    first(read_input(inputFile));
    second(read_input(inputFile));

    return 0;
}


