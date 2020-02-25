#include <random>
#include <ctime>
#include "random_generator.h"

random_generator::random_generator()
{
    random_generator_.seed(time(0));
}

int random_generator::get_int(int max)
{
    std::uniform_int_distribution<> range(0, max);
    return range(random_generator_);
}

#include "random_generator.h"
