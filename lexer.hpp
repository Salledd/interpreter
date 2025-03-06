#pragma once
#include "token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string&);
    std::vector<Token> tokenize();

private:
    std::string input;
    std::size_t index = 0;

    static const std::unordered_map<std::string, TokenType> keywords;
    static const std::unordered_map<std::string, TokenType> operators;

    Token extract();
    Token extract_number();
    Token extract_identifier();
    Token extract_string();
    Token extract_char();
    Token extract_operator();
    void extract_comment();
};
