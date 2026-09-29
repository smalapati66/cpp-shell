#pragma once

#include <string>
#include <vector>

// Splits a line on whitespace into tokens. No quoting or escaping yet.
std::vector<std::string> tokenize(const std::string& line);

class Shell {
public:
    // Runs the read-eval-print loop until the user exits.
    void run();

private:
    std::string prompt_ = "$ ";
};
