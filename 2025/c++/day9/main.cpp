#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <numeric>
#include <memory>

#include <vector>
#include <string>
#include <array>
/*
..............
.......#...#..
..............
..#....#.x....
..............
..#......#....
..............
.........#.#..
..............
*/

using namespace std;

struct Tile 
{
    int x;
    int y;
    char v;

    Tile() : x{-1},  y{-1}, v{'.'}
    {

    }

    Tile(int x, int y, char v) : x{x},  y{y}, v{v}
    {

    }

    bool isRed() const 
    {
        return v == '#';
    }

};

ostream& operator<<(ostream& out, const Tile& p) 
{
    return out << "Tile{" << p.v << ":" << to_string(p.x) << "," << to_string(p.y) << "}";
}

struct Line 
{
    Tile t1;
    Tile t2;
};

class Floor 
{
public:
    Floor()
    {

    }

    void print() 
    {
        //cout << "Floor: (" << to_string(W) <<  "," << to_string(H) << ") - " << redTiles.size() << endl;
        // for (int y = 0; y < H; ++y) 
        // {
        //     for (int x = 0; x < W; ++x) 
        //     {
        //         cout << get(x, y)->v;
        //     }
        //     cout << endl;
        // }
    }

    bool isRed(int x, int y) 
    {
        for (const auto& tile : redTiles) 
        {
            if (tile.x == x && tile.y == y) 
            {
                return true;
            }
        }
        return false;
    }

    vector<Tile>& getRedTiles() 
    {
        return redTiles;
    }

    void createBoundaries() 
    {
        const auto size = redTiles.size();
        for (int i = 0; i < size; i++) 
        {
            const auto& curr = redTiles[i];
            const auto& next = redTiles[(i+1) % size];

            edges.emplace_back(
                Tile { min(curr.x, next.x),  min(curr.y, next.y), 'G' }, 
                Tile { max(curr.x, next.x),  max(curr.y, next.y), 'G' });
        }
    }

private:

    vector<Tile> redTiles;
    vector<Line> edges; 

    // GitHub.com/Fadi88/aoc

};

struct Rectangle 
{
    uint64_t area;
    size_t t1;
    size_t t2;
};

uint64_t area(const Tile& t1, const Tile& t2) 
{
    uint64_t dx = abs(t1.x - t2.x) + 1;
    uint64_t dy = abs(t1.y - t2.y) + 1;
    return (dx * dy);
}

void first(shared_ptr<Floor> input) 
{
    const auto& tiles = input->getRedTiles();
    vector<Rectangle> rectangles;

    for (int i = 0; i < tiles.size(); i++) 
    {
        for (int j = i+1; j < tiles.size(); j++) 
        {
            const auto t1 = tiles[i];
            const auto t2 = tiles[j];
            const auto ar = area(t1, t2);
            //cout << "Working on " << t1 << " and " << t2 << " -> " << to_string(ar) << endl;
            rectangles.emplace_back(ar, i, j);
        }
    }

    std::sort(rectangles.begin(), rectangles.end(),
          [](const Rectangle &a, const Rectangle &b)
          {
              return a.area > b.area;
          });

    const auto& largestRect = rectangles[0];
    cout << "Largest rectangle=" << to_string(largestRect.area) << " (" << tiles[largestRect.t1] << " and " << tiles[largestRect.t2] << ")" << endl;
    cout << "Part one: " << to_string(largestRect.area) << endl;
}

void second(shared_ptr<Floor> map) 
{
    uint64_t count = 0;

    
    cout << "Part two: " << count << endl;
}

shared_ptr<Floor> read_input(string filename) 
{
    shared_ptr<Floor> result = nullptr;
    ifstream infile(filename);
    if (infile.good()) 
    {
        result = make_shared<Floor>();
        auto& tiles = result->getRedTiles();

        string line;
        string part;
        while (getline(infile, line)) 
        {
            istringstream is(line);

            getline(is, part, ',');
            int x = stoi(part);

            getline(is, part);
            int y = stoi(part);

            tiles.emplace_back(x, y, '#');
        }
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


