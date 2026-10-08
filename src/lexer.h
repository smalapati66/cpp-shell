#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

struct Token {
    std::string text;      // quotes removed, escapes resolved
    bool quoted = false;   // any part of the word was quoted (expansion must skip it later)
};

// Scans a command line into tokens. Each lex_* method consumes input from
// pos_ and appends to a word, so quote handling lives in the call structure
// instead of an enum of states.
class Lexer {
public:
    explicit Lexer(std::string_view src) : src_(src) {}

    // Returns std::nullopt on a syntax error (e.g. unterminated quote).
    std::optional<std::vector<Token>> run();

private:
    bool at_end() const { return pos_ >= src_.size(); }
    char peek() const { return src_[pos_]; }  // caller must check at_end() first
    char advance() { return src_[pos_++]; }

    void skip_whitespace();
    bool lex_word(Token& out);
    bool lex_single_quote(std::string& out);
    bool lex_double_quote(std::string& out);

    std::string_view src_;  // non-owning view; the caller's string must outlive the Lexer
    std::size_t pos_ = 0;
};

std::optional<std::vector<Token>> tokenize(std::string_view line);
