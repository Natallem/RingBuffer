#include <random>
#include <ctime>
#include "randomGenerator.h"

randomGenerator::randomGenerator() {
    generator.seed(time(0));
}

int randomGenerator::getIntInRange(int min, int max) {
    std::uniform_int_distribution<> range(min, max);
    return range(generator);
}

char randomGenerator::getByte() {
    std::uniform_int_distribution<> range(0, 7);
    return range(generator);
}


