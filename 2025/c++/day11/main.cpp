#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <algorithm>
#include <memory>

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>
#include <queue>

// TODO: there are a lot of inputs with only output, these ones could reduce the total list?

using namespace std;

struct Track
{
    string in;
    vector<string> out;
};

/* A BFS to find all possible paths */
uint64_t bfs(const vector<Track> &input)
{
    queue<Track> q;
    // unordered_set<string> visited;
    uint64_t count = 0;

    for (size_t i = 0; i < input.size(); i++)
    {
        const auto &track = input[i];
        if (track.in == "you")
        {
            q.push(track);
            break;
        }
    }

    while (!q.empty())
    {
        auto current = q.front();
        q.pop();

        if (current.in == "out")
        {
            // Ok ready
            count++;
        }

        // Generate next states

        for (const auto &next : current.out)
        {
            if (next == "out")
            {
                count++;
            }
            else
            {
                for (const auto &in : input)
                {
                    if (in.in == next)
                    {
                        q.push(in);
                    }
                }
            }
        }
    }

    return count;
}

void first(vector<Track> input)
{
    uint64_t sum = 0;

    // Reduce the list
    for (size_t i = 0; i < input.size(); i++)
    {
        auto &track = input[i];
        if (track.out.size() == 1)
        {
            string &replace = track.in;
            string &with = track.out[0];

            for (size_t j = 0; j < input.size(); j++)
            {
                if (i == j)
                    continue;

                auto &replacer = input[j];
                for (size_t k = 0; k < replacer.out.size(); k++)
                {
                    if (replacer.out[k] == replace)
                    {
                        replacer.out[k] = with;
                    }
                }
            }
        }
    }

    for (int i = input.size() - 1; i >= 0; i--)
    {
        auto &track = input[i];
        if (track.out.size() == 1)
        {
            input.erase(input.begin() + i);
        }
    }

    sum = bfs(input);

    cout << "Part one: " << sum << endl;
}

/* A DFS to find all paths with cache  */
int dfs(const vector<Track>& input, unordered_map<string, int>& cache, const Track& start, string stop) 
{
    if (cache.contains(start.in)) 
    {
        return cache[start.in];
    }

    int count = 0;
    for (const auto& out : start.out) 
    {
        if (out == stop) 
        {
            return 1;
        }

        const auto it = std::find_if(input.begin(), input.end(), [out](const auto& t){ return t.in == out; });
        if (it != input.end()) 
        {
            count += dfs(input, cache, *it, stop);
        }
    }

    cache[start.in] = count;

    // Went through and no end found. Should not get here
    return count;
}

void second(vector<Track> input)
{
    uint64_t sum = 0;

    auto findTrack = [input](const string& n) -> const Track *
    {
        for (auto& t : input) 
        {
            if (t.in == n) 
            {
                return &t;
            }
        }
        return nullptr;
    };

    unordered_map<string, int> cache;

    auto start = findTrack("svr");
    auto fft = findTrack("fft");
    auto dac= findTrack("dac");

    uint64_t svr_fft = dfs(input, cache, *start, "fft"); cache.clear();
    uint64_t fft_dac = dfs(input, cache, *fft, "dac"); cache.clear();
    uint64_t dac_out = dfs(input, cache, *dac, "out"); cache.clear();
    uint64_t svr_fft_dac_out = svr_fft * fft_dac * dac_out;
    cout << "Path start-fft-dac-out: " << 
        " start-fft:" << to_string(svr_fft) << 
        " * fft-dac:" << to_string(fft_dac) << 
        " * dac-out:" << to_string(dac_out) << 
        " = " << to_string(svr_fft_dac_out) <<  endl; 

    uint64_t svr_dac = dfs(input, cache, *start, "dac"); cache.clear();
    uint64_t dac_fft = dfs(input, cache, *dac, "fft"); cache.clear();
    uint64_t fft_out = dfs(input, cache, *fft, "out"); cache.clear();
    uint64_t svr_dac_fft_out = svr_dac * dac_fft * fft_out;
    cout << "Path start-dac-fft-out: " << 
        " start-dac:" << to_string(svr_dac) << 
        " * dac-fft:" << to_string(dac_fft) << 
        " * fft-out:" << to_string(fft_out) << 
        " = " << to_string(svr_dac_fft_out) <<  endl; 

    sum = svr_fft_dac_out + svr_dac_fft_out;

    cout << "Part two: " << sum << endl;
}

vector<Track> read_input(string filename)
{
    ifstream infile(filename);
    vector<Track> result;

    if (infile.good())
    {
        string line;
        while (getline(infile, line))
        {
            if (line.empty())
                continue;

            Track track;
            track.in = line.substr(0, 3);

            istringstream is(line.substr(4));
            string sub;
            while (getline(is, sub, ' '))
            {
                if (sub.empty())
                    continue;
                track.out.push_back(sub);
            }

            result.push_back(track);
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
    std::string inputFile = "test2.txt";
    if (argc > 1)
    {
        inputFile = std::string(archv[1]);
    }

    std::cout << "Let's get started on " << inputFile << std::endl;

    auto input = read_input(inputFile);
    //first(input);
    second(input);

    cout << endl;

    return 0;
}
