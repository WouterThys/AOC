#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <array>
#include <set>
#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 5
#define H 7
#else
#define DATA "data.txt"
#define W 5
#define H 7
#endif

#define FIT (H-2)

static uint16_t index(uint16_t x, uint16_t y) 
{
    return (x + (y * W));
}

struct Element 
{
    void init(std::string& input, int init) 
    {
        int read_pos = W+1;
        std::fill_n(data.begin(), W, init);
        for (uint16_t y = 1; y < H; y++) 
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

                if (v == '#') 
                {
                    data[x]++;
                }
                
                read_pos++;
            }
        }
    }

    void print() const 
    {
        for (auto v : data) 
        {
            std::cout << v << ",";
        }
    }

    std::array<int, W> data {0};
};

struct Lock : public Element 
{

};

struct Key : public Element 
{

    bool matches(const Lock& l) const 
    {
        for (int i = 0; i < data.size(); i++) 
        {
            if (l.data[i] + data[i] > FIT) 
            {
                return false;
            }
        }

        return true;
    }

};

class Container 
{
public:
    Container() { }

    void init(std::string input) 
    {
        std::istringstream  instream(input);
        std::string line;
        
        size_t start = 0;
        size_t blockLength = ((W+1) * H) + 1; // +1+1 for \n
        size_t blocks = (input.length() + 1) / blockLength;
        
        for (size_t i = 0; i < blocks; i++) 
        {
            std::string block = input.substr(start, blockLength);
            if (block[0] == '#') 
            {
                Lock lock;
                lock.init(block, 0);
                locks.push_back(lock);
            }
            else 
            {
                Key key;
                key.init(block, -1);
                keys.push_back(key);
            }

            start += blockLength;
        }
        
        std::cout << locks.size() << " locks, " << keys.size() << " keys" << std::endl;

#ifdef TEST

        std::cout << "Locks: " << std::endl;
        for(const auto& l : locks) 
        {
            l.print();
            std::cout << std::endl;
        }

        std::cout << "Keys: " << std::endl;
        for(const auto& k : keys) 
        {
            k.print();
            std::cout << std::endl;
        }

#endif

    }

    void solve() 
    {
        // Naive??
        int count = 0;

        for (const auto& l : locks) 
        {
            for (const auto& k : keys) 
            {
                if (k.matches(l)) 
                {
                    count++;
#ifdef TEST
                    std::cout << "Lock ";
                    l.print();
                    std::cout << " and key ";
                    l.print();
                    std::cout << " FIT" << std::endl;
#endif
                }
#ifdef TEST
                else 
                {
                    std::cout << "Lock ";
                    l.print();
                    std::cout << " and key ";
                    l.print();
                    std::cout << " OVERLAP" << std::endl;
                }
#endif
            }
        }


        std::cout << "There are " << count << " matches" << std::endl;
    }

    void print() const 
    {

    }


private:

    std::vector<Lock> locks;
    std::vector<Key> keys;

};

std::unique_ptr<Container> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Container>(new Container());
        ptr->init(buffer.str());
        return std::move(ptr);
    }   
    else 
    {
        std::cout << "Bad input file " << filename << std::endl;
        return nullptr;
    }
}


void part1() 
{
    auto floor = read_input("../day25/" DATA);
    floor->solve();
#ifdef TEST
    floor->print();
#endif
    // auto count = floor->count();
    // std::cout  << "Result: " << count << std::endl;
}

void part2() 
{
    auto floor = read_input("../day25/" DATA);
    floor->solve();

#ifdef TEST
    floor->print();
#endif    
    // auto count = floor->count();
    // cout  << "Result: " << count << endl;
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 25" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
