#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <unordered_map>
#include <regex>
#include <cctype>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 11
#define H 7
#else
#define DATA "data.txt"
#define W 101
#define H 103
#endif

#define ASCII_TO_CHAR(v) (v-48)
#define CHAR_TO_ASCII(c) (c+48)

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

struct Robot 
{
    uint16_t x;
    uint16_t y;

    const int16_t vx;
    const int16_t vy;

    Robot(uint16_t x, uint16_t y, int16_t vx, int16_t vy)
        : x(x), y(y), vx(vx), vy(vy) {}


    void move() 
    {
        int x_ = x + vx;
        int y_ = y + vy;

        if (x_ < 0) 
        {
            x = W + x_;
        }
        else if (x_ >= W) 
        {
            x = x_ - W;
        }
        else 
        {
            x = x_;
        }

        if (y_ < 0) 
        {
            y = H + y_;
        }
        else if (y_ >= H) 
        {
            y = y_ - H;
        }
        else 
        {
            y = y_;
        }
    }

    void print() const 
    {
        std::cout << "p[" << x << "," << y << "]" << ", v[" << vx << "," << vy << "]" << std::endl;
    }
};

class Floor 
{
public:
    Floor() 
    {

    }

    void addRobot(std::string line) 
    {
        uint16_t x, y;
        int16_t vx, vy;

        // Parse the line
        char ignore; // To skip unwanted characters
        std::stringstream ss(line);
        ss >> ignore >> ignore >> x >> ignore >> y; // Parse position
        ss >> ignore >> ignore >> vx >> ignore >> vy; // Parse velocity

        robots.push_back(Robot(x,y,vx,vy));
    }

    void move(int times)
    {
#ifdef TEST
        print();
#endif
        for (int i = 0; i < times; ++i) 
        {
#ifdef TEST
            system("clear");
#endif

            for (auto& r : robots) 
            {
                r.move();
            }

#ifdef TEST
            print();
#endif
        }
        
    }

    void calculate() 
    {
        // q1
        int q1 = 0;
        for (uint16_t y = 0; y < H / 2; y++)
        {
            for (uint16_t x = 0; x < W / 2; x++)
            {
                q1 += std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
            }
        }

        int q2 = 0;
        for (uint16_t y = 0; y < H / 2; y++)
        {
            for (uint16_t x = (W / 2)+1; x < W; x++)
            {
                q2 += std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
            }
        }

        int q3 = 0;
        for (uint16_t y = (H / 2)+1; y < H; y++)
        {
            for (uint16_t x = 0; x < W / 2; x++)
            {
                q3 += std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
            }
        }

        int q4 = 0;
        for (uint16_t y = (H / 2)+1; y < H; y++)
        {
            for (uint16_t x = (W / 2)+1; x < W; x++)
            {
                q4 += std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
            }
        }

        std::cout << q1 << "*" << q2 <<  "*" << q3 <<  "*" << q4 << "=" << q1 * q2 * q3 * q4 << std::endl;
    }

    bool onlyOnes() 
    {
        for (uint16_t y = 0; y < H; y++)
        {
            for (uint16_t x = 0; x < W; x++)
            {
                auto count = std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
                if (count > 1) 
                {
                    return false;
                }
            }
        }
        return true;
    }

    void print() 
    {
        int total = 0;
        for (uint16_t y = 0; y < H; y++)
        {
            for (uint16_t x = 0; x < W; x++)
            {
                auto count = std::count_if(robots.begin(), robots.end(), [x,y](const Robot& r) { return (r.x == x && r.y == y); });
                if (count > 0) 
                {
                    std::cout << count << "";
                    total += count;
                }
                else 
                {
                    std::cout << ".";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

private:

    std::vector<Robot> robots;

};

std::unique_ptr<Floor> read_input(const std::string filename) 
{
    std::ifstream instream(filename);

    if (instream.good()) 
    {
        auto floor = std::unique_ptr<Floor>(new Floor());
        std::string line;
        while(std::getline(instream, line)) 
        {
            floor->addRobot(line);
        }
        return std::move(floor);
    }
    else 
    {
        std::cout << "Invalid input " << filename << std::endl;
        return nullptr;
    }
}

void part1() 
{
    auto floor = read_input("../day14/" DATA);
    floor->move(100);
    floor->calculate();
}

void part2() 
{
    auto floor = read_input("../day14/" DATA);

    int count = 0;
    bool onlyOnes = false;

    do 
    {
        while (!onlyOnes) 
        {
            system("clear");
            floor->move(1);
            onlyOnes = floor->onlyOnes();
            count++;
        }
        floor->print();
        std::cout << count << std::endl;
        std::cin.get();
        onlyOnes = false;
    } 
    while (true);
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 14" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


