#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

#include <Sifar/Support/string.hpp>

using sifar::ReadArchive;
using sifar::WriteArchive;

using sifar::base;

using namespace sifar::library;

class Human
{
    SERIALIZATION_ACCESS()

protected:
    std::string name_;
    int age_;

public:
    Human(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }

    const std::string& name() const noexcept { return name_; }
    int age() const noexcept { return age_; }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & name_;
        ar & age_;
    }
};

class Boy : public Human
{
private:
    int force_;

public:
    Boy() : Human("unnamed", 0), force_(0)
    {
    }

    Boy(const std::string& name, int age, int force)
        : Human(name, age), force_(force)
    {
    }

    int force() const noexcept { return force_; }

    SERIALIZATION_UNIFIED(ar)
    {
        base<Human>(ar, *this);

        ar & force_;
    }
};

void test_object_serialization()
{
    {
        std::ofstream file("test_object_serialization.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = WriteArchive<std::ofstream>(file);

        Boy obj("Tom", 21, 9);

        std::string hi = "Hello!";
        char bye[] = "Good Bye!";

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Serialization done.\n";
    }
    {
        std::ifstream file("test_object_serialization.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = ReadArchive<std::ifstream>(file);

        Boy obj;

        std::string hi = "Hello!";
        char bye[10];

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Deserialization done.\n";

        std::cout << obj.name() << ' ' << obj.age() << ' ' << obj.force()
                  << ' ' << hi << ' ' << bye << '\n';
    }
}

int main()
{
    test_object_serialization();

    return 0;
}
