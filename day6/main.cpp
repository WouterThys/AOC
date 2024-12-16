#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>
#include <regex>

using namespace std;

#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 130
#define H W
#else
#define DATA "data.txt"
#define W 130
#define H W
#endif

// First answer:

void read_input(const string filename) 
{
    ifstream instream(filename);
    stringstream buffer;

    if (instream.good()) 
    {
        
    }
    else 
    {
        // return nullptr;
    }
}

void part1() 
{
    system("clear");
    cout << "Hello again" << endl;
}

void part2() 
{
    
}

int main (int argc, char* argv[]) 
{
    cout << "hallo day 6" << endl;
#ifdef TEST
    cout << "TEST TEST TEST" << endl;
#endif

    part1();
    //part2();


    return 0;
}


