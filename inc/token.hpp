#pragma once

#include <string>

enum class TokenType {
    TYPE_LONG, TYPE_INT, TYPE_SHORT, TYPE_CHAR, TYPE_BOOL, TYPE_VOID,
    TYPE_LONG_DOUBLE, TYPE_DOUBLE, TYPE_FLOAT, 
    
    KW_CONST, KW_STATIC, KW_UNSIGNED, 
    
    KW_STRUCT,
    KW_IF, KW_ELSE, KW_WHILE, KW_DO, KW_FOR, KW_RETURN, KW_BREAK, KW_CONTINUE,
    KW_SIZEOF, KW_READ, KW_PRINT, KW_ASSERT, KW_EXIT,
    KW_TYPEDEF,

    ID,
    
    STRING, CHAR, NUM_INT, NUM_DOUBLE, BOOL,

    PLUS, MINUS, STAR, SLASH, MOD,          // +  -  *  /  %
    INC, DEC,                   // ++  --
    AND, OR, NOT,                           // &&  ||  !
    EQ, NEQ, GT, LT, GE, LE,                // ==  !=  >  <  >=  <=
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN,      // =  +=  -=
    MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,    // *=  /=  %=
    QUESTION, COLON,                        // ? :
    SEMICOLON, COMMA, DOT,                  // ; , .
    LPAREN, RPAREN,                         // ()
    LBRACE, RBRACE,                         // {}
    LBRACKET, RBRACKET,                     // []

    END,
};


struct Token {
    TokenType type;
    std::string value;

    bool operator==(TokenType t) const {
        return this->type == t;
    }
};
