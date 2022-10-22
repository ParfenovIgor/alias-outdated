%option noyywrap
%option nounistd
%option c++
%option yyclass="Lexer"
%option prefix="alias_"

%{
#include "lexer.h"
#include "shell.hpp"
#include "parser.hpp"
%}

digit      [0-9]
alpha      [a-zA-Z_]
alphanum   [a-zA-Z_0-9\.]

%%

#.* ; /* ignore comments */

[ \t]+ {

}

[\r\n]+ {
    
}

"int" {
    return alias::Parser::make_INT();
}

"ptr" {
    return alias::Parser::make_PTR();
}

"def" {
    return alias::Parser::make_DEF();
}

"alloc" {
    return alias::Parser::make_ALLOC();
}

"assume" {
    return alias::Parser::make_ASSUME();
}

"if" {
    return alias::Parser::make_IF();
}

"else" {
    return alias::Parser::make_ELSE();
}

":=" {
    return alias::Parser::make_ASSIGN();
}

"+" {
    return alias::Parser::make_PLUS();
}

"-" {
    return alias::Parser::make_MINUS();
}

"[" {
    return alias::Parser::make_BRACKET_OPEN();
}

"]" {
    return alias::Parser::make_BRACKET_CLOSE();
}

"{" {
    return alias::Parser::make_BRACE_OPEN();
}

"}" {
    return alias::Parser::make_BRACE_CLOSE();
}

{alpha}{alphanum}* {
    return alias::Parser::make_ID(yytext);
}

-?{digit}+ {
    return alias::Parser::make_CONST_INT(atoi(yytext));
}

<<EOF>> {
    return 0;
}

. {
    std::cerr << "Unknown token: " << yytext << '\n';
    std::exit(1);
}

%%
