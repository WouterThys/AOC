#include <iostream>
#include <iomanip>
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

// TEST: ./day_22  0.00s user 0.00s system 68% cpu 0.003 total
// DATA: ./day_22  0.06s user 0.01s system 92% cpu 0.068 total

//#define TEST
//#define CACHE

#ifdef TEST
#define DATA "test.txt"
#define N 2000
#else
#define DATA "data.txt"
#define N 2000
#endif

struct Buyer 
{
    std::array<uint64_t,N> sns {0}; // Secret numbers
    std::array<uint64_t,N> price {0}; // ones digit for
    std::array< int64_t,N> diffs {0}; // ones digit for

    uint64_t highestPrice{0};

    void print() const 
    {
        for (size_t i = 0; i < sns.size(); ++i) 
        {
            std::cout << std::setw(9) << sns[i]  // Right-align to 9 characters
                  << ": " << price[i]       // Print price
                  << " (" << (diffs[i] >= 0 ? "+" : "") << diffs[i] << ")" // Print diff with sign
                  << std::endl;
        }
    }

    uint64_t sellForSequence(int64_t seq[4]) 
    {
        for (size_t i = 4; i < N; i++) 
        {
            if ( diffs[i-3] == seq[0] && diffs[i-2] == seq[1] && diffs[i-1] == seq[2] && diffs[i-0] == seq[3] ) 
            {
                return price[i];
            }
        }
        return 0;
    }

};

class Calculator 
{
public:
    Calculator() 
    {

    }

    void init(std::string input) 
    {
        std::istringstream str(input);
        std::string line;
        while(std::getline(str, line)) 
        {
            if (!line.empty()) 
            {
                auto sn = stoull(line);
                Buyer b;
                b.sns[0] = sn;
                b.price[0] = sn % 10;
                b.diffs[0] = INT64_MAX;
                buyers.push_back(b);   
            }
        }
    }

    uint64_t getSecretNumber(uint64_t input) 
    {
        uint64_t sn = input;
        uint64_t result = 0;
        
        // Multiply with 64
        result = sn << 6;
        result = mix(sn, result);
        sn = prune(result);

        // Divide 32
        result = sn >> 5;
        result = mix(sn, result);
        sn = prune(result);

        // Multiply with 2048
        result = sn << 11;
        result = mix(sn, result);
        sn = prune(result);

        return sn;
    }

    uint64_t calculateSumOfSns() 
    {
        uint64_t total = 0;

        for (auto& buyer : buyers) 
        {
            for (int i = 0; i < N; i++) 
            {
                buyer.sns[i+1] = getSecretNumber(buyer.sns[i]);
            }

#ifdef TEST
            std::cout << buyer.sns[0] << ": " << buyer.sns[N] << std::endl;
#endif

            total += buyer.sns[N];
        }


        return total;
    }

    uint64_t calculateMostBananas() 
    {
        uint64_t total = 0;
        uint64_t sn;

        // Calculate the results
        for (auto& buyer : buyers) 
        {
            for (int i = 1; i < N; i++) 
            {
                sn = getSecretNumber(buyer.sns[i-1]);
                buyer.sns[i] = sn;
                buyer.price[i] = sn%10;
                buyer.diffs[i] = buyer.price[i] - buyer.price[i-1];
            }

#ifdef TEST
            // std::cout << "========" << std::endl;
            // buyer.print();
            // std::cout << std::endl;
            //std::cout << buyer.sns[0] << ": " << buyer.sns[N] << std::endl;
#endif
        }

        // Now do something with it
        int64_t sequence[4] = { -2,1,-1,3 };

        // This was the test sequence and it works
        // for (auto& buyer : buyers) 
        // {
        //     total += buyer.sellForSequence(sequence);
        // } // This will produce 23 in the TEST -> ok
        // std::cout << total << std::endl;

        uint64_t resultForSequence = 0;
        uint16_t seqAsNum = 0;

        for (const auto& buyer : buyers)
        {
            for (int i = 4; i < N; i++) 
            {
                resultForSequence = 0;

                sequence[0] = buyer.diffs[i-3];
                sequence[1] = buyer.diffs[i-2];
                sequence[2] = buyer.diffs[i-1];
                sequence[3] = buyer.diffs[i-0];

                seqAsNum = (sequence[0] * 1000) + (sequence[1] * 100) + (sequence[2] * 10) + (sequence[3] * 1);

                for (auto& buyer : buyers) 
                {
                    resultForSequence += buyer.sellForSequence(sequence);
                }

                if (resultForSequence > total) 
                {
                    total = resultForSequence;
                }
            }

            std::cout << "Highest up until now: " << total << std::endl;

        }

        return total;
    }

private:

    uint64_t mix(uint64_t sn, uint64_t in) const 
    {
        return (sn ^ in);
    }

    uint64_t prune(uint64_t sn) const
    {
        return (sn % 16777216);
    }

private:

    std::vector<Buyer> buyers;

    std::unordered_map<uint16_t, uint64_t> cache;

#ifdef CACHE
    
#endif
};

std::unique_ptr<Calculator> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Calculator>(new Calculator());
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
    auto ptr = read_input("../day22/" DATA);

    uint64_t sn = ptr->calculateSumOfSns();
    std::cout << "Result: " << sn << std::endl;

}

void part2() 
{
    auto ptr = read_input("../day22/" DATA);
    uint64_t bns = ptr->calculateMostBananas();
    std::cout << "Result: " << bns << std::endl;
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 22" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();

    return 0;
}
