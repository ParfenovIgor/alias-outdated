#include <iostream>

#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "validator.hpp"

extern alias::Shell shell;
extern ast::node_ptr<ast::Block> program;

int main(int argc, char **argv) {
        
    if (shell.parse_args(argc, argv)) {
        std::cerr << "Error parsing arguments\n";
        return 1;
    }

    if (shell.parse_program()) {
        std::cerr << "Error parsing program\n";
        return 1;
    }

    ast::Validator tc;
    try {
        program->accept(&tc);
    }
    catch (const char *error) {
        std::cerr << error;
        return 1;
    }
    
    return 0;
}