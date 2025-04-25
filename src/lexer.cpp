#include "lexer.hpp"
#include <stdexcept>
#include <cctype>

Lexer::Lexer(const std::string& input) : input(input) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (index < input.size()) {
        Token token = extract();
        tokens.push_back(token);
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

Token Lexer::extract() {
    while (index < input.size() && std::isspace(input[index])) ++index;
    if (index >= input.size()) return {TokenType::END, ""};
    if (input[index] == '/' && (input[index + 1] == '/' || input[index + 1] == '*')){
        extract_comment();
        return extract();
    }
    if (std::isdigit(input[index])) return extract_number();
    if (std::isalpha(input[index]) || input[index] == '_') return extract_identifier();
    if (input[index] == '"') return extract_string();
    if (input[index] == '\'') return extract_char();
    if (input[index] == '.') {
        if (index + 1 < input.size() && std::isdigit(input[index + 1])) {      
            return extract_number();
        }
        return extract_operator();
    }
    return extract_operator();
}

Token Lexer::extract_number() {
    std::size_t start = index;
    std::string value;

    if (input[index] == '.') {  
        // Число начинается с точки
        value = "0";    // .1  ->  0.1 
        value += input[index++];
        while (std::isdigit(input[index])) value += input[index++];
        return {TokenType::NUM_DOUBLE , value};
    } else {
        // Число начинается с цифры 
        while (std::isdigit(input[index])) value += input[index++];
        if (input[index] == '.') {
            value += input[index++];
            while (std::isdigit(input[index])) value += input[index++];
            return {TokenType::NUM_DOUBLE , value};
        }
    }
    // Если число заканчивается на .
    if (value.back() == '.') {
        value += "0";  // 1. ->  1.0
        return {TokenType::NUM_DOUBLE , value};
    }

    return {TokenType::NUM_INT, value};
}


Token Lexer::extract_identifier() {
    std::size_t start = index;
    while (std::isalnum(input[index]) || input[index] == '_') ++index;
    std::string name = input.substr(start, index - start);
    if (auto it = keywords.find(name); it != keywords.end()) {
        return {it->second, name};
    }
    if(name == "true") return {TokenType::BOOL, name};
    if(name == "false") return {TokenType::BOOL, name};
    return {TokenType::ID, name};
}

Token Lexer::extract_string() {
    std::size_t start = ++index;
    std::string value;
    
    while (index < input.size() && input[index] != '"') {
        if (input[index] == '\\') {
            ++index;
            if (index >= input.size()) break;
            switch (input[index]) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                case '0': value += '\0'; break;
                default: value += input[index]; break;
            }
        } else {
            value += input[index];
        }
        ++index;
    }
    if (index < input.size() && input[index] == '"') ++index; // пропускаем закрывающую кавычку
    else throw std::runtime_error("Unterminated string literal");

    return {TokenType::STRING, value};
}


Token Lexer::extract_char() {
    ++index; // пропускаем открывающую '
    if (index >= input.size()) throw std::runtime_error("Unterminated char literal");

    char ch;
    if (input[index] == '\\') {
        ++index;
        if (index >= input.size()) throw std::runtime_error("Unterminated escape sequence in char literal");
        switch (input[index]) {
            case 'n': ch = '\n'; break;
            case 't': ch = '\t'; break;
            case 'r': ch = '\r'; break;
            case '\\': ch = '\\'; break;
            case '\'': ch = '\''; break;
            case '0': ch = '\0'; break;
            default: ch = input[index]; break;
        }
    } else {
        ch = input[index];
    }
    ++index;

    if (index >= input.size() || input[index] != '\'') throw std::runtime_error("Unterminated char literal");
    ++index; // пропускаем закрывающую '

    return {TokenType::CHAR, std::string(1, ch)};
}


Token Lexer::extract_operator() {
    std::string op;
    // Собираем длинный оператор
    while (operators.find(op + input[index]) != operators.end()) {
        op += input[index++];
    }
    // Если нашли оператор в таблице
    if (!op.empty() && operators.find(op) != operators.end()) {
        return {operators.at(op), op};
    }
    throw std::runtime_error("Unknown operator: " + std::string(1, input[index]));
}

void Lexer::extract_comment() {
    if (input[index + 1] == '/') {  
        while (index < input.size() && input[index] != '\n') ++index;
    } else {  
        index += 2;  
        while (index + 1 < input.size() && !(input[index] == '*' && input[index + 1] == '/')) ++index;
        index += 2;  
    }
}

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"short", TokenType::TYPE_SHORT}, {"int", TokenType::TYPE_INT}, {"long", TokenType::TYPE_LONG},
    {"char", TokenType::TYPE_CHAR}, {"bool", TokenType::TYPE_BOOL}, {"void", TokenType::TYPE_VOID},
    {"ldouble", TokenType::TYPE_LONG_DOUBLE}, {"double", TokenType::TYPE_DOUBLE}, {"float", TokenType::TYPE_FLOAT},

    {"const", TokenType::KW_CONST}, {"static", TokenType::KW_STATIC}, 
    {"unsigned", TokenType::KW_UNSIGNED},

    {"True", TokenType::BOOL}, {"False", TokenType::BOOL},

    {"struct", TokenType::KW_STRUCT},
    {"typedef", TokenType::KW_TYPEDEF}, // Добавлено ключевое слово typedef
    {"if", TokenType::KW_IF}, {"else", TokenType::KW_ELSE}, {"while", TokenType::KW_WHILE}, 
    {"for", TokenType::KW_FOR}, {"return", TokenType::KW_RETURN}, {"break", TokenType::KW_BREAK}, 
    {"continue", TokenType::KW_CONTINUE},

    {"sizeof", TokenType::KW_SIZEOF}, 
    {"read", TokenType::KW_READ}, {"print", TokenType::KW_PRINT},
    {"assert", TokenType::KW_ASSERT}, {"exit", TokenType::KW_EXIT},

};

const std::unordered_map<std::string, TokenType> Lexer::operators = {
    {"+", TokenType::PLUS},   {"-", TokenType::MINUS}, {"*", TokenType::STAR},
    {"/", TokenType::SLASH},  {"%", TokenType::MOD},   {"=", TokenType::ASSIGN},
    {"++", TokenType::INC}, {"--", TokenType::DEC},
    {"+=", TokenType::PLUS_ASSIGN}, {"-=", TokenType::MINUS_ASSIGN},
    {"*=", TokenType::MULT_ASSIGN}, {"/=", TokenType::DIV_ASSIGN},
    {"%=", TokenType::MOD_ASSIGN},
    {"&&", TokenType::AND}, {"||", TokenType::OR}, {"!", TokenType::NOT},
    {"==", TokenType::EQ}, {"!=", TokenType::NEQ}, {">", TokenType::GT},
    {"<", TokenType::LT}, {">=", TokenType::GE}, {"<=", TokenType::LE},
    {"?", TokenType::QUESTION}, {":", TokenType::COLON},
    {";", TokenType::SEMICOLON}, {",", TokenType::COMMA}, {".", TokenType::DOT},
    {"(", TokenType::LPAREN}, {")", TokenType::RPAREN},
    {"{", TokenType::LBRACE},    {"}", TokenType::RBRACE}, 
    {"[", TokenType::LBRACKET}, {"]", TokenType::RBRACKET},
};

