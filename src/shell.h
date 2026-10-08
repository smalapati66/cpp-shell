#pragma once

#include <string>
#include <vector>

#include "lexer.h"

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
