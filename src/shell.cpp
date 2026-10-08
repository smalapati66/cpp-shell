#include "shell.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <filesystem>
#include <iostream>

void Shell::run() {
    std::string line;
    while (std::cout << prompt_ && std::getline(std::cin, line)) {
        const auto tokens = tokenize(line);
        if (!tokens) {
          std::cerr << "syntax error: unterminated quote\n";
          continue;
        }
        std::vector<std::string> args;
        for (const Token& token : *tokens) {
            args.push_back(token.text);
        }
        if (args.empty()) {
            continue;
        }
        if (args[0] == "exit") {
            break;
        }
        if (run_builtin(args)) {
            continue;
        }
        execute_external(args);
    }
}

void Shell::execute_external(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const std::string& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    const pid_t pid = fork();
    if (pid < 0) {
        std::perror("fork");
        return;
    }

    if (pid == 0) {
        execvp(argv[0], argv.data());
        // Only reached if exec failed. Must _exit so the child never
        // returns into the REPL and reads from the terminal as a second shell.
        std::cerr << args[0] << ": "
                  << (errno == ENOENT ? "command not found" : std::strerror(errno)) << '\n';
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        std::perror("waitpid");
        return;
    }
    if (WIFEXITED(status)) {
        last_status_ = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        last_status_ = 128 + WTERMSIG(status);
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
