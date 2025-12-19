#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <numeric>
#include <memory>

#include <vector>
#include <string>
#include <array>
/* 14 x 9
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

struct Rectangle 
{
    uint64_t area;
    // size_t t1;
    // size_t t2;
    Tile t1;
    Tile t2;
};

struct HLine 
{
    int x0;
    int x1;
    int y;
};

struct VLine 
{
    int x;
    int y0;
    int y1;
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

    void draw(const Rectangle& rect) const 
    {
        for (int y = 0; y < 9; y++) 
        {
            for (int x = 0; x < 13; x++) 
            {
                if (rect.t1.x <= x && x <= rect.t2.x 
                      && rect.t1.y <= y && y <= rect.t2.y)      
                {
                    cout << "O";
                }
                else if (isRedTile(x, y)) 
                {
                    cout << "#";
                }
                else if (isInside(x, y))
                {
                    cout << "X";
                }
                else 
                {
                    cout << ".";
                }
            }
            cout << endl;
        }
        cout << endl;
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
        hEdges.clear();
        vEdges.clear();

        const auto size = redTiles.size();
        for (int i = 0; i < size; i++) 
        {
            const auto& curr = redTiles[i];
            const auto& next = redTiles[(i+1) % size];

            if (curr.y == next.y) 
            {
                hEdges.emplace_back(min(curr.x, next.x), max(curr.x, next.x), curr.y);
            }
            else 
            {
                vEdges.emplace_back(curr.x, min(curr.y, next.y), max(curr.y, next.y));
            }
        }
    }

    bool isInside(const Rectangle& rect) const
    {
        return isInside(rect.t1.x, rect.t1.y) &&
            isInside(rect.t2.x, rect.t2.y) &&
            isInside(rect.t1.x, rect.t2.y) &&
            isInside(rect.t2.x, rect.t1.y);
        ;
    }

    bool isInside(int x, int y) const
    {
        // Exactly on an horizontal edge
        for (const auto& hline : hEdges) 
        {
            if (y == hline.y && x >= hline.x0 && x <= hline.x1) 
            {
                return true;
            }
        }


        // Do 'ray casting' in all 4 directions.
        // Crossing edges more than once in one direction means always outside. Draw it...
        // Crossing no edge at all also means outside.
        // Note: expect x0 always to be left of x1 (idem y0 and y1)
        int edgeCrossedCount = 0;
        // Right
        for (const auto& vline : vEdges) 
        {
            // Excactly on vertical edge
            if (x == vline.x && y >= vline.y0 && y <= vline.y1) 
            {
                return true;
            }

            if (vline.x > x // Right of tile
                && vline.y0 < y // Higher
                && vline.y1 >= y // Lower
            ) 
            {
                edgeCrossedCount++;
            }
        }

        return (edgeCrossedCount % 2 == 1);
    }

    bool intersects(const Rectangle& rect) const 
    {
        // Check horizontal edges
        for (const auto& hline : hEdges) 
        {   
            // This line is in between top and bottom of rectangle -> possible intersection
            /* E = horizontal edge. Rectangle t1 is left upper, t2 is right bottom
            ..........
            ....___...
            ...|...|..
            ..E|EEE|E.
            ...|___|..
            ..........
            
            */
            if (hline.y > rect.t1.y && hline.y < rect.t2.y) 
            {
                // Still need to check if hoizontal position is not outside
                if (hline.x0 < rect.t2.x && hline.x1 > rect.t1.x) 
                {
                    // This is an intersection
                    return true;
                }
            }
        }

        // Check vertical edges
        for (const auto& vline : vEdges) 
        {   
            if (vline.x > rect.t1.x && vline.x < rect.t2.x) 
            {
                // Still need to check if hoizontal position is not outside
                if (vline.y0 < rect.t2.y && vline.y1 > rect.t1.y) 
                {
                    // This is an intersection
                    return true;
                }
            }
        }

        // No intersections found
        return false;
    }

    bool isRedTile(int x, int y) const
    {
        for (const auto& t : redTiles) 
        {
            if (t.x == x && t.y == y) 
            {
                return true;
            }
        }
        return false;
    }

private:

    vector<Tile> redTiles;
    vector<HLine> hEdges; // Horizontal edges 
    vector<VLine> vEdges; // Vertical edges

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
            rectangles.emplace_back(ar, t1, t2);
        }
    }

    std::sort(rectangles.begin(), rectangles.end(),
          [](const Rectangle &a, const Rectangle &b)
          {
              return a.area > b.area;
          });

    const auto& largestRect = rectangles[0];
    cout << "Largest rectangle=" << to_string(largestRect.area) << " (" << largestRect.t1 << " and " << largestRect.t2 << ")" << endl;
    cout << "Part one: " << to_string(largestRect.area) << endl;
}

void second(shared_ptr<Floor> map) 
{
    uint64_t result = 0;

    map->createBoundaries();

    const auto& tiles = map->getRedTiles();
    vector<Rectangle> rectangles;

    for (int i = 0; i < tiles.size(); i++) 
    {
        for (int j = i+1; j < tiles.size(); j++) 
        {
            const auto t1 = tiles[i];
            const auto t2 = tiles[j];
            const auto ar = area(t1, t2);
            
            Tile tr1 { min(t1.x, t2.x), min(t1.y, t2.y), 'x' };
            Tile tr2 { max(t1.x, t2.x), max(t1.y, t2.y), 'x' };

            rectangles.emplace_back(ar, tr1, tr2);
        }
    }

    std::sort(rectangles.begin(), rectangles.end(),
          [](const Rectangle &a, const Rectangle &b)
          {
              return a.area > b.area;
          });


    for (const auto& rect : rectangles) 
    {
        // Check if the upper left corner is inside -> if not continue already
        if (!map->isInside(rect.t1.x, rect.t1.y)) 
        {
            continue;
        }

        // Intersection checking -> intersection means not inside
        if (map->intersects(rect)) 
        {
            continue;
        }

        // We are here so found!!
        result = rect.area;
        break;
    }
    
    cout << "Part two: " << result << endl;
}

shared_ptr<Floor> read_input(string filename) 
{
    shared_ptr<Floor> result = nullptr;
    ifstream infile(filename);
    if (infile.good()) 
    {
        result = make_shared<Floor>();
        auto& tiles = result->getRedTiles();

        int max_x = 0;
        int max_y = 0;

        string line;
        string part;
        while (getline(infile, line)) 
        {
            istringstream is(line);

            getline(is, part, ',');
            int x = stoi(part);

            getline(is, part);
            int y = stoi(part);

            if (x > max_x) 
            {
                max_x = x;
            }
            if (y > max_y) 
            {
                max_y = y;
            }

            tiles.emplace_back(x, y, '#');
        }

        cout << "Size is " << to_string(max_x) << "x" << to_string(max_y) << endl;

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


