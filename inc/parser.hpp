#pragma once
#include "token.hpp"
#include "ast.hpp" // Для работы с AST
#include <vector>
#include <string>
#include <stdexcept>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    void parse();  // точка входа парсинга - translation_unit()
    std::shared_ptr<ASTNode> getAST() const; // Метод для получения корня AST

private:
    const std::vector<Token>& tokens;
    size_t pos = 0;
    std::shared_ptr<TranslationUnitNode> root; // Корень AST

    // вспомогательные
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    bool token_is_type() const;
    bool token_is_modifier() const;
    void expect(TokenType type, const std::string& error_msg);

    // грамматические правила
    void translation_unit();
    std::shared_ptr<Decl> declaration();
    Modifiers parse_modifiers();
    std::shared_ptr<Decl> var_decl();
    std::shared_ptr<Stmt> expr_statement();
    std::shared_ptr<FunctionDecl> func_decl();
    std::shared_ptr<Decl> struct_decl();
    std::shared_ptr<Decl> assert_decl();
    std::shared_ptr<Decl> typedef_decl();

    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> conditional_statement();
    std::shared_ptr<Stmt> loop_statement();
    std::shared_ptr<Stmt> while_statement();
    std::shared_ptr<Stmt> do_while_statement();
    std::shared_ptr<Stmt> for_statement();
    std::shared_ptr<Stmt> break_statement();
    std::shared_ptr<Stmt> continue_statement();
    std::shared_ptr<Stmt> return_statement();
    std::shared_ptr<Stmt> io_statement();
    std::shared_ptr<Stmt> exit_statement();
    std::shared_ptr<Stmt> block_statement();

    // выражения
    std::shared_ptr<Expr> expression(bool allow_comma = true);
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> ternary();
    std::shared_ptr<Expr> logical_or();
    std::shared_ptr<Expr> logical_and();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> binary();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> postfix();
    std::shared_ptr<Expr> primary();
    std::shared_ptr<Expr> convert_literal();
    std::shared_ptr<Expr> ternary_expression();
    std::shared_ptr<Expr> cast_expression();
    std::shared_ptr<Expr> array_initializer(); // Обработка инициализации массивов
};
