#pragma once

#include <string>

enum class TokenType {
    TYPE_INT, TYPE_DOUBLE, TYPE_CHAR, TYPE_BOOL, TYPE_VOID,

    KW_IF, KW_ELSE, KW_WHILE, KW_FOR, KW_RETURN, KW_BREAK, KW_CONTINUE,
    KW_SIZEOF, KW_READ, KW_PRINT, KW_CONST, KW_STATIC_ASSERT, KW_ASSERT, KW_EXIT,

    ID, STRING, CHAR, END,

    PLUS, MINUS, STAR, SLASH, MOD,          // +  -  *  /  %
    INCREMENT, DECREMENT,                   // ++  --
    AND, OR, NOT,                           // &&  ||  !
    EQ, NEQ, GT, LT, GE, LE,                // ==  !=  >  <  >=  <=
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN,      // =  +=  -=
    MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,    // *=  /=  %=
    QUESTION, COLON,                        // ? :
    SEMICOLON, COMMA, DOT,                  // ; , .
    LPAREN, RPAREN,                         // ()
    LBRACE, RBRACE,                         // {}
    LBRACKET, RBRACKET,                     // []
};


struct Token {
    TokenType type;
    std::string value;

    bool operator==(TokenType t) const {
        return this->type == t;
    }
};
