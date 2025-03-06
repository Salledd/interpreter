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
    if (std::isdigit(input[index])) return extract_number();
    if (std::isalpha(input[index]) || input[index] == '_') return extract_identifier();
    if (input[index] == '"') return extract_string();
    if (input[index] == '\'') return extract_char();
    if (input[index] == '/' && (input[index + 1] == '/' || input[index + 1] == '*')){
        extract_comment();
        return extract();
    }
    return extract_operator();
}

Token Lexer::extract_number() {
    std::size_t start = index;
    bool is_double = false;
    std::string value;

    if (input[index] == '.') {  
        // Число начинается с точки
        is_double = true;
        value = "0";    // .1  ->  0.1 
        value += input[index++];
        while (std::isdigit(input[index])) value += input[index++];
    } else {
        // Число начинается с цифры 
        while (std::isdigit(input[index])) value += input[index++];
        if (input[index] == '.') {
            is_double = true;
            value += input[index++];
            while (std::isdigit(input[index])) value += input[index++];
        }
    }
    // Если число заканчивается на .
    if (is_double && value.back() == '.') {
        value += "0";  // 1. ->  1.0
    }

    return {is_double ? TokenType::TYPE_DOUBLE : TokenType::TYPE_INT, value};
}


Token Lexer::extract_identifier() {
    std::size_t start = index;
    while (std::isalnum(input[index]) || input[index] == '_') ++index;
    std::string name = input.substr(start, index - start);
    if (keywords.find(name) != keywords.end()) {
        return {keywords.at(name), name};
    }
    return {TokenType::ID, name};
}

Token Lexer::extract_string() {
    std::size_t start = ++index;
    while (input[index] != '"') ++index;
    std::string value = input.substr(start, index - start);
    ++index;
    return {TokenType::STRING, value};
}

Token Lexer::extract_char() {
    std::size_t start = ++index;
    while (input[index] != '\'') ++index;
    std::string value = input.substr(start, index - start);
    ++index;
    return {TokenType::CHAR, value};
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
        // Однострочный комментарий: игнорируем до конца строки
        while (index < input.size() && input[index] != '\n') ++index;
    } else {  
        // Многострочный комментарий: игнорируем до */
        index += 2;  
        while (index + 1 < input.size() && !(input[index] == '*' && input[index + 1] == '/')) ++index;
        index += 2;  
    }
}


const std::unordered_map<std::string, TokenType> Lexer::keywords = {

    {"int", TokenType::TYPE_INT}, {"double", TokenType::TYPE_DOUBLE}, 
    {"char", TokenType::TYPE_CHAR}, {"bool", TokenType::TYPE_BOOL}, {"void", TokenType::TYPE_VOID},

    {"if", TokenType::KW_IF}, {"else", TokenType::KW_ELSE}, {"while", TokenType::KW_WHILE}, 
    {"for", TokenType::KW_FOR}, {"return", TokenType::KW_RETURN}, {"break", TokenType::KW_BREAK}, 
    {"continue", TokenType::KW_CONTINUE},

    {"sizeof", TokenType::KW_SIZEOF}, {"read", TokenType::KW_READ}, {"print", TokenType::KW_PRINT},
    {"const", TokenType::KW_CONST}, {"static_assert", TokenType::KW_STATIC_ASSERT}, 
    {"assert", TokenType::KW_ASSERT}, {"exit", TokenType::KW_EXIT}
};



const std::unordered_map<std::string, TokenType> Lexer::operators = {
    {"+", TokenType::PLUS},   {"-", TokenType::MINUS}, {"*", TokenType::STAR},
    {"/", TokenType::SLASH},  {"%", TokenType::MOD},   {"=", TokenType::ASSIGN},
    {"++", TokenType::INCREMENT}, {"--", TokenType::DECREMENT},
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

