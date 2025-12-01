#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 15
#define H W
#else
#define DATA "data.txt"
#define W 141
#define H W
#endif

static uint32_t index(uint16_t x, uint16_t y) 
{
    return (x + (y * W));
}

static bool valid(uint16_t x, uint16_t y) 
{
    return (x < W && y < H);
}

enum Direction
{
    L, U, R, D
};

/* Movements for all directions, use combination like
    for (i..4) {
     x = rowNum[i];
     y = colNum[i];
    }
 */
constexpr int rowNum[] = {-1,  0,  1,  0};
constexpr int colNum[] = { 0, -1,  0,  1};
                         //L   U   R   D

struct Position
{
    uint16_t x;
    uint16_t y;
    char k;
};


class IKeypad 
{
public:

    char currentPos() const 
    {
        return keys[pos].k;
    }

    std::string moveTo(const char to) 
    {
        std::string result;
        if (child) 
        {
            result += child->moveTo(to);
        }
        else 
        {
            result = to;
        }

        std::string outerResult;
        const char pc = keys[pos].k;

        for (const char c : result) 
        {
            const auto& src = keys[pos];
            const auto& dst = getPos(c);

            pos = (dst.x + (dst.y * w));
            outerResult += move(src, dst);
            outerResult += "A";
        }

        return outerResult;
    }

protected:

    std::string move(const Position& src, const Position& dst) const
    {
        std::string route;

        // Than move the x direction
        if (dst.x > src.x) route += std::string(dst.x-src.x,'>');
        else if (dst.x < src.x) route += std::string(src.x-dst.x,'<');

        // First move in y direction
        if (dst.y > src.y) route += std::string(dst.y-src.y,'v');
        else if (dst.y < src.y) route += std::string(src.y-dst.y,'^');

        return route;
    }

    const Position& getPos(const char c) const
    {
        for (const auto& p : keys) 
        {
            if (p.k == c) return p;
        }
        std::cout << "ERROR invalid key " << c << std::endl;
        return keys[0];
    }

    std::string name; // Just to help debugging
    uint16_t pos;
    uint16_t w;
    std::vector<Position> keys;

    IKeypad* child;
};


class NumericKeypad : public IKeypad
{
public:

    NumericKeypad(std::string name, IKeypad* child) 
    {
        this->name = name;
        this->child = child;

        keys = 
        {
            Position{0,0,'7'}, Position{1,0,'8'}, Position{2,0,'9'},
            Position{0,1,'4'}, Position{1,1,'5'}, Position{2,1,'6'},
            Position{0,2,'1'}, Position{1,2,'2'}, Position{2,2,'3'},
            Position{0,3,' '}, Position{1,3,'0'}, Position{2,3,'A'},
        };

        pos = 11;
        w = 3;
    }

    void reset() 
    {
        pos = 11;
    }

};

class DirectionalKeypad : public IKeypad
{
public:

    DirectionalKeypad(std::string name, IKeypad* child) 
    {
        this->name = name;
        this->child = child;

        keys = 
        {
            Position{0,0,' '}, Position{1,0,'^'}, Position{2,0,'A'},
            Position{0,1,'<'}, Position{1,1,'v'}, Position{2,1,'>'},
        };

        pos = 2;
        w = 3;
    }

    void reset() 
    {
        pos = 2;
    }

};

class CodeCracker 
{
public:
    CodeCracker() 
    {
        door =   std::make_unique<NumericKeypad>("door", nullptr);
        robot1 = std::make_unique<DirectionalKeypad>("robot 1", door.get());
        robot2 = std::make_unique<DirectionalKeypad>("robot 2", robot1.get());
    }

    void init(std::string input) 
    {
        std::istringstream strstr(input);
        std::string line;
        while(std::getline(strstr, line)) 
        {
            if (!line.empty()) 
            {
                codes.push_back(line);
            }
        }
    }

    void solve() 
    {
        uint32_t total = 0;
        for(const auto& code : codes) 
        {
            // 184340
            door->reset();
            robot1->reset();
            robot2->reset();

            std::string result;
            for (const char c : code) 
            {
                result += robot2->moveTo(c);
            }

            std::cout << code << ": " << result.size() << " - " << result << std::endl;

            std::istringstream str(code);
            std::string strNum;
            std::getline(str, strNum, 'A');
            auto num = stoi(strNum);
            total += ( num * result.size() );


        }
        std::cout << "total: " << total << std::endl;
    }

private:

    std::vector<std::string> codes;
    std::unique_ptr<NumericKeypad> door;
    std::unique_ptr<DirectionalKeypad> robot1;
    std::unique_ptr<DirectionalKeypad> robot2;
};


std::unique_ptr<CodeCracker> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<CodeCracker>(new CodeCracker());
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
    auto ptr = read_input("../day21/" DATA);
    ptr->solve();

    // auto door =   std::make_unique<NumericKeypad>("door", nullptr);
    // auto robot1 = std::make_unique<DirectionalKeypad>("robot 1", door.get());
    // auto robot2 = std::make_unique<DirectionalKeypad>("robot 2", robot1.get());

    // std::cout << "Move from " << robot2->currentPos() << " to 0: " << robot2->moveTo('0') << std::endl;


}

void part2() 
{
    auto ptr = read_input("../day21/" DATA);

}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 21" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    part1();
    //part2();

    return 0;
}
