#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <algorithm>
#include <memory>

#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

struct Box
{
    int x = 0;
    int y = 0;
    int z = 0;

    bool operator==(const Box &other) const
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
};

struct Distance
{
    uint64_t d;
    size_t b1Index;
    size_t b2Index;
};

ostream &operator<<(ostream &out, const Box &b)
{
    return out << "Box{" << to_string(b.x) << "," << to_string(b.y) << "," << to_string(b.z) << "}";
}

uint64_t distance(const Box &b1, const Box &b2)
{
    uint64_t dx = b1.x - b2.x;
    uint64_t dy = b1.y - b2.y;
    uint64_t dz = b1.z - b2.z;
    return (dx * dx) + (dy * dy) + (dz * dz);
}

bool contains(const vector<size_t> &circuit, size_t b1)
{
    for (const auto i : circuit)
    {
        if (i == b1)
        {
            return true;
        }
    }
    return false;
}

bool isConnected(const vector<vector<size_t>> &connections, size_t b1, size_t b2)
{
    for (const auto &circuit : connections)
    {
        if (contains(circuit, b1) && contains(circuit, b2))
        {
            return true;
        }
    }
    return false;
}

bool connect(vector<vector<size_t>> &connections, size_t b1, size_t b2)
{
    int firstIsIn = -1;
    int secondIsIn = -1;
    for (int i = 0; i < connections.size(); ++i)
    {
        auto &circuit = connections[i];
        if (firstIsIn < 0 && contains(circuit, b1))
        {
            firstIsIn = i;
        }
        if (secondIsIn < 0 && contains(circuit, b2))
        {
            secondIsIn = i;
        }
    }

    if (firstIsIn < 0 && secondIsIn < 0)
    {
        // No circuits with any of the boxes, add a new one
        vector<size_t> newCircuit = {b1, b2};
        connections.push_back(newCircuit);
        return true;
    }
    else if (firstIsIn >= 0 && secondIsIn < 0)
    {
        // First is already in a circuit -> add b2
        connections[firstIsIn].push_back(b2);
        return true;
    }
    else if (firstIsIn < 0 && secondIsIn >= 0)
    {
        // Second is already in a circuit -> add b1
        connections[secondIsIn].push_back(b1);
        return true;
    }
    else if (firstIsIn >= 0 && secondIsIn >= 0)
    {
        if (firstIsIn != secondIsIn)
        {
            // Both are already in a circuit -> combine
            for (auto n : connections[secondIsIn])
            {
                connections[firstIsIn].push_back(n);
            }
            connections[secondIsIn].clear();

            // clean up
            for (auto it = connections.end() - 1; it >= connections.begin(); it--)
            {
                if ((*it).empty())
                {
                    connections.erase(it);
                }
            }
            return true;
        }
        else
        {
            // Both are already in the same circuit
            return false;
        }
    }
    else
    {
        // Both are already in the same circuit
        return false;
    }
}

void print(const vector<vector<size_t>> &connections)
{
    for (int i = 0; i < connections.size(); ++i)
    {
        cout << to_string(i) << ": " << to_string(connections[i].size()) << " connections: ";
        for (int j = 0; j < connections[i].size(); ++j)
        {
            cout << to_string(connections[i][j]) << " ";
        }
        cout << endl;
    }
}

void first(vector<Box> input)
{
    uint64_t sum = 0;

    vector<Distance> distances;
    for (int i = 0; i < input.size(); ++i)
    {
        // Do not calculate distance to self, or to 'other way around'
        for (int j = i + 1; j < input.size(); ++j)
        {
            distances.emplace_back(distance(input[i], input[j]), i, j);
        }
    }

    std::sort(distances.begin(), distances.end(),
              [](const Distance &a, const Distance &b)
              {
                  return a.d < b.d;
              });

    // Setup the connections
    vector<vector<size_t>> connections;

    // Run given amount of times
    int count = 0;
    for (int i = 0; i < 1000; i++)
    {
        const auto &dist = distances[i];

        // cout << "Run " << to_string(i) << "->Working on (" << to_string(dist.b1Index) << ")" << input[dist.b1Index] << " and (" << to_string(dist.b2Index) << ")" << input[dist.b2Index] << endl;

        if (connect(connections, dist.b1Index, dist.b2Index))
        {
            count++;
        }

        // system("clear");
        // print(connections);
        // cout << "Count: " << count << endl;
    }

    std::sort(connections.begin(), connections.end(),
          [](const vector<size_t> &a, const vector<size_t> &b)
          {
              return a.size() > b.size();
          });

    sum = 1;
    for (int i = 0; i < 3; i++) 
    {
        sum *= connections[i].size();
    }

    // 13008 is too low => again int overflow :o
    // 131580 is good

    cout << "Part one: " << sum << endl;
}

void second(vector<Box> input)
{
    uint64_t sum = 0;

    vector<Distance> distances;
    for (int i = 0; i < input.size(); ++i)
    {
        // Do not calculate distance to self, or to 'other way around'
        for (int j = i + 1; j < input.size(); ++j)
        {
            distances.emplace_back(distance(input[i], input[j]), i, j);
        }
    }

    std::sort(distances.begin(), distances.end(),
              [](const Distance &a, const Distance &b)
              {
                  return a.d < b.d;
              });

    // Setup the connections
    vector<vector<size_t>> connections;

    // Run given amount of times
    for (int i = 0; ; i++)
    {
        const auto &dist = distances[i];

        connect(connections, dist.b1Index, dist.b2Index);

        if (connections.size() == 1 && connections[0].size() == input.size()) 
        {
            cout << "Count: " << i << endl;

            cout << "Last connection: " << input[dist.b1Index] << " and " << input[dist.b2Index] << endl;

            sum = input[dist.b1Index].x * input[dist.b2Index].x;

            break;
        }
    }

    cout << "Part two: " << sum << endl;
}

vector<Box> read_input(string filename)
{
    ifstream infile(filename);
    vector<Box> result;

    if (infile.good())
    {
        string line;
        while (getline(infile, line))
        {
            if (line.empty())
                continue;

            istringstream is(line);
            string part;
            Box box;
            getline(is, part, ',');
            box.x = stoi(part);
            getline(is, part, ',');
            box.y = stoi(part);
            getline(is, part);
            box.z = stoi(part);
            result.push_back(box);
        }
    }
    else
    {
        cout << "Bad input file " << filename << endl;
    }
    return result;
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
    //first(input);
    second(input);

    return 0;
}
