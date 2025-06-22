#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace repl {
    void start(std::istream& in, std::ostream& out);
    void printParserErrors(std::ostream& out, const std::vector<std::string>& errors);
}