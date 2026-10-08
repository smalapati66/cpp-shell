#include "lexer.h"

#include <cctype>

namespace {

bool is_space(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

}  // namespace

std::optional<std::vector<Token>> Lexer::run() {
    std::vector<Token> tokens;
    while (true) {
        skip_whitespace();
        if (at_end()) {
            return tokens;
        }
        Token token;
        if (!lex_word(token)) {
            return std::nullopt;
        }
        tokens.push_back(std::move(token));
    }
}

void Lexer::skip_whitespace() {
    while (!at_end() && is_space(peek())) {
        advance();
    }
}

// A word runs until unquoted whitespace. Quoted and unquoted pieces glue
// together, so a"b c"d is the single word "ab cd".
bool Lexer::lex_word(Token& out) {
    while (!at_end() && !is_space(peek())) {
        const char c = advance();
        if (c == '\'') {
            out.quoted = true;
            if (!lex_single_quote(out.text)) return false;
        } else if (c == '"') {
            out.quoted = true;
            if (!lex_double_quote(out.text)) return false;
        } else if (c == '\\') {
            out.quoted = true;
            lex_escape(out.text);
        } else {
            out.text += c;
        }
    }
    return true;
}

// Called after an unquoted \. The next character is taken literally,
// whatever it is. A trailing \ at end of line is dropped, as bash -c does;
// line continuation can come later with multi-line input.
void Lexer::lex_escape(std::string& out) {
    if (!at_end()) {
        out += advance();
    }
}

// Called after the opening '. Everything is literal until the closing '.
bool Lexer::lex_single_quote(std::string& out) {
    while (!at_end()) {
        const char c = advance();
        if (c == '\'') return true;
        out += c;
    }
    return false;  // unterminated
}

// Called after the opening ". Only \" and \\ are escapes; any other
// backslash is kept literally.
bool Lexer::lex_double_quote(std::string& out) {
    while (!at_end()) {
        const char c = advance();
        if (c == '"') return true;
        if (c == '\\') {
            if (at_end()) return false;
            const char next = advance();
            if (next != '"' && next != '\\') out += '\\';
            out += next;
        } else {
            out += c;
        }
    }
    return false;  // unterminated
}

std::optional<std::vector<Token>> tokenize(std::string_view line) {
    return Lexer(line).run();
}
