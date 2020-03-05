#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H
#include <random>

class random_generator
{
public:
    random_generator();
    template <class T>
    T get_type(T min, T max);
    char get_byte();
    unsigned get_int(unsigned max);
    int get_int_in_range(int min, int max);

    const std::mt19937 &getRandomGenerator() const;

private:
    std::mt19937 random_generator_;
};

#endif //RANDOM_GENERATOR_H
