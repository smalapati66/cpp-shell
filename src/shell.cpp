#include "shell.h"

#include <cstdlib>
#include <filesystem>
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
        if (run_builtin(args)) {
            continue;
        }
        // TODO: run external commands (fork/execvp)
    }
}

bool Shell::run_builtin(const std::vector<std::string>& args) {
    if (args[0] == "cd") {
        builtin_cd(args);
        return true;
    }
    if (args[0] == "pwd") {
        builtin_pwd();
        return true;
    }
    return false;
}

void Shell::builtin_cd(const std::vector<std::string>& args) {
    if (args.size() > 2) {
        std::cerr << "cd: too many arguments\n";
        return;
    }

    std::filesystem::path target;
    if (args.size() == 2) {
        target = args[1];
    } else if (const char* home = std::getenv("HOME")) {
        target = home;
    } else {
        std::cerr << "cd: HOME not set\n";
        return;
    }

    // filesystem functions throw std::filesystem::filesystem_error on failure
    // (missing directory, not a directory, permission denied, ...).
    try {
        std::filesystem::current_path(target);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "cd: " << target.string() << ": " << e.code().message() << '\n';
    }
}

void Shell::builtin_pwd() const {
    std::cout << std::filesystem::current_path().string() << '\n';
}
