<translation_unit> ::= <decl>*
<decl> ::= <var_decl>
         | <func_decl>
         | <struct_decl>
<var_decl> ::= [ "const" ] <type_or_id> <init_declarator> ( "," <init_declarator> )* ";"
<init_declarator> ::= <declarator> [ "=" <expr> ]
<declarator> ::= <id>
              | <id> "[" <expr>? "]"
<func_decl> ::= <type_or_id> <id> "(" [ <param_decl> ( "," <param_decl> )* ] ")" ( ";" | <block_st> )
<param_decl> ::= <type_or_id> <declarator>
<struct_decl> ::= "struct" <id> "{" <var_decl>+ "}" ";"

<type_or_id> ::= <type> | <id>
<type> ::= "int" | "double" | "char" | "bool" | "void"

<statement> ::= <expr_st>
              | <conditional_st>
              | <loop_st>
              | <return_st>
              | <continue_st>
              | <break_st>
              | <decl_st>
              | <block_st>
              | <io_st>
              | <assert_st>
              | <exit_st>
<expr_st> ::= <expr>? ";"
<decl_st> ::= <var_decl>  // точка с запятой уже внутри var_decl
<return_st> ::= "return" <expr>? ";"
<break_st> ::= "break" ";"
<continue_st> ::= "continue" ";"
<block_st> ::= "{" <statement>* "}"
<conditional_st> ::= "if" "(" <expr> ")" <statement> [ "else" <statement> ]
<loop_st> ::= <while_st>
            | <do_while_st>
            | <for_st>
<while_st> ::= "while" "(" <expr> ")" <statement>
<do_while_st> ::= "do" <statement> "while" "(" <expr> ")" ";"
<for_st> ::= "for" "(" [ <var_decl> | <expr> ] ";" <expr>? ";" <expr>? ")" <statement>
<io_st> ::= "print" "(" <expr> ")" ";"
          | "read" "(" <id> ")" ";"
<assert_st> ::= "assert" "(" <expr> ")" ";"
              | "static_assert" "(" <expr> ")" ";"
<exit_st> ::= "exit" "(" <expr>? ")" ";"

<expr> ::= <ternary_expr>
<ternary_expr> ::= <binary_expr> [ "?" <expr> ":" <expr> ]
<binary_expr> ::= <unary_expr> ( <binary_op> <unary_expr> )*
<unary_expr> ::= ( <unary_op> | <cast> ) <unary_expr>
               | <postfix_expr>
<cast> ::= "(" <type_or_id> ")" 
<postfix_expr> ::= <primary_expr>
                 | <postfix_expr> "++"
                 | <postfix_expr> "--"
                 | <postfix_expr> "[" <expr> "]"
                 | <postfix_expr> "(" [ <expr> ( "," <expr> )* ] ")"
                 | <postfix_expr> "." <id>
<primary_expr> ::= <literal>
                 | <id>
                 | "(" <expr> ")"
                 | "sizeof" "(" <type_or_id> ")"

<literal> ::= <int_literal> | <double_literal> | <char_literal> | <string_literal> | "true" | "false"
<binary_op> ::= "+" | "-" | "*" | "/" | "%" 
              | "==" | "!=" | "<" | "<=" | ">" | ">=" 
              | "&&" | "||"
              | "=" | "+=" | "-=" | "*=" | "/=" | "%=" 
              | "," 
<unary_op> ::= "+" | "-" | "!" | "~" | "++" | "--"
<id> ::= [a-zA-Z_][a-zA-Z0-9_]*
