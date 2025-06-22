#include "repl/repl.h"
#include <iostream>

int main() {
    std::cout << "Hello! This is the Monkey Programming Language (C++ version)\n";
    std::cout << "Feel free to type in commands.\n";
    repl::start(std::cin, std::cout);
    return 0;
}