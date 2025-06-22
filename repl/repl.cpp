#include "repl.h"

#include <iostream>
#include <string>
#include <memory>
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../evaluator/evaluator.h"
#include "../environment/environment.h"
#include "../object/object.h"

namespace repl {

const std::string PROMPT = "->";

const std::string MONKEY_FACE = R"(
           __,__
 .--.  .-"     "-.  .--.
/ .. \/  .-. .-.  \/ .. \
| |  '|  /   Y   \ |'  | |
| \   \  \ 0 | 0 /  /   / |
 \ '- ,\.-""   ""-./, -' /
  ''-' /_   ^ ^   _\ '-''
      |  \._   _./  |
      \   \ '~' /   /
       '._ '-=-' _.'
          '~---~'
)";

void start(std::istream& in, std::ostream& out) {
    auto env = std::make_shared<object::Environment>();

    std::string line;
    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            break;
        }

        auto l = std::make_shared<lexer::Lexer>(line);
        parser::Parser p(l);

        auto program = p.parseProgram();
        if (!p.errors().empty()) {
            printParserErrors(out, p.errors());
            continue;
        }

        auto evaluated = evaluator::eval(program, env);
        if (evaluated != nullptr) {
            out << evaluated->inspect() << std::endl;
        }
    }
}

void printParserErrors(std::ostream& out, const std::vector<std::string>& errors) {
    out << MONKEY_FACE;
    out << "Woops! We ran into some monkey business here!\n";
    out << " parser errors:\n";
    for (const auto& msg : errors) {
        out << "\t" << msg << "\n";
    }
}

}