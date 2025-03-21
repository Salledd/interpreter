#include "parser.hpp"
#include "ast.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek() const { return tokens[pos]; }

Token Parser::advance() { return pos < tokens.size() ? tokens[pos++] : tokens.back(); }

bool Parser::check(TokenType type) const { return pos < tokens.size() && tokens[pos].type == type; }

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& error_msg) {
    if (!match(type)) throw std::runtime_error("Expected " + error_msg + ", got: " + peek().value);
}

void Parser::parse() {
    translation_unit();
}



void Parser::translation_unit() {
    while (!check(TokenType::END)) {
        declaration();
    }
}

void Parser::declaration() {
    if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_DOUBLE) ||
        check(TokenType::TYPE_CHAR) || check(TokenType::TYPE_BOOL) || check(TokenType::TYPE_VOID)) {
        if (tokens[pos + 1].type == TokenType::ID && tokens[pos + 2].type == TokenType::LPAREN) {
            func_decl();
        } else {
            var_decl();
        }
    } else if (match(TokenType::KW_STRUCT)) {
        struct_decl();
    } else {
        statement();
    }
}

void Parser::var_decl() {
    advance(); // consume type
    expect(TokenType::ID, "variable name");
    advance(); // consume variable name
    if (match(TokenType::ASSIGN)) {
        expression();
    }
    expect(TokenType::SEMICOLON, "';' after variable declaration");
}

void Parser::func_decl() {
    advance(); // consume return type
    expect(TokenType::ID, "function name");
    advance(); // consume function name
    expect(TokenType::LPAREN, "'(' after function name");

    while (!check(TokenType::RPAREN)) {
        advance(); // param type
        expect(TokenType::ID, "parameter name");
        advance(); // param name
        if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "',' between parameters");
    }
    expect(TokenType::RPAREN, "')' after parameters");

    if (match(TokenType::SEMICOLON)) return; // function prototype
    block_statement();
}

void Parser::struct_decl() {
    expect(TokenType::ID, "struct name");
    advance(); // consume struct name
    expect(TokenType::LBRACE, "'{' to start struct body");
    while (!check(TokenType::RBRACE)) var_decl();
    expect(TokenType::RBRACE, "'}' to end struct body");
    expect(TokenType::SEMICOLON, "';' after struct");
}





void Parser::statement() {
    if (match(TokenType::KW_IF)) conditional_statement();
    else if (match(TokenType::KW_WHILE) || match(TokenType::KW_FOR) || match(TokenType::KW_DO)) loop_statement();
    else if (match(TokenType::KW_RETURN)) return_statement();
    else if (match(TokenType::KW_PRINT) || match(TokenType::KW_READ)) io_statement();
    else if (match(TokenType::KW_ASSERT)) assert_statement();
    else if (match(TokenType::KW_EXIT)) exit_statement();
    else if (match(TokenType::LBRACE)) block_statement();
    else expr_statement();
}

void Parser::expr_statement() {
    expression();
    expect(TokenType::SEMICOLON, "semicolon at end of expression");
}

void Parser::conditional_statement() {
    expect(TokenType::LPAREN, "'(' after if");
    expression();
    expect(TokenType::RPAREN, "')' after condition");
    statement();
    if (match(TokenType::KW_ELSE)) statement();
}

void Parser::loop_statement() {
    if (tokens[pos - 1].type == TokenType::KW_WHILE) {
        expect(TokenType::LPAREN, "'(' after while");
        expression();
        expect(TokenType::RPAREN, "')' after while condition");
        statement();
    } else if (tokens[pos - 1].type == TokenType::KW_FOR) {
        expect(TokenType::LPAREN, "'(' after for");
        if (!check(TokenType::SEMICOLON)) var_decl();
        else advance(); // empty init
        expression(); expect(TokenType::SEMICOLON, "';' after loop condition");
        expression(); expect(TokenType::RPAREN, "')' after loop increment");
        statement();
    } else if (tokens[pos - 1].type == TokenType::KW_DO) {
        statement();
        expect(TokenType::KW_WHILE, "while after do");
        expect(TokenType::LPAREN, "'(' after while");
        expression();
        expect(TokenType::RPAREN, "')' after condition");
        expect(TokenType::SEMICOLON, "';' after do-while");
    }
}

void Parser::return_statement() {
    if (!check(TokenType::SEMICOLON)) expression();
    expect(TokenType::SEMICOLON, "';' after return");
}

void Parser::io_statement() {
    auto type = tokens[pos - 1].type;
    expect(TokenType::LPAREN, "'(' after IO");
    if (type == TokenType::KW_PRINT) {
        expression();
    } else {
        expect(TokenType::ID, "variable name for read()");
        advance();
    }
    expect(TokenType::RPAREN, "')' after IO");
    expect(TokenType::SEMICOLON, "';' after IO");
}

void Parser::assert_statement() {
    expect(TokenType::LPAREN, "'(' after assert");
    expression();
    expect(TokenType::RPAREN, "')' after assert");
    expect(TokenType::SEMICOLON, "';' after assert");
}

void Parser::exit_statement() {
    expect(TokenType::LPAREN, "'(' after exit");
    expression();
    expect(TokenType::RPAREN, "')' after exit");
    expect(TokenType::SEMICOLON, "';' after exit");
}

void Parser::block_statement() {
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        statement();
    }
    expect(TokenType::RBRACE, "'}' to close block");
}



void Parser::expression() {
    assignment();
}

void Parser::assignment() {
    logical_or();
    if (match(TokenType::ASSIGN)) {
        assignment(); // правоассоциативность
    }
}

void Parser::logical_or() {
    logical_and();
    while (match(TokenType::OR)) {
        logical_and();
    }
}

void Parser::logical_and() {
    equality();
    while (match(TokenType::AND)) {
        equality();
    }
}

void Parser::equality() {
    comparison();
    while (match(TokenType::EQ) || match(TokenType::NEQ)) {
        comparison();
    }
}

void Parser::comparison() {
    term();
    while (match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LE) || match(TokenType::GE)) {
        term();
    }
}

void Parser::term() {
    factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        factor();
    }
}

void Parser::factor() {
    unary();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD)) {
        unary();
    }
}

void Parser::unary() {
    if (match(TokenType::PLUS) || match(TokenType::MINUS) || match(TokenType::NOT)) {
        unary(); // рекурсия для префикса
    } else {
        postfix();
    }
}

void Parser::postfix() {
    primary();
    while (true) {
        if (match(TokenType::INC) || match(TokenType::DEC)) {
            continue;
        } else if (match(TokenType::LBRACKET)) {
            expression();
            expect(TokenType::RBRACKET, "']' expected after array index");
        } else if (match(TokenType::LPAREN)) {
            if (!check(TokenType::RPAREN)) {
                do {
                    expression();
                } while (match(TokenType::COMMA));
            }
            expect(TokenType::RPAREN, "')' to close function call");
        } else if (match(TokenType::DOT)) {
            expect(TokenType::ID, "member name after '.'");
            advance();
        } else {
            break;
        }
    }
}

void Parser::primary() {
    if (match(TokenType::NUM_INT) || match(TokenType::NUM_FLOAT) ||
        match(TokenType::STRING) || match(TokenType::CHAR) || match(TokenType::BOOL)) {
        return;
    }
    if (match(TokenType::ID)) {
        return;
    }
    if (match(TokenType::LPAREN)) {
        expression();
        expect(TokenType::RPAREN, "')' after expression");
        return;
    }
    throw std::runtime_error("Unexpected token in primary: " + peek().value);
}
