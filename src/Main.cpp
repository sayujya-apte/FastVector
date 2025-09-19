#include "FastVector.h"
#include <chrono>
#include <iostream>
#include <vector>

int main()
{
    int N = 10'000'000;

    auto                  t1 = std::chrono::high_resolution_clock::now();
    Apte::FastVector<int> fv;
    for (int i = 0; i < N; i++)
    {
        fv.push_back(i);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto             t3 = std::chrono::high_resolution_clock::now();
    std::vector<int> sv;
    for (int i = 0; i < N; i++)
    {
        sv.push_back(i);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    auto t5 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
    {
        fv[i] += 5;
    }
    auto t6 = std::chrono::high_resolution_clock::now();

    auto t7 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
    {
        sv[i] += 5;
    }
    auto t8 = std::chrono::high_resolution_clock::now();

    std::cout << fv.size() << std::endl;
    std::cout << sv.size() << std::endl;

    std::cout << "Loading 10,000,000 values :- " << std::endl;
    std::cout << "Apte::FastVector time  : " << std::chrono::duration<double>(t2 - t1).count() << "s\n";
    std::cout << "std::vector time       : " << std::chrono::duration<double>(t4 - t3).count() << "s\n";
    std::cout << std::endl;
    std::cout << "Modifying 10,000,000 values :- " << std::endl;
    std::cout << "Apte::FastVector time  : " << std::chrono::duration<double>(t6 - t5).count() << "s\n";
    std::cout << "std::vector time       : " << std::chrono::duration<double>(t8 - t7).count() << "s\n";
}
