#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "ast.hpp"
#include "shell.hpp"

namespace ast {

struct State {
    std::vector <std::pair <int, int> > heap; // {index, size}
};

bool operator < (State &a, State &b);

// Visits AST nodes and checks types.
class Validator : public Visitor {
public:
    Validator();
    void generate();
    void visit(ast::Block *block) override;
    void visit(ast::If *if_) override;
    void visit(ast::Definition *definition) override;
    void visit(ast::Assignment *assignment) override;
    void visit(ast::Assumption *assumption) override;
private:
    int getIndex(std::string id);
    Type getType(std::string id);
    void simplifyStates();
    void printStates();
    std::vector <std::pair <std::string, int> > variable_stack; // {name, index}
    std::vector <Type> variable_type;
    std::vector <int> heap_size;
    int variable_index = 0;
    int heap_index = 0;
    
    std::vector <State> states;

    int spaces = 0;
};
} // namespace ast

#endif // VALIDATOR_H
