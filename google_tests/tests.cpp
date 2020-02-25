#include <fstream>
#include "gtest/gtest.h"
#include "generator/random_generator.h"

static random_generator generator = random_generator();

void get_random_input(std::ostream &output, size_t length) {
    for (int i = 0; i < length; i++) {
        output << (uint8_t )generator.get_int(255);;
    }
}

bool check_same_input(std::istream &input1, std::istream &input2) {
    char ch1, ch2;
    while (input1.get(ch1)) {
        if (!input2.get(ch2))
            return false;
        if (ch2 != ch1)
            return false;
    }
    if (input2.get(ch2))
        return false;
    return true;
}


::testing::AssertionResult
test_random_data(long length, std::function<void(std::istream &input, std::ostream &output)> const &encoder,
                 std::function<void(std::istream &input, std::ostream &output)> const &decoder) {
    std::stringstream input;
    get_random_input(input, length);

    std::stringstream encoded_input;
    encoder(input, encoded_input);

    std::stringstream output;
    decoder(encoded_input, output);

    input.seekg(0, std::ios::beg);
    if (check_same_input(input, output)) {
        return ::testing::AssertionSuccess();
    } else {
        std::ofstream failed_input("/failed_input/"+std::to_string(length)+".txt");
        input.seekp(0,std::ios::beg);
        failed_input << input.rdbuf();
        return ::testing::AssertionFailure() << "random line wasn't encoded/decoded correctly";
    }
}



//TEST(random_test_100000, bzip2) {
//    EXPECT_TRUE(test_random_data(100000, bzip2::encode, bzip2::decode));
//}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
