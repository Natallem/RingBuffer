#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H
#include <random>

class random_generator
{
public:
    random_generator();
    int get_int(int max);

private:
    std::mt19937 random_generator_;
};

#endif //RANDOM_GENERATOR_H
