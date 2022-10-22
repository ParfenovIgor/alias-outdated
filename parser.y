%language "C++"
%skeleton "lalr1.cc"
%require "3.2"

%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define api.parser.class { Parser }
%define api.namespace    { alias }

%lex-param   { alias::Lexer &lexer }
%lex-param   { alias::Shell &shell }
%parse-param { alias::Lexer &lexer }
%parse-param { alias::Shell &shell }

%token INT PTR DEF ALLOC ASSUME IF ELSE ASSIGN PLUS MINUS BRACKET_OPEN BRACKET_CLOSE BRACE_OPEN BRACE_CLOSE ID CONST_INT

%type <std::string> ID
%type <int> CONST_INT

%type <ast::node_ptr<ast::Block>> BLOCK
%type <ast::node_ptr<ast::If>> IF_STATEMENT
%type <ast::node_ptr<ast::Definition>> DEFINITION
%type <ast::node_ptr<ast::Assignment>> ASSIGNMENT
%type <ast::node_ptr<ast::Assumption>> ASSUMPTION

%start BLOCK

%code requires {
    #include <iostream>
    #include <string>
    #include <vector>
    #include "ast.hpp"

    namespace alias {
    class Lexer;
    class Shell;
    }
}

%code top {
    #include "lexer.h"
    #include "shell.hpp"

    static alias::Parser::symbol_type yylex(alias::Lexer &lexer, alias::Shell &shell) {
        return lexer.get_next_token();
    }
    
    ast::node_ptr<ast::Block> program = nullptr;  // Points to the whole program node.
}


%%

BLOCK :
    %empty {
        $$ = std::make_shared<ast::Block> ();
        program = $$;
    }
    | BRACE_OPEN BLOCK BRACE_CLOSE BLOCK {
        $4->defs.push_back($2);
        $$ = $4;
    }
    | IF_STATEMENT BLOCK {
        $2->defs.push_back($1);
        $$ = $2;
    }
    | DEFINITION BLOCK {
        $2->defs.push_back($1);
        $$ = $2;
    }
    | ASSIGNMENT BLOCK {
        $2->defs.push_back($1);
        $$ = $2;
    }
    | ASSUMPTION BLOCK {
        $2->defs.push_back($1);
        $$ = $2;
    }
;

IF_STATEMENT :
    IF BRACE_OPEN BLOCK BRACE_CLOSE {
        $$ = std::make_shared<ast::If> ($3);
    }
    | IF BRACE_OPEN BLOCK BRACE_CLOSE ELSE BRACE_OPEN BLOCK BRACE_CLOSE {
        $$ = std::make_shared<ast::If> ($3, $7);
    }
;

DEFINITION :
    DEF INT ID {
        $$ = std::make_shared<ast::Definition> ($3, ast::Type::Int);
    }
    | DEF PTR ID {
        $$ = std::make_shared<ast::Definition> ($3, ast::Type::Ptr);
    }
;

ASSIGNMENT :
    ID ASSIGN ID {
        $$ = std::make_shared<ast::Assignment> ($1, $3, 0, 0, false, false, false);
    }
    | ID ASSIGN ID PLUS CONST_INT {
        $$ = std::make_shared<ast::Assignment> ($1, $3, 0, $5, false, false, false);
    }
    | ID ASSIGN ALLOC CONST_INT {
        $$ = std::make_shared<ast::Assignment> ($1, "", $4, 0, false, false, false);
    }
;

ASSUMPTION :
    ASSUME ID ASSIGN ID {
        $$ = std::make_shared<ast::Assumption> ($2, $4, 0);
    }
    | ASSUME ID ASSIGN ID PLUS CONST_INT {
        $$ = std::make_shared<ast::Assumption> ($2, $4, $6);
    }
;

%%
void alias::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
