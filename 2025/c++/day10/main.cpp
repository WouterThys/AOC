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

class Machine
{
public:
    Machine(
        size_t length,
        uint16_t result,
        vector<uint16_t> buttons,
        vector<uint16_t> joltages) : length{length},
                                     result{result},
                                     buttons{buttons}
    {
    }

    void printBits(uint16_t value) const
    {
        for (int i = length - 1; i >= 0; i--)
        {
            if (value & (1 << i))
            {
                cout << "1";
            }
            else
            {
                cout << "0";
            }
        }
    }

    void print() const
    {
        // Result
        cout << "R:";
        printBits(result);
        cout << endl;

        // Buttons
        for (const auto &v : buttons)
        {
            cout << "B:";
            printBits(v);
            cout << endl;
        }

        cout << endl;
    }

    uint16_t apply(vector<uint16_t> coefficients) const
    {
        uint16_t result = 0;
        for (size_t i = 0; i < buttons.size(); i++)
        {
            const auto a = coefficients[i];
            const auto b = buttons[i];
            for (size_t m = 0; m < a; m++)
            {
                result ^= b;
            }
        }

        return result;
    }

    size_t getLength() const 
    {
        return length;
    }

    size_t buttonLength() const 
    {
        return buttons.size();
    }

    uint16_t getResult() const 
    {
        return result;
    }

private:
    uint16_t result;
    vector<uint16_t> buttons;
    size_t length;
};


string stateToString(size_t l, const vector<uint16_t>& coef)
{
    string r = "";
    for (size_t i = 0; i < l; i++) 
    {
        r += to_string(coef[i]);
    }
    return r;
}

struct State 
{
    vector<uint16_t> cs;
    string csString;
    uint64_t sum;
};

uint64_t bfs(const Machine& machine)
{
    queue<State> q;
    unordered_set<string> visited;

    State s0;
    for (size_t i = 0; i < machine.buttonLength(); i++) 
    {
        s0.cs.push_back(0);
    }
    s0.csString = stateToString(machine.buttonLength(), s0.cs);
    s0.sum = 0;

    q.push(s0);
    visited.insert(s0.csString);

    while (!q.empty())
    {
        State current = q.front();
        q.pop();

        uint16_t result = machine.apply(current.cs);
        if (result == machine.getResult())
        {
            // cout << "Solution found: a = " << current.a << ", b = " << current.b << ", c = " << current.c << endl;
            // cout << "Minimal sum: " << current.sum << endl;
            return current.sum;
        }

        // Generate next states
        vector<State> nextStates;
        for (size_t i = 0; i < machine.buttonLength(); i++) 
        {
            State nextState = current;
            nextState.cs[i] += 1;
            nextState.sum += 1;
            nextState.csString = stateToString(machine.buttonLength(), nextState.cs);
            nextStates.push_back(nextState);
        }

        for (const auto &next : nextStates)
        {
            if (visited.find(next.csString) == visited.end())
            {
                visited.insert(next.csString);
                q.push(next);
            }
        }
    }

    cout << "No solution found." << endl;
    return 0;
}


void first(vector<Machine> input)
{
    uint64_t sum = 0;

    // vector<uint16_t> coef = { 0, 0, 1, 1, 1 };
    // auto r = input[1].apply(coef);
    // input[1].printBits(r);
    // cout << endl;

    for (size_t i = 0; i < input.size(); i++)
    {
        const auto &m = input[i];
        auto n = bfs(m);
        //cout << "Machine " << to_string(i) << " needs " << to_string(n) << " presses" << endl;
        sum += n;
    }

    // 409 is correct

    cout << "Part one: " << sum << endl;
}

void second(vector<Machine> input)
{
    uint64_t sum = 0;

    cout << "Part two: " << sum << endl;
}

vector<Machine> read_input(string filename)
{
    ifstream infile(filename);
    vector<Machine> result;

    if (infile.good())
    {
        string line;
        while (getline(infile, line))
        {
            if (line.empty())
                continue;

            size_t m_size = 0;
            uint16_t m_result = 0;
            vector<uint16_t> m_buttons;
            vector<uint16_t> m_joltages;

            istringstream is(line);
            string part;
            while (getline(is, part, ' '))
            {
                if (part.empty())
                    continue;

                if (part[0] == '[')
                {
                    // Result
                    m_size = part.length() - 2;
                    for (int i = 1; i < part.length() - 1; ++i)
                    {
                        char c = part[i];
                        m_result |= ((c == '#' ? 1 : 0) << m_size - i);
                    }
                }
                else if (part[0] == '(')
                {
                    // Buttons
                    istringstream bs(part.substr(1, part.length() - 1));
                    string btn;
                    uint16_t btnVal = 0;
                    while (getline(bs, btn, ','))
                    {
                        int btnIdx = stoi(btn);
                        btnVal |= (1 << (m_size - btnIdx - 1));
                    }
                    m_buttons.push_back(btnVal);
                }
                else if (part[0] == '{')
                {
                    // Joltages
                }
            }

            result.emplace_back(m_size, m_result, m_buttons, m_joltages);
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

    // Machine m { 6, 0b100000, { 0b000110 }, {} };
    // auto r = m.apply({1}); // Apply button 1 once
    // m.printBits(r);

    auto input = read_input(inputFile);
    first(input);
    second(input);

    cout << endl;

    return 0;
}
