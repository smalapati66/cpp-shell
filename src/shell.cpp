#include "shell.h"

#include <iostream>
#include <sstream>

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Shell::run() {
    std::string line;
    while (std::cout << prompt_ && std::getline(std::cin, line)) {
        const std::vector<std::string> args = tokenize(line);
        if (args.empty()) {
            continue;
        }
        if (args[0] == "exit") {
            break;
        }
        // TODO: execute commands
    }
}
