#include <iostream>
#include <algorithm>
#include <set>

#include "data.hpp"

void sum() 
{
    std::cout << "pandas are summing!!" << std::endl;

    auto l1_len = sizeof(list1) / sizeof(int);
    auto l2_len = sizeof(list2) / sizeof(int);

    if (l1_len != l2_len) 
    {
        std::cout << "Invalid array lengths" << std::endl;
        return;
    }

    std::sort(std::begin(list1), std::end(list1));
    std::sort(std::begin(list2), std::end(list2));
    std::cout << "sorted!!" << std::endl;

    int sum = 0;

    for (int i = 0; i < l1_len; ++i) 
    {
        sum += abs( list1[i] - list2[i] );
    }

    std::cout << "sum: " << sum << std::endl;
}

void sim() 
{
    std::sort(std::begin(list1), std::end(list1));
    std::sort(std::begin(list2), std::end(list2));
    std::cout << "sorted!!" << std::endl;

    // unique values in list 1
    std::set<int> s(std::begin(list1), std::end(list1));

    int sim = 0;
    for (auto v : s) 
    {
        int n = std::count(std::begin(list2), std::end(list2), v);
        sim += (v * n);
    }

     std::cout << "sim: " << sim << std::endl;

}

int main (int argc, char* archv[]) 
{
    //sum();
    sim();

    return 0;
}


