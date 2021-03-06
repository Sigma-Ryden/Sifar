#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::ReadArchive;
using sifar::WriteArchive;

using namespace sifar::common; // support of common types
using namespace sifar::tracking; // support of data tracking

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_tracking()
{
    {
        std::ofstream file("test_tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = WriteArchive<std::ofstream>(file);

        int x = 123;
        int* p1 = &x;
        int* p2 = p1;

        println(&x);
        println(p1);
        println(p2);

        println(x);
        println(*p1);
        println(*p2);

        try
        {
            track(ar, x); // track data and read/write with key
            track(ar, p1); // track data if not track and read/write with key
            track(ar, p2); // is the same as above
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }
    }
    {
        std::ifstream file("test_tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = ReadArchive<std::ifstream>(file);

        int x;
        int* p1 = nullptr;
        int* p2 = nullptr; // track function will throw exception if pointer != nullptr

        try
        {
            track(ar, x);
            track(ar, p1);
            track(ar, p2);

        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        println(&x);
        println(p1);
        println(p2);

        println(x);
        println(*p1);
        println(*p2);
    }
}

int main()
{
    test_tracking();

    return 0;
}
