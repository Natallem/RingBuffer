#include <random>
#include <ctime>
#include "random_generator.h"

random_generator::random_generator() {
    random_generator_.seed(time(0));
}

unsigned random_generator::get_int(unsigned max) {
    std::uniform_int_distribution<unsigned> range(0, max);
    return range(random_generator_);
}

int random_generator::get_int_in_range(int min, int max) {
    std::uniform_int_distribution<> range(min, max);
    return range(random_generator_);

}

template<class T>
T random_generator::get_type(T min, T max) {
    std::uniform_int_distribution<T> range(min, max);
    return range(random_generator_);
}

char random_generator::get_byte() {
    std::uniform_int_distribution<> range(0, 7);
    return range(random_generator_);
}

const std::mt19937 &random_generator::getRandomGenerator() const {
    return random_generator_;
}

#include "random_generator.h"
