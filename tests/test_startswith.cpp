#include "../utils.hpp"
#include <string>
#include <iostream>

int main() {
    // Test cases
    std::cout << std::boolalpha; // to output boolean values as true/false

    std::cout << "Test 0  : " << url_match_root("/", "/") << std::endl;

    std::cout << "Test 0a : " << url_match_root("/asdfadsf", "/") << std::endl;

    std::cout << "Test 1  : " << url_match_root("/he/", "/hE") << std::endl;

    std::cout << "Test 1a : " << url_match_root("/He/dsaf", "/he") << std::endl;

    std::cout << "Test 1b : " << url_match_root("/he/dsaf", "/he/") << std::endl;

    std::cout << "Test 1c : " << url_match_root("/He", "/hE/") << std::endl;

    std::cout << "Test 1d : " << url_match_root("/wOr", "/woR") << std::endl;

    std::cout << "Test 2  : " << url_match_root("/world", "/wor") << std::endl;

    std::cout << "Test 2a : " << url_match_root("openai", "ai") << std::endl;

    std::cout << "Test 2b : " << url_match_root("start", "starts") << std::endl;

    return 0;
}