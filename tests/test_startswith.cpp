#include "../utils.hpp"
#include <string>
#include <iostream>

int main() {
    // Test cases
    std::cout << std::boolalpha; // to output boolean values as true/false

    // Test 1: Check if "hello" starts with "he"
    std::cout << "Test 1: " << url_match_root("/he/", "/hE") << std::endl;

    // Test 1: Check if "hello" starts with "he"
    std::cout << "Test 1a: " << url_match_root("/He/dsaf", "/he") << std::endl;

    // Test 1: Check if "hello" starts with "he"
    std::cout << "Test 1b: " << url_match_root("/he/dsaf", "/he/") << std::endl;

    // Test 1: Check if "hello" starts with "he"
    std::cout << "Test 1c: " << url_match_root("/He", "/hE/") << std::endl;

    // Test 2: Check if "world" starts with "wor"
    std::cout << "Test 1d: " << url_match_root("/wOr", "/woR") << std::endl;

    // Test 2: Check if "world" starts with "wor"
    std::cout << "Test 2: " << url_match_root("/world", "/wor") << std::endl;

// Test 3: Check if "openai" starts with "ai"
    std::cout << "Test 2a: " << url_match_root("openai", "ai") << std::endl;

    // Test 4: Check if "start" starts with "starts"
    std::cout << "Test 2b: " << url_match_root("start", "starts") << std::endl;

    return 0;
}