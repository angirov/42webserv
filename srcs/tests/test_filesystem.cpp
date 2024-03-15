#include <iostream>
#include <string>
#include "../utils.hpp"



int main() {
    std::string path = "/webserv/conf/ConfigClass.cpp"; // Replace with the directory path you want to check
    if (isValidDirectory(path)) {
        std::cout << "The " << path << " is a valid directory." << std::endl;
    } else {
        std::cout << "The " << path << " is not a valid directory or does not exist." << std::endl;
    }

    if (hasReadPermission(path)) {
        std::cout << "The " << path << " has read permission." << std::endl;
    } else {
        std::cout << "The " << path << " does not have read permission or does not exist." << std::endl;
    }
    return 0;
}