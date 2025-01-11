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
#include <unordered_set>
#include <unordered_map>

#include <memory>
#include <regex>

#include <chrono>
#include <thread>
#include <functional>

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#define N 2000
#else
#define DATA "data.txt"
#define N 2000
#endif

struct Hasher {
    std::size_t operator () (const std::vector<std::string>& s) const {
        return (*s.begin())[0];
    }
};

class Router 
{
public:
    Router() 
    {

    }

    void init(std::string input) 
    {
        std::istringstream stream(input);
        std::string line;
        while(std::getline(stream, line)) 
        {
            std::string n1;
            std::string n2;
            std::stringstream ss(line);
            
            std::getline(ss, n1, '-');
            std::getline(ss, n2);

            map[n1].insert(n2);
            map[n2].insert(n1);
        }
    }

    void findParties() 
    {
        std::unordered_set<std::vector<std::string>, Hasher> count;

        for (const auto& [c1, c2s] : map) 
        {
            if (c1[0] != 't') continue;

            for (const auto& c2 : c2s) 
            {
                for (const auto c3 : map[c2]) 
                {
                    if (c1 != c3 && map[c3].contains(c1)) 
                    {
                        std::vector<std::string> temp {c1, c2, c3};
                        std::ranges::sort(temp);
                        if(count.contains(temp)) continue;

                        count.insert(temp);
                    }   
                }
            }
        }

        std::cout << count.size() << std::endl;

    }

    void largestParty() 
    {
        std::set<std::set<std::string>> sets_of_3;
        for (const auto& [c1, c2s] : map) {
            for (const auto& c2 : c2s) {
                for (const auto c3 : map[c2]) {
                    if (c1 != c3 && map[c3].contains(c1)) {
                        sets_of_3.insert({c1, c2, c3});
                    }   
                }            
            }
        }

        bool new_set_size_reached = true;
        std::set<std::set<std::string>> current = sets_of_3;
        while (true) {
            std::set<std::set<std::string>> next;
            for (const auto & s : current) {
                for (const auto& [ele, connections] : map) {
                    if (connections.size() < s.size()) {
                        continue;
                    }
                    bool contains_all = true;
                    for (const auto s_ele : s) {
                        if (!map[ele].contains(s_ele)) {
                            contains_all = false;
                            break;
                        }
                    }
                    if (contains_all) {
                        auto superset = s;
                        superset.insert(ele);
                        next.insert(superset);
                    }
                }
            }
            if (next.empty()) break;
            current = next;
        }
        std::set<std::string> lan_party_s;
        for (const auto& s : current) {
            lan_party_s = s;
        }
        std::vector<std::string> lan_party(lan_party_s.begin(), lan_party_s.end());
        std::ranges::sort(lan_party);
        for (int i = 0; i < lan_party.size() - 1; i++) {
            std::cout << lan_party[i] << ',';
        }
        std::cout << lan_party.back() << '\n';
    }

private:

    std::unordered_map<std::string, std::unordered_set<std::string>> map;

};


std::unique_ptr<Router> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Router>(new Router());
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
    auto ptr = read_input("../day23/" DATA);
    ptr->findParties();
}

void part2() 
{
    auto ptr = read_input("../day23/" DATA);
    ptr->largestParty();
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 23" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();

    return 0;
}
