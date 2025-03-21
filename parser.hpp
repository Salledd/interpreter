#pragma once
#include "token.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    void parse();  // точка входа парсинга - translation_unit()

private:
    const std::vector<Token>& tokens;
    size_t pos = 0;

    // вспомогательные
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    void expect(TokenType type, const std::string& error_msg);

    // грамматические правила
    void translation_unit();
    void declaration();
    void var_decl();
    void func_decl();
    void struct_decl();

    void statement();
    void expr_statement();
    void conditional_statement();
    void loop_statement();
    void return_statement();
    void io_statement();
    void assert_statement();
    void exit_statement();
    void block_statement();

    // выражения
    void expression();
    void assignment();
    void ternary();
    void logical_or();
    void logical_and();
    void equality();
    void comparison();
    void term();
    void factor();
    void unary();
    void postfix();
    void primary();
};
    