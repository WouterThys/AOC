#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>

using namespace std;

struct Position 
{
    int x;
    int y;
    char v; // value (. or @)
    bool m; // Marked

    Position(int x, int y, char v) : x{x},  y{y}, v{v}, m{false}
    {

    }

    bool isRoll() const
    {
        return v == '@';
    }

    void mark() 
    {
        m = true;
    }

    void removeRoll() 
    {
        v = '.';
    }

    Position adjacent(int moveX, int moveY) const
    {
        return Position( x + moveX, y + moveY, v );
    }

};

ostream& operator<<(ostream& out, const Position& p) 
{
    return out << "Position{" << p.v << ":" << to_string(p.x) << "," << to_string(p.y) << "}";
}

class Map 
{
public:
    Map(vector<Position>& positions, int w, int h) : positions{positions}, W {w}, H{h}
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

    Position* get(int x, int y)
    {
        if (isValid(x, y)) 
        {
            return &positions[index(x,y)];
        }
        return nullptr;
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

    bool fewerThan4Adj(Position& pos) 
    {
        int count = 0;
        for (int h = -1; h <= 1; ++h) 
        {
            for (int v = -1; v <= 1; ++v) 
            {
                if (h == 0 && v == 0) continue;

                auto p = get(pos.x + h, pos.y + v);
                if (isValid(p) && p->isRoll()) 
                {
                    count++;
                    if (count > 3) 
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void clean() 
    {
        for (int y = 0; y < H; ++y) 
        {
            for (int x = 0; x < W; ++x) 
            {
                auto p = get(x, y);
                if (p->m) 
                {
                    p->removeRoll();
                }
            }
        }
    }

    vector<Position>& getPositions() 
    {
        return positions;
    }

private:

    int W; /* width */
    int H; /* height */

    vector<Position> positions;

};


void first(shared_ptr<Map> map) 
{
    int count = 0;
    for (auto& p : map->getPositions()) 
    {
        if (p.isRoll() && map->fewerThan4Adj(p)) 
        {
            count++;
        }
    }
    //map->print();
    cout << "Part one: " << count << endl;
}

void second(shared_ptr<Map> map) 
{
    int count = 0;
    int runCount = 1;
    while (runCount > 0) 
    {
        runCount = 0;
        for (auto& p : map->getPositions()) 
        {
            if (p.isRoll() && map->fewerThan4Adj(p)) 
            {
                runCount++;
                p.mark();
            }
        }
        map->clean();
        count += runCount;
        cout << "Removed " << runCount << endl;
    }

    //map->print();
    cout << "Part two: " << count << endl;
}

shared_ptr<Map> read_input(string filename) 
{
    shared_ptr<Map> result = nullptr;
    ifstream infile(filename);
    if (infile.good()) 
    {
        vector<Position> positions;
        int x = 0, maxX = 0;
        int y = 0, maxY = 0;

        string line;
        while (getline(infile, line)) 
        {
            x = 0;
            for (char c : line) 
            {
                positions.push_back(Position(x, y, c));
                x++;
            }
            y++;
        }
        maxX = x;
        maxY = y;
        
        result = make_shared<Map>(positions, maxX, maxY);
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

    auto input = read_input(inputFile);
    first(input);
    second(input);

    return 0;
}


