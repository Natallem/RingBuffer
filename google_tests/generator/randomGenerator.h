#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>

class randomGenerator {
public:
    randomGenerator();

    char getByte();

    int getIntInRange(int min, int max);

private:
    std::mt19937 generator;

};

#endif //RANDOM_GENERATOR_H
