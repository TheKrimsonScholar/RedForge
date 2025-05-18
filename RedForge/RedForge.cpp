#include <iostream>
#include <chrono>

#include "GraphicsSystem.h"

struct K
{
    int k1;
    int k2;

    void SetK1(int value)
    {
        k1 = value;
    }

    inline void KSetK1(int value)
    {
        k1 = value;
    }

    static inline void KSetK1(K& k, int value)
    {
        k.k1 = value;
    }
};

inline void KSetK1(K& k, int value)
{
    k.k1 = value;
}

struct L
{
    int k1;
    int k2;
};

int main()
{
    //K* array = new K[100000000]{};

    ////std::cout << "Start" << std::endl;

    //auto timeStart = std::chrono::high_resolution_clock::now();

    //for(int i = 0; i < 100000000; i++)
    //{
    //    //array[i].k1 = 1;
    //    //array[i].SetK1(1);
    //    //KSetK1(array[i], 1);
    //    //array[i].KSetK1(1);
    //    K::KSetK1(array[i], 1);
    //}

    //auto timeEnd = std::chrono::high_resolution_clock::now();

    //std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

    ////std::cout << "End" << std::endl;

    //char input;
    //std::cin >> input;

    //delete[] array;

    //return 0;

    /*std::vector<L> k;
    k.push_back({ 1, 2 });
    k.push_back({ 2, 3 });
    k.push_back({ 3, 4 });

    for(L& i : k)
        std::cout << i.k1 << " " << i.k2 << std::endl;

    std::cout << "-----" << std::endl;

    auto it = k.begin() + 1;
    k.erase(it);

    for(L& i : k)
        std::cout << i.k1 << " " << i.k2 << std::endl;

    return 0;*/

    GraphicsSystem graphics;

    graphics.Run();
}