#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <regex>

using namespace std;

//#define TEST

#ifdef TEST
#define DATA "test.txt"
#else
#define DATA "data.txt"
#endif

#define RULE_START_LINE     0

#ifdef TEST
#define UPDATE_START_LINE   22
#else
#define UPDATE_START_LINE   1177
#endif

static std::regex RULE_PATTERN = std::regex(R"(^(\d+)\|(\d+)$)");

// First answer:

struct Rule 
{
    // This rule means that A should be before B
    uint16_t A; // 75
    uint16_t B; // 47

    Rule(uint16_t a, uint16_t b) : A{a}, B{b} 
    {

    }

    Rule(string line) 
    {
        std::smatch matches;

        if (std::regex_match(line, matches, RULE_PATTERN)) 
        {
            A = static_cast<uint16_t>(stoi(matches[1].str()));
            B = static_cast<uint16_t>(stoi(matches[2].str()));
        }
    }


    bool includes(const uint16_t v1, const uint16_t v2) const
    {
        /* This rule is about these two numbers */
        return (v1 == A && v2 == B) || (v2 == A && v1 == B);
    }

    bool valid(int i, int j, uint16_t b) const 
    {
        if (b == B) 
        {
            return i < j;
        }
        else 
        {
            // This does not concern us
            return true;
        }
    }
};

class Update
{
public:
    
    Update(string line) 
    {
        istringstream iss(line);
        string str;
        while (getline(iss, str, ',')) 
        {
            pages.push_back(stoi(str));
        }
    }

    bool isValid(const vector<Rule>& rules) const
    {
        uint16_t a = 0;
        uint16_t b = 0;

        for (int i = 0; i < pages.size()-1; i++) 
        {
            a = pages[i];

            for (int j = 0; j < pages.size(); j++) 
            {
                if (i == j) continue;
                b = pages[j];

                // Loop all rules
                for (const auto & rule : rules) 
                {
                    // If this rule is about A,B than, than index i should be > j
                    if (rule.A == a && rule.B == b) 
                    {
                        if (i > j) return false;
                    }

                    // If this rule is about B,A than, than index j should be > i
                    if (rule.A == b && rule.B == a) 
                    {
                        if (j > i) return false;
                    }
                }
            }
        }

        return true;
    }

    uint16_t middle() const
    {
        uint16_t mIndex = pages.size() / 2;
        return pages[mIndex];
    }

    void sortPages(const vector<Rule>& rules) 
    {
        sort(pages.begin(), pages.end(), [&](uint16_t a, uint16_t b) 
        {
            for (int i = 0; i < rules.size(); i++) 
            {
                const auto& r = rules[i];
                if (r.A == a && r.B == b) 
                {
                    return true;
                }
                if (r.B == a && r.A == b) 
                {
                    return false;
                }
            }
            return false;
        });
    }

    void print() const
    {
        for (int i = 0; i < pages.size(); i++) 
        {
            cout << pages[i] << " ";
        }
        cout << endl;
    }

private:
    vector<uint16_t> pages;

};

void read_input(const string filename, vector<Rule>& rules, vector<Update>& updates) 
{
    ifstream instream(filename);
    stringstream buffer;

    if (instream.good()) 
    {
        string line;
        int count = 0;
        while (getline(instream, line)) 
        {
            if (count < UPDATE_START_LINE) 
            {
                if (!line.empty()) 
                {
                    rules.push_back(Rule(line));
                }
            }
            else 
            {
                if (!line.empty()) 
                {
                    updates.push_back(Update(line));
                }
            }

            count++;
        }

    }
    else 
    {
        // return nullptr;
    }
}

void part1() 
{
    vector<Rule> rules;
    vector<Update> updates;

    read_input("/home/thys/source/aoc/2024/day5/" DATA, rules, updates);

    int count = 0;
    for (const auto& upd : updates) 
    {
        if (upd.isValid(rules)) 
        {
            count += upd.middle();   
        }
    }

    cout << "Count: " << count << endl;
}

void part2() 
{
    vector<Rule> rules;
    vector<Update> updates;

    read_input("/home/thys/source/aoc/2024/day5/" DATA, rules, updates);

    int count = 0;
    for (auto& upd : updates) 
    {
        if (!upd.isValid(rules)) 
        {
            upd.sortPages(rules);
            count += upd.middle();   
        }
    }

    cout << "Count: " << count << endl;
}

int main (int argc, char* argv[]) 
{
    cout << "hallo day 5" << endl;
#ifdef TEST
    cout << "TEST TEST TEST" << endl;
#endif

    //part1();
    part2();

    return 0;
}


