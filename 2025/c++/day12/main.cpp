#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <algorithm>
#include <memory>

#include <unordered_set>
#include <vector>
#include <string>
#include <bitset>
#include <queue>

using namespace std;

using Shape = std::array<bool, 3*3>;


size_t shapeIndex(int x, int y)
{
    return (x + (y * 3));
}

bool shapeIsValid(int x, int y) 
{
    return (x > 0) && (y > 0) && (x < 3) && (y < 3);
}

void shapePrint(const Shape& shape) 
{
    for (int y = 0; y < 3; ++y) 
    {
        for (int x = 0; x < 3; ++x) 
        {
            cout << (shape[shapeIndex(x, y)] ? '#' : '.');
        }
        cout << endl;
    }
    cout << endl;
}


/*
For a 3x3 grid, the indices are:
0 1 2
3 4 5
6 7 8

After a 90-degree clockwise rotation, the new positions are:
6 3 0
7 4 1
8 5 2
*/
Shape shapeRotate(const Shape& shape) 
{
    Shape rotated;
    rotated[0] = shape[6];
    rotated[1] = shape[3];
    rotated[2] = shape[0];
    rotated[3] = shape[7];
    rotated[4] = shape[4];
    rotated[5] = shape[1];
    rotated[6] = shape[8];
    rotated[7] = shape[5];
    rotated[8] = shape[2];
    return rotated;
}

Shape shapeRotateN(const Shape& shape, int times) 
{
    Shape temp = shapeRotate(shape);
    for (int i = 1; i < times; i++) 
    {
        temp = shapeRotate(temp);
    }
    return temp;
}

struct Area 
{
    int width;
    int height;
    vector<int> indices; // 0 0 0 0 2 0 mean only shape index 4 is 2 times

    void print() const
    {
        cout << to_string(width) << "x" << to_string(height) << ": ";
        for (auto i : indices) 
        {
            cout << to_string(i) << " ";
        }
        cout << endl;
    }

    void printArea() const 
    {
        for (int h = 0; h < height; h++) 
        {
            for (int w = 0; w < width; w++) 
            {
                cout << occupied[index(w, h)];
            }
            cout << endl;
        }
    }


    void clear() 
    {
        addedShapes.clear();
        occupied.clear();
        nextChar = 'A';
        
        if (occupied.empty()) 
        {
            occupied = vector<char>(width * height, '.');
        }
    }

    size_t index(int x, int y) const 
    {
        return (x + (y * height));
    }

    bool isValid(int x, int y) 
    {
        return (x > 0) && (y > 0) && (x < width) && (y < height);
    }

    bool addShape(const Shape& shape, int x0, int y0) 
    {
        // -> shape anchor is always left left upper corner (x=0 and y=0)
        // 

        if (occupied.empty()) 
        {
            occupied = vector<char>(width * height, '.');
        }

        bool fitted = true;
        if (x0 + 3 <= width && y0 + 3 <= height) 
        {
            for (int y = 0; fitted && y < 3; y++) 
            {
                for (int x = 0; fitted && x < 3; x++) 
                {
                    if (shape[shapeIndex(x, y)]) 
                    {
                        int ndx = index(x0 + x, y0 + y);
                        if (occupied[ndx] != '.') 
                        {
                            fitted = false;
                        }
                    }
                }
            }
        }
        else 
        {
            fitted = false;
        }

        if (fitted) 
        {
            // Looping the same loop again here because now we know
            // we can occupy the area. Could also copy the current 
            // area and reset when the shape could not be fitted..
            for (int y = 0; y < 3; y++) 
            {
                for (int x = 0; x < 3; x++) 
                {
                    if (shape[shapeIndex(x, y)]) 
                    {
                        int ndx = index(x0 + x, y0 + y);
                        occupied[ndx] = nextChar;
                    }
                }
            }
            nextChar++;
        }

        return fitted;
    }


    vector<Shape> addedShapes;
    vector<char> occupied;
    char nextChar = 'A';

};

class Data 
{
public:

    void print() const 
    {
        int shapeCount = 0;
        for (const auto& shape : shapes) 
        {
            cout << to_string(shapeCount++) << ":" << endl;
            shapePrint(shape);
            cout << endl;
        }

        for (const auto& area: areas) 
        {
            area.print();
        }
    }

    auto& getShapes() 
    {
        return shapes;
    }

    auto& getAreas() 
    {
        return areas;
    }

private:
    vector<Shape> shapes;
    vector<Area> areas;

};


void first(shared_ptr<Data> input)
{
    uint64_t sum = 0;

    auto areas = input->getAreas();
    auto shapes = input->getShapes();

    auto shape_areas = vector<int>();
    for (const auto& shape : shapes) 
    {
        int area = std::count(shape.begin(), shape.end(), true);
        shape_areas.push_back(area);
    }

    for (auto& area : areas) 
    {
        // auto shape = shapes[4];
        // auto s = shapeRotateN(shapes[4], 2);

        // area.addShape(shape, 0, 0);
        // area.addShape(s, 1, 1);
        // area.printArea();
        // cout << endl;
        
        int shapeAreaSum = 0;
        for (int i = 0; i < area.indices.size(); i++) 
        {
            int n = area.indices[i];
            shapeAreaSum += (n * shape_areas[i]); 
        }

        if (shapeAreaSum <= (area.width * area.height)) 
        {
            // Area is big enough to fit all shapes
            sum++;
        }

        //cout << "Area sum " << shapeAreaSum << endl;
    }

    cout << "Part one: " << sum << endl;
}

void second(shared_ptr<Data> input)
{
    uint64_t sum = 0;

    cout << "Part two: " << sum << endl;
}

shared_ptr<Data> read_input(string filename)
{
    ifstream infile(filename);

    if (infile.good())
    {
        shared_ptr<Data> result = make_shared<Data>();

        auto& shapes = result->getShapes();
        auto& areas = result->getAreas();

        string line;
        while (getline(infile, line))
        {
            if (line.empty())
                continue;

            // Quite hardcoded but hey.. it works..

            // Shape
            if (line[1] == ':') 
            {
                // Skip index, this is just the index in the shape vector
                Shape shape;
                for (int y = 0; y < 3; ++y) 
                {
                    // Read 3 lines
                    getline(infile, line);
                    // Read 3 chars
                    for (int x = 0; x < 3; ++x) 
                    {
                        shape[shapeIndex(x, y)] = line[x] == '#';
                    }
                }
                shapes.push_back(shape);
            }
            // Area ex: 4x4: 0 0 0 0 2 0
            else 
            {
                istringstream areaIs(line);
                string part;

                Area area;

                getline(areaIs, part, 'x');
                area.width = stoi(part);

                getline(areaIs, part, ':');
                area.height = stoi(part);

                while (getline(areaIs, part, ' ')) 
                {
                    if (part.empty()) continue;
                    area.indices.push_back(stoi(part));
                }


                areas.push_back(area);
            }
        }
    
        return result;
    }
    else
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }
}

int main(int argc, char *archv[])
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

    cout << endl;

    return 0;
}
