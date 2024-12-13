#include <iostream>
#include <algorithm>
#include <set>

#include <fstream>
#include <sstream>
#include <string>

#include <vector>

using namespace std;

// Answer 1: 479 are safe
// Answer 2: not 511

class Report 
{
public:

    Report(std::string line) 
    {
        istringstream iss(line);
    
        int x;
        while (iss >> x) 
        {
            data.push_back(x);
        }
    }

    bool isValidDiff(int diff) const 
    {
        return (diff > 0 && diff < 4);
    }

    bool validateList(int ignoreIndex = -1) 
    {
        int x; // current element
        int p; // previous element

        bool first = true;
        bool second = true;

        for (int i = 0; i < data.size(); i++)
        {
            if (i == ignoreIndex) 
            {
                continue;
            }

            x = data[i];
            if (first) 
            {
                first = false;

                // First element, nothing to do yet
            }
            else if (second) 
            {
                second = false;

                // Second element, decide on inc or dec
                int diff = x - p;
                increase = diff > 0;
                if (!isValidDiff(abs(diff))) 
                {
                    return false;
                }
            }
            else 
            {
                // Everything else
                int diff = x - p;
                bool inc = diff > 0;
                
                if((inc != increase) || !isValidDiff(abs(diff)))
                {
                    return false;
                }
            }

             p = x;
        }
        return true;
    }

    bool isSafe(int ignore = -1)
    {
        // Just safe
        if (validateList()) 
        {
            message = "Safe";
            return true;
        }
        // Loop indices
        for (int i = 0; i < data.size(); i++)
        {
            // Start validating leaving out one element at a time
            if (validateList(i)) 
            {
                message = "Safe (Leave out level " + to_string(i+1) + ": '" + to_string(data[i]) + "')";
                return true;
            }
        }

        message = "Unsafe";

        return false;
    }

    void print() const 
    {
        cout << message << endl;
    }

private:

    vector<int> data;

    bool increase = false;

    std::string message;
    
};

void read_input(string filename, vector<Report>& result) 
{
    ifstream infile(filename);
    if (infile.good()) 
    {
        std::string line;
        while (std::getline(infile, line)) 
        {
            result.push_back(Report(line));
        }
    }
    else 
    {
        cout << "Invalid input file: " << filename << endl;
    }
    infile.close();
}

int main (int argc, char* archv[]) 
{
    cout << "hallo" << endl;

    vector<Report> reports;
    //read_input("/home/thys/source/aoc/2024/day2/test.txt", reports);
    read_input("/home/thys/source/aoc/2024/day2/data.txt", reports);

    cout << "Got " << reports.size() << " reports" << endl;

    auto count = count_if(reports.begin(), reports.end(), [](Report& i){ return i.isSafe(); });

    cout << count << " are safe" << endl;

    int i = 1;
    for (const auto& r : reports) 
    {
        cout << i << " ";
        r.print();
        i++;
    }

    return 0;
}


