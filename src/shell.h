#pragma once

#include <string>
#include <vector>
#include <optional>

// Splits a line on whitespace into tokens. No quoting or escaping yet.
std::optional<std::vector<std::string>> tokenize(const std::string& line);

class Shell {
public:
    void run();

private:
    bool run_builtin(const std::vector<std::string>& args);

    void builtin_cd(const std::vector<std::string>& args);
    void builtin_pwd() const;

    void execute_external(const std::vector<std::string>& args);

    std::string prompt_ = "$ ";
    int last_status_ = 0;
};
