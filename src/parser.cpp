#include <stdexcept>
#include <iostream>
#include "parser.hpp"
#include "ast.hpp"

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
    if (!match(type)) //throw std::runtime_error("Expected " + error_msg + ", got: " + peek().value);
        std::cout << "!Expected " + error_msg + ", got: " + peek().value << std::endl;
}

void Parser::parse() {
    root = std::make_shared<TranslationUnitNode>(); // Создаем корень AST
    translation_unit(); // Запускаем парсинг
}

std::shared_ptr<ASTNode> Parser::getAST() const {
    return root; // Возвращаем корень AST
}

void Parser::translation_unit() {
    while (!check(TokenType::END)) {
        if (check(TokenType::TYPE_INT) && tokens[pos + 1].type == TokenType::ID && tokens[pos + 1].value == "main") {
            root->statements.push_back(main_function());
        } else {
            root->statements.push_back(declaration());
        }
    }
}

std::shared_ptr<MainFunctionNode> Parser::main_function() {
    advance(); // consume 'int'
    expect(TokenType::ID, "main");
    expect(TokenType::LPAREN, "'(' after main");
    expect(TokenType::RPAREN, "')' after main");
    auto body = std::dynamic_pointer_cast<BlockStmt>(block_statement());
    return std::make_shared<MainFunctionNode>(body);
}

std::shared_ptr<Decl> Parser::declaration() {
    if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_DOUBLE) ||
        check(TokenType::TYPE_CHAR) || check(TokenType::TYPE_BOOL) || check(TokenType::TYPE_VOID)) {
        if (tokens[pos + 1].type == TokenType::ID && tokens[pos + 2].type == TokenType::LPAREN) {
            return func_decl();
        } else {
            return var_decl();
        }
    } else if (match(TokenType::KW_STRUCT)) {
        return struct_decl();
    } else {
        throw std::runtime_error("Unexpected token in declaration: " + peek().value);
    }
}

std::shared_ptr<Decl> Parser::var_decl() {
    auto type = advance().value; // consume type
    auto name = advance().value; // consume variable name
    ExprPtr init = nullptr;
    if (match(TokenType::ASSIGN)) {
        init = expression();
    }
    expect(TokenType::SEMICOLON, "';' after variable declaration");
    return std::make_shared<VarDecl>(type, name, init);
}

std::shared_ptr<FunctionDecl> Parser::func_decl() {
    auto return_type = advance().value; // consume return type
    auto name = advance().value; // consume function name
    expect(TokenType::LPAREN, "'(' after function name");

    std::vector<std::pair<std::string, std::string>> params;
    while (!check(TokenType::RPAREN)) {
        auto param_type = advance().value; // param type
        auto param_name = advance().value; // param name
        params.emplace_back(param_type, param_name);
        if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "',' between parameters");
    }
    expect(TokenType::RPAREN, "')' after parameters");

    if (match(TokenType::SEMICOLON)) {
        return std::make_shared<FunctionDecl>(return_type, name, params, nullptr);
    }
    auto body = std::dynamic_pointer_cast<BlockStmt>(block_statement());
    return std::make_shared<FunctionDecl>(return_type, name, params, body);
}

std::shared_ptr<Stmt> Parser::block_statement() {
    expect(TokenType::LBRACE, "'{' to start block");
    auto block = std::make_shared<BlockStmt>();
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        block->statements.push_back(statement());
    }
    expect(TokenType::RBRACE, "'}' to close block");
    return block;
}

std::shared_ptr<Decl> Parser::struct_decl() {
    auto name = advance().value; // consume struct name
    expect(TokenType::LBRACE, "'{' to start struct body");
    std::vector<VarDecl> fields;
    while (!check(TokenType::RBRACE)) {
        fields.push_back(*std::dynamic_pointer_cast<VarDecl>(var_decl()));
    }
    expect(TokenType::RBRACE, "'}' to end struct body");
    expect(TokenType::SEMICOLON, "';' after struct");
    return std::make_shared<StructDecl>(name, fields);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(TokenType::KW_IF)) return conditional_statement();
    else if (match(TokenType::KW_WHILE) || match(TokenType::KW_FOR) || match(TokenType::KW_DO)) return loop_statement();
    else if (match(TokenType::KW_RETURN)) return return_statement();
    else if (match(TokenType::KW_PRINT) || match(TokenType::KW_READ)) return io_statement();
    else if (match(TokenType::KW_ASSERT)) return assert_statement();
    else if (match(TokenType::KW_EXIT)) return exit_statement();
    else if (match(TokenType::LBRACE)) return block_statement();
    else return expr_statement();
}

std::shared_ptr<Stmt> Parser::expr_statement() {
    auto expr = expression();
    expect(TokenType::SEMICOLON, "semicolon at end of expression");
    return std::make_shared<ExprStmt>(expr);
}

std::shared_ptr<Stmt> Parser::conditional_statement() {
    expect(TokenType::LPAREN, "'(' after if");
    auto condition = expression();
    expect(TokenType::RPAREN, "')' after condition");
    auto then_branch = statement();
    std::shared_ptr<Stmt> else_branch = nullptr;
    if (match(TokenType::KW_ELSE)) else_branch = statement();
    return std::make_shared<IfStmt>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> Parser::loop_statement() {
    if (tokens[pos - 1].type == TokenType::KW_WHILE) {
        expect(TokenType::LPAREN, "'(' after while");
        auto condition = expression();
        expect(TokenType::RPAREN, "')' after while condition");
        auto body = statement();
        return std::make_shared<WhileStmt>(condition, body);
    } else if (tokens[pos - 1].type == TokenType::KW_FOR) {
        expect(TokenType::LPAREN, "'(' after for");
        std::shared_ptr<Decl> init = nullptr;
        if (!check(TokenType::SEMICOLON)) init = var_decl();
        else advance(); // empty init
        auto condition = expression(); expect(TokenType::SEMICOLON, "';' after loop condition");
        auto increment = expression(); expect(TokenType::RPAREN, "')' after loop increment");
        auto body = statement();
        return std::make_shared<ForStmt>(init, condition, increment, body);
    } else if (tokens[pos - 1].type == TokenType::KW_DO) {
        auto body = statement();
        expect(TokenType::KW_WHILE, "while after do");
        expect(TokenType::LPAREN, "'(' after while");
        auto condition = expression();
        expect(TokenType::RPAREN, "')' after condition");
        expect(TokenType::SEMICOLON, "';' after do-while");
        return std::make_shared<WhileStmt>(condition, body); // Исправлено: возвращаем WhileStmt
    }
    throw std::runtime_error("Unexpected token in loop statement: " + peek().value);
}

std::shared_ptr<Stmt> Parser::return_statement() {
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) value = expression();
    expect(TokenType::SEMICOLON, "';' after return");
    return std::make_shared<ReturnStmt>(value);
}

std::shared_ptr<Stmt> Parser::io_statement() {
    auto type = tokens[pos - 1].type;
    expect(TokenType::LPAREN, "'(' after IO");
    ExprPtr expr = nullptr; 
    if (type == TokenType::KW_PRINT) {
        expr = expression();
    } else {
        expect(TokenType::ID, "variable name for read()");
        expr = std::make_shared<VariableExpr>(advance().value);
    }
    expect(TokenType::RPAREN, "')' after IO");
    expect(TokenType::SEMICOLON, "';' after IO");
    if (type == TokenType::KW_PRINT) {
        return std::make_shared<PrintStmt>(expr);
    } else if(type == TokenType::KW_READ){
        return std::make_shared<ReadStmt>(expr);
    }
    throw std::runtime_error("Unexpected token in IO statement: " + peek().value);
}

std::shared_ptr<Stmt> Parser::assert_statement() {
    expect(TokenType::LPAREN, "'(' after assert");
    auto expr = expression();
    expect(TokenType::RPAREN, "')' after assert");
    expect(TokenType::SEMICOLON, "';' after assert");
    return std::make_shared<AssertStmt>(expr);
}

std::shared_ptr<Stmt> Parser::exit_statement() {
    expect(TokenType::LPAREN, "'(' after exit");
    auto expr = expression();
    expect(TokenType::RPAREN, "')' after exit");
    expect(TokenType::SEMICOLON, "';' after exit");
    return std::make_shared<ExitStmt>(expr);
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    auto expr = logical_or();
    if (match(TokenType::ASSIGN)) {
        auto value = assignment(); // правоассоциативность
        return std::make_shared<AssignExpr>(expr, value);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_or() {
    auto expr = logical_and();
    while (match(TokenType::OR)) {
        auto right = logical_and();
        expr = std::make_shared<LogicalExpr>(expr, right, "||");
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_and() {
    auto expr = equality();
    while (match(TokenType::AND)) {
        auto right = equality();
        expr = std::make_shared<LogicalExpr>(expr, right, "&&");
    }
    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match(TokenType::EQ) || match(TokenType::NEQ)) {
        auto op = tokens[pos - 1].value;
        auto right = comparison();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LE) || match(TokenType::GE)) {
        auto op = tokens[pos - 1].value;
        auto right = term();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        auto op = tokens[pos - 1].value;
        auto right = factor();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD)) {
        auto op = tokens[pos - 1].value;
        auto right = unary();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match(TokenType::PLUS) || match(TokenType::MINUS) || match(TokenType::NOT)) {
        auto op = tokens[pos - 1].value;
        auto right = unary(); // рекурсия для префикса
        return std::make_shared<UnaryExpr>(op, right);
    } else {
        return postfix();
    }
}

std::shared_ptr<Expr> Parser::postfix() {
    ExprPtr expr = primary(); 
    if (match(TokenType::INC) || match(TokenType::DEC)) {
        auto op = tokens[pos - 1].value;
        expr = std::make_shared<PostfixExpr>(expr, op);
    } else if (match(TokenType::LBRACKET)) {
        auto index = expression();
        expect(TokenType::RBRACKET, "']' expected after array index");
        expr = std::make_shared<ArrayAccessExpr>(expr, index);
    } else if (match(TokenType::LPAREN)) {
        std::vector<ExprPtr> args; 
        if (!check(TokenType::RPAREN)) {
            do {
                args.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        expect(TokenType::RPAREN, "')' to close function call");
        expr = std::make_shared<CallExpr>(expr, args);
    } else if (match(TokenType::DOT)) {
        expect(TokenType::ID, "member name after '.'");
        auto member = advance().value;
        expr = std::make_shared<MemberAccessExpr>(expr, member);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(TokenType::NUM_INT) || match(TokenType::NUM_FLOAT) ||
        match(TokenType::STRING) || match(TokenType::CHAR) || match(TokenType::BOOL)) {
        return std::make_shared<LiteralExpr>(tokens[pos - 1].value);
    }
    if (match(TokenType::ID)) {
        return std::make_shared<VariableExpr>(tokens[pos - 1].value);
    }
    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        expect(TokenType::RPAREN, "')' after expression");
        return expr;
    }
    throw std::runtime_error("Unexpected token in primary: " + peek().value);
}
