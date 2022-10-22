#ifndef LEXER_H
#define LEXER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer alias_FlexLexer
#include <FlexLexer.h>
#endif

// Replaces `int yylex()` with `symbol_type get_next_token()` to be compatible with bison 3
#undef YY_DECL
#define YY_DECL alias::Parser::symbol_type alias::Lexer::get_next_token()

#include "parser.hpp"
#include <fstream>

namespace alias {
    class Shell; 
    
    class Lexer : public yyFlexLexer {
    public:
        Lexer(Shell& shell) : driver(shell) {}
        virtual ~Lexer() {}
        virtual alias::Parser::symbol_type get_next_token();
        
    private:
        Shell &driver;
    };

}

#endif // LEXER_H