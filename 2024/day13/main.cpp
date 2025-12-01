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
#include <math.h>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 5
#define H W
#else
#define DATA "data.txt"
#define W 140
#define H W
#endif

// Not 26439
// Not 26689
// but 25751

int64_t determinant(uint64_t a, uint64_t b, uint64_t c, uint64_t d) 
{
    return a * d - b * c;
}

class Machine 
{
public:

    Machine(std::string buttonA, std::string buttonB, std::string prize, uint64_t offset) 
    {
        a1 = static_cast<uint64_t>(stoul(buttonA.substr(buttonA.find("X+") + 2)));
        a2 = static_cast<uint64_t>(stoul(buttonA.substr(buttonA.find("Y+") + 2)));
        b1 = static_cast<uint64_t>(stoul(buttonB.substr(buttonB.find("X+") + 2)));
        b2 = static_cast<uint64_t>(stoul(buttonB.substr(buttonB.find("Y+") + 2)));
        c1 = static_cast<uint64_t>(stoul(prize.substr(prize.find("X=") + 2, prize.find(","))));
        c2 = static_cast<uint64_t>(stoul(prize.substr(prize.find("Y=") + 2)));

        c1 += offset;
        c2 += offset;
    }

    void solve(bool allowMoreThan100) 
    {
        static int count = 0;

        count ++;
        x = 0;
        y = 0;

        // Calculate the determinant of the coefficient matrix
        int64_t det = determinant(a1, b1, a2, b2);

        if (det == 0) 
        {
            std::cout << "The system of equations has no unique solution (det = 0)." << std::endl;
            return;
        }

        // Calculate determinants for x and y
        int64_t det_x = determinant(c1, b1, c2, b2);
        int64_t det_y = determinant(a1, c1, a2, c2);

        // Check
        if (det_x % det != 0 || det_y % det != 0) 
        {
            // oops
            return;
        }

        // Solve for x and y
        x = det_x / det;
        y = det_y / det;

        // Not valid
        if (x < 0 || y < 0) 
        {
            x = 0;
            y = 0;
            return;
        }
        // Not higher than 100..
        else if (!allowMoreThan100 && (x > 100 || y > 100)) 
        {
            x = 0;
            y = 0;
            return;
        }

#ifdef TEST
        std::cout << count << " solved!" << std::endl;
        //std::cout << "[" << x << "," << y << "]" << std::endl;

#endif
    }

    uint64_t cost() 
    {
        return (x * 3) + y;
    }

    void print() const 
    {
        // std::cout << a1 << " " << b1 << " " << c1 << std::endl;
        // std::cout << a2 << " " << b2 << " " << c2 << std::endl;
    }

private:
    uint64_t a1, b1;
    uint64_t a2, b2;
    uint64_t c1, c2; // Price

    double x,y;
};

class Game 
{
public:
    Game() 
    {

    }

    void addMachine(std::string buttonA, std::string buttonB, std::string prize, uint64_t offset) 
    {
        Machine m { buttonA, buttonB, prize, offset };
        machines.push_back(m);
    }

    void run(bool allowMoreThan100) 
    {
        uint64_t total = 0;
        for(auto& m : machines) 
        {
            m.solve(allowMoreThan100);
            total += m.cost();
        }

        std::cout << "Total: " << total << std::endl;
    }

    void print() const 
    {
        for(const auto& m : machines) 
        {
            m.print();
            std::cout << std::endl;
        }
    }

private:
    std::vector<Machine> machines;

};

std::unique_ptr<Game> read_input(const std::string filename, uint64_t offset) 
{
    std::ifstream instream(filename);
    std::string buttonA;
    std::string buttonB;
    std::string prize;
    std::string tmp;

    auto game = std::unique_ptr<Game>(new Game());

    if (instream.good()) 
    {
        while (getline(instream, buttonA)) 
        {
            getline(instream, buttonB);
            getline(instream, prize);
            getline(instream, tmp); // empty line

            game->addMachine(buttonA, buttonB, prize, offset);
        }

        return std::move(game);
    }
    else 
    {
        std::cout << "Invalid input " << filename << std::endl;
        return nullptr;
    }
}

void part1() 
{
    auto game = read_input("../day13/" DATA, 0);
#ifdef TEST
    game->print();
#endif

    game->run(false);


}

void part2() 
{
    auto game = read_input("../day13/" DATA, 10000000000000);
#ifdef TEST
    game->print();
#endif
    game->run(true);
}

int main (int argc, char* argv[]) 
{
    std::cout << "hallo day 13" << std::endl;
#ifdef TEST
    std::cout << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();


    return 0;
}


