#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "parser.hpp"
#include "ast.hpp"
#include "types.hpp"

const std::unordered_map<TokenType, Modifier> token_to_modifier = {
    {TokenType::KW_CONST, Modifier::Const},
    {TokenType::KW_STATIC, Modifier::Static},
    {TokenType::KW_UNSIGNED, Modifier::Unsigned},
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek() const { return tokens[pos]; }

Token Parser::advance() { return pos < tokens.size() ? tokens[pos++] : tokens.back(); }

bool Parser::check(TokenType type) const { return pos < tokens.size() && tokens[pos].type == type; }

bool Parser::token_is_type() const {
    return check(TokenType::TYPE_SHORT) || check(TokenType::TYPE_INT) || check(TokenType::TYPE_LONG) ||
           check(TokenType::TYPE_DOUBLE) || check(TokenType::TYPE_LONG_DOUBLE) || check(TokenType::TYPE_FLOAT) ||
           check(TokenType::TYPE_CHAR) || check(TokenType::TYPE_BOOL) || check(TokenType::TYPE_VOID);
}

bool Parser::token_is_modifier() const {
    return check(TokenType::KW_CONST) || check(TokenType::KW_STATIC) || check(TokenType::KW_UNSIGNED);
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& error_msg) {
    if (!match(type)) //throw std::runtime_error("Expected " + error_msg + ", got: " + peek().value);
        std::cout << "!Expected " + error_msg + ", got: " + peek().value << "(" + tokens[pos-1].value +" "+ peek().value +" " + (pos < tokens.size() ? tokens[pos+1].value : "" )+ ")" << std::endl ;
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
        root->decls.push_back(declaration());
    }
}

std::shared_ptr<Decl> Parser::declaration() {
    if (match(TokenType::KW_STRUCT)) {
        return struct_decl();
    } else if (match(TokenType::KW_TYPEDEF)) {
        return typedef_decl();
    } else if (token_is_type() || token_is_modifier() || check(TokenType::ID)) {
        if (tokens[pos + 2].type == TokenType::LPAREN) { // заменить !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
            return func_decl();
        } else {
            return var_decl();
        }
    } else if (match(TokenType::KW_ASSERT)) return assert_decl();

    throw std::runtime_error("Unexpected token in declaration: " + peek().value);
}

std::shared_ptr<Decl> Parser::typedef_decl() {
    auto mods = parse_modifiers();          // Пропускаем модификаторы
    if (!(token_is_type() || check(TokenType::ID))) {
        std::cout << "Error: type or id expected after typedef" << std::endl;
    }
    auto original_type = advance().value;   // Пропускаем исходный тип
    if (!check(TokenType::ID)) {
        std::cout << "Error: alias(id) expected after typedef" << std::endl;
    }
    auto alias_name = advance().value;      // Пропускаем имя псевдонима

    expect(TokenType::SEMICOLON, "';' after typedef declaration");

    return std::make_shared<TypedefDecl>(mods, make_type(original_type, mods.has(Modifier::Const)), alias_name);
}

Modifiers Parser::parse_modifiers() {
    Modifiers modifiers;

    while (true) {
        auto it = token_to_modifier.find(peek().type);
        if (it != token_to_modifier.end()) {
            modifiers.add(it->second);
            advance(); // Пропускаем токен модификатора
        } else {
            break; 
        }
    }

    return modifiers;
}

std::shared_ptr<Decl> Parser::var_decl() {
    if(match(TokenType::SEMICOLON)) return nullptr; // пустая декларация

    Modifiers modifiers = parse_modifiers();

    if (check(TokenType::TYPE_VOID)) {
        std::cout << "Error: 'void' type cannot be used for variable declaration" << std::endl;
    }
    auto type = make_type(advance().value, modifiers.has(Modifier::Const)); // пропуск type

    std::vector<Variable> variables;

    do {
        auto name = advance().value;
        ExprPtr size = nullptr;
        ExprPtr init = nullptr;
        bool is_array = false;

        if (match(TokenType::LBRACKET)) { // Обработка массива
            is_array = true;
            if (!check(TokenType::RBRACKET)) {
                size = expression(false); // Обработка size
            }
            expect(TokenType::RBRACKET, "']' after array size");
        } 
        if (match(TokenType::ASSIGN) || check(TokenType::LBRACE)) {
            if (check(TokenType::LBRACE)) {
                init = array_initializer();
            } else {
                init = expression(false);
            }
        }
        variables.emplace_back(name, init, size, is_array); // ф-ция создает объект сразу в списке
    } while (match(TokenType::COMMA));

    expect(TokenType::SEMICOLON, "';' after variable declaration");
    return std::make_shared<VarDecl>(type, variables, modifiers);
}

std::shared_ptr<Expr> Parser::array_initializer() {
    expect(TokenType::LBRACE, "'{' to start array initializer");
    std::vector<ExprPtr> elements;
    while (!check(TokenType::RBRACE)) {
        elements.push_back(expression(false));
        if (!check(TokenType::RBRACE)) expect(TokenType::COMMA, "',' between array elements");
    }
    expect(TokenType::RBRACE, "'}' to end array initializer");
    return std::make_shared<ArrayInitExpr>(elements);
}

std::shared_ptr<Decl> Parser::struct_decl() {
    auto name = advance().value; // пропуск struct name
    expect(TokenType::LBRACE, "'{' to start struct body");
    std::vector<VarDecl> fields;
    while (!check(TokenType::RBRACE)) {
        fields.push_back(*std::dynamic_pointer_cast<VarDecl>(var_decl()));
    }
    expect(TokenType::RBRACE, "'}' to end struct body");
    
    // Имя псевдонима
    match(TokenType::ID);
      
    expect(TokenType::SEMICOLON, "';' after struct declaration");
    return std::make_shared<StructDecl>(name, fields);
}

std::shared_ptr<FunctionDecl> Parser::func_decl() {
    auto return_mods = parse_modifiers();
    auto return_type = make_type(advance().value, return_mods.has(Modifier::Const)); // пропуск return type
    auto name = advance().value; // пропуск function name
    expect(TokenType::LPAREN, "'(' after function name");

    std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> params;
    while (!check(TokenType::RPAREN)) {
        auto param_mods = parse_modifiers();
        auto param_type = make_type(advance().value, param_mods.has(Modifier::Const)); // param type
        auto param_name = advance().value; // param name
        params.emplace_back(std::make_pair(param_mods, param_type), param_name);
        if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "',' between parameters");
    }
    expect(TokenType::RPAREN, "')' after parameters");

    if (match(TokenType::SEMICOLON)) {
        return std::make_shared<FunctionDecl>(return_mods, return_type, name, params, nullptr);
    }
    auto body = std::dynamic_pointer_cast<BlockStmt>(block_statement());
    return std::make_shared<FunctionDecl>(return_mods, return_type, name, params, body);
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

std::shared_ptr<Stmt> Parser::statement() {
    if (match(TokenType::KW_IF)) return conditional_statement();
    if (match(TokenType::KW_WHILE)) return while_statement();
    if (match(TokenType::KW_DO)) return do_while_statement();
    if (match(TokenType::KW_FOR)) return for_statement();
    if (match(TokenType::KW_RETURN)) return return_statement();
    if (match(TokenType::KW_BREAK)) return break_statement();
    if (match(TokenType::KW_CONTINUE)) return continue_statement();
    if (match(TokenType::KW_PRINT) || match(TokenType::KW_READ)) return io_statement();
    if (match(TokenType::KW_EXIT)) return exit_statement();
    if (check(TokenType::LBRACE)) return block_statement();
    if ((token_is_type() || check(TokenType::ID)) && tokens[pos + 1].type == TokenType::ID) {
        return std::make_shared<ExprStmt>(var_decl());
    }
    return expr_statement();
}

std::shared_ptr<Stmt> Parser::while_statement() {
    expect(TokenType::LPAREN, "'(' after while");
    auto condition = expression();
    expect(TokenType::RPAREN, "')' after condition");
    auto body = statement();
    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::do_while_statement() {
    auto body = statement();
    expect(TokenType::KW_WHILE, "'while' after do");
    expect(TokenType::LPAREN, "'(' after while");
    auto condition = expression();
    expect(TokenType::RPAREN, "')' after condition");
    expect(TokenType::SEMICOLON, "';' after do-while");
    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::for_statement() {
    expect(TokenType::LPAREN, "'(' after for");
    std::shared_ptr<Decl> init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        if (token_is_type() || check(TokenType::ID) || token_is_modifier()) {
            init = var_decl();
        } else {
            init = std::dynamic_pointer_cast<Decl>(expression());
            expect(TokenType::SEMICOLON, "';' after for initializer");
        }
    }
    auto condition = check(TokenType::SEMICOLON) ? nullptr : expression();
    expect(TokenType::SEMICOLON, "';' after for condition");
    auto increment = check(TokenType::RPAREN) ? nullptr : expression();
    expect(TokenType::RPAREN, "')' after for increment");
    auto body = statement();
    return std::make_shared<ForStmt>(init, condition, increment, body);
}

std::shared_ptr<Stmt> Parser::break_statement() {
    expect(TokenType::SEMICOLON, "';' after break");
    return std::make_shared<BreakStmt>();
}

std::shared_ptr<Stmt> Parser::continue_statement() {
    expect(TokenType::SEMICOLON, "';' after continue");
    return std::make_shared<ContinueStmt>();
}

std::shared_ptr<Stmt> Parser::expr_statement() {
    if (check(TokenType::SEMICOLON)) {
        advance(); // пропуск ';'
        return nullptr;
    }
    auto expr = expression();
    expect(TokenType::SEMICOLON, "';' after expression");
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
    std::vector<ExprPtr> args;
    if (type == TokenType::KW_PRINT) {
        do {
            args.push_back(expression(false));

        } while (match(TokenType::COMMA));
    } else {
        expect(TokenType::ID, "variable name for read()");
        expr = std::make_shared<IdExpr>(advance().value);
    }
    expect(TokenType::RPAREN, "')' after IO");
    expect(TokenType::SEMICOLON, "';' after IO");
    if (type == TokenType::KW_PRINT) {
        return std::make_shared<PrintStmt>(args);
    } else if(type == TokenType::KW_READ){
        return std::make_shared<ReadStmt>(expr);
    }
    throw std::runtime_error("Unexpected token in IO statement: " + peek().value);
}

std::shared_ptr<Decl> Parser::assert_decl() {
    expect(TokenType::LPAREN, "'(' after assert");
    auto expr = expression();
    std::string message = "";
    if (match(TokenType::COMMA)) {
        if(!check(TokenType::STRING)) {
            throw std::runtime_error("Expected string after ',' in assert statement");
        }
        auto message = advance().value;
        }
    expect(TokenType::RPAREN, "')' after assert");  // опц строка 
    expect(TokenType::SEMICOLON, "';' after assert");
    return std::make_shared<AssertDecl>(expr, message);
}

std::shared_ptr<Stmt> Parser::exit_statement() {
    expect(TokenType::LPAREN, "'(' after exit");
    auto expr = expression();
    expect(TokenType::RPAREN, "')' after exit");
    expect(TokenType::SEMICOLON, "';' after exit");
    return std::make_shared<ExitStmt>(expr);
}

std::shared_ptr<Expr> Parser::expression(bool allow_comma) {
    auto expr = assignment();
    if(allow_comma){
        while (match(TokenType::COMMA)) {
            auto right = assignment();
            expr = std::make_shared<BinaryExpr>(",", expr, right);
        }
    }
    return expr;
}

std::shared_ptr<Expr> Parser::assignment() {
    auto expr = ternary_expression();
    if (match(TokenType::ASSIGN) || match(TokenType::PLUS_ASSIGN) || match(TokenType::MINUS_ASSIGN) ||
    match(TokenType::MULT_ASSIGN) || match(TokenType::DIV_ASSIGN) || match(TokenType::MOD_ASSIGN)) {
        auto op = tokens[pos - 1].value;
        auto value = assignment(); // правоассоциативность
        return std::make_shared<AssignExpr>(op, expr, value);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::ternary_expression() {
    auto condition = logical_or();
    if (match(TokenType::QUESTION)) {
        auto true_expr = expression();
        expect(TokenType::COLON, "':' in ternary expression");
        auto false_expr = expression();
        return std::make_shared<TernaryExpr>(condition, true_expr, false_expr);
    }
    return condition;
}

std::shared_ptr<Expr> Parser::logical_or() {
    auto expr = logical_and();
    while (match(TokenType::OR)) {
        auto right = logical_and();
        expr = std::make_shared<LogicalExpr>(tokens[pos - 1].value, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_and() {
    auto expr = equality();
    while (match(TokenType::AND)) {
        auto right = equality();
        expr = std::make_shared<LogicalExpr>(tokens[pos - 1].value, expr, right);
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
        auto operand = unary();
        return std::make_shared<UnaryExpr>(op, operand);
    }else if(match(TokenType::KW_SIZEOF)){
        if(match(TokenType::LPAREN)){
            std::variant<TypePtr, ExprPtr> type;
            if(token_is_type()){
                type = make_type(advance().value);
            }else if(check(TokenType::ID)){
                type = std::make_shared<IdExpr>(advance().value);
            }
            expect(TokenType::RPAREN, "')' after sizeof type");
            return std::make_shared<SizeofExpr>(nullptr, type);
        }else{
            auto operand = expression();
            return std::make_shared<SizeofExpr>(operand, nullptr);
        }

    }
    return postfix();
}

std::shared_ptr<Expr> Parser::postfix() {
    auto expr = primary();
    while (true) {
        if (match(TokenType::INC) || match(TokenType::DEC)) {
            auto op = tokens[pos - 1].value;
            expr = std::make_shared<PostfixExpr>(expr, op);
        } else if (match(TokenType::LBRACKET)) {
            auto index = expression();
            expect(TokenType::RBRACKET, "']' after array index");
            expr = std::make_shared<ArrayAccessExpr>(expr, index);
        } else if (tokens[pos-1].type == TokenType::ID && match(TokenType::LPAREN)) {
            std::vector<ExprPtr> args;
            if (!check(TokenType::RPAREN)) {
                do {
                    args.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            expect(TokenType::RPAREN, "')' after function arguments");
            expr = std::make_shared<CallExpr>(expr, args);
        } else if (match(TokenType::DOT)) {
            auto member = expression();
            if(!std::dynamic_pointer_cast<IdExpr>(member)) {
                throw std::runtime_error("Expected member name after '.' in member access");
            }
            expr = std::make_shared<MemberAccessExpr>(expr, member);
        } else {
            break;
        }
    }
    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(TokenType::NUM_INT)){
        return std::make_shared<LiteralExpr>( std::stoi(tokens[pos - 1].value));
    } else if (match(TokenType::NUM_DOUBLE)){
        return std::make_shared<LiteralExpr>( std::stod(tokens[pos - 1].value));
    } else if (match(TokenType::STRING)){
        return std::make_shared<LiteralExpr>(tokens[pos - 1].value);
    } else if(match(TokenType::CHAR)){
        return std::make_shared<LiteralExpr>(tokens[pos - 1].value[0]);
    } else if(match(TokenType::BOOL)) {
        if (tokens[pos - 1].value == "true") {
            return std::make_shared<LiteralExpr>(true);
        } else if (tokens[pos - 1].value == "false") {
            return std::make_shared<LiteralExpr>(false);
        }
    } else if (match(TokenType::ID)) {
        return std::make_shared<IdExpr>(tokens[pos - 1].value);
    } else if (match(TokenType::LPAREN)) {
        auto expr = expression();
        expect(TokenType::RPAREN, "')' after expression");
        return expr;
    }

    throw std::runtime_error("Unexpected token in primary expression: " + peek().value);
}


