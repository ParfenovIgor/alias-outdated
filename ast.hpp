#ifndef AST_H
#define AST_H

#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <set>

// Forward declarations
namespace ast {
struct Node;
struct Statement;
struct Block;
struct If;
struct Definition;
struct Assignment;
struct Assumption;
} // namespace ast

// Base class for code generator and anything that traverses AST.
class Visitor {
public:
    virtual void visit(ast::Block *block) = 0;
    virtual void visit(ast::If *if_) = 0;
    virtual void visit(ast::Definition *definition) = 0;
    virtual void visit(ast::Assignment *assignment) = 0;
    virtual void visit(ast::Assumption *assumption) = 0;
};

namespace ast {

template <typename Node> using node_ptr = std::shared_ptr<Node>;

enum class TypeEnum { EMPTY, INT, REAL, BOOL, ARRAY, RECORD, FUNCTION, STRING };
enum class OperatorEnum { PLUS, MINUS, MUL, DIV, MOD, AND, OR, NOT, XOR, EQ, NEQ, LT, GT, LEQ, GEQ }; 

// Base class for AST nodes
struct Node {
    virtual void accept(Visitor *v) = 0;
};

struct Statement : virtual Node {
    void accept(Visitor *v) override = 0;
};

struct Block : Statement {
    std::vector<node_ptr<Statement>> defs;
    
    void accept(Visitor *v) override { v->visit(this); }
};

struct If : Statement {
    node_ptr<Block> block1, block2;

    If(node_ptr<Block> block1) {
        this->block1 = block1;
        this->block2 = nullptr;
    }

    If(node_ptr<Block> block1, node_ptr<Block> block2) {
        this->block1 = block1;
        this->block2 = block2;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

enum class Type {
    Int,
    Ptr,
};

struct Definition : Statement {
    std::string name;
    Type type;

    Definition(std::string name, Type type) {
        this->name = name;
        this->type = type;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct Assignment : Statement {
    std::string name1, name2;
    int alloc, phase;
    bool deref1, deref2, addr2;

    Assignment(std::string name1, std::string name2, int alloc, int phase, bool deref1, bool deref2, bool addr2) {
        this->name1 = name1;
        this->name2 = name2;
        this->alloc = alloc;
        this->phase = phase;
        this->deref1 = deref1;
        this->deref2 = deref2;
        this->addr2 = addr2;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct Assumption : Statement {
    std::string name1, name2;
    int phase;

    Assumption(std::string name1, std::string name2, int phase) {
        this->name1 = name1;
        this->name2 = name2;
        this->phase = phase;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

// </Statements>
} // namespace ast

#endif // AST_H
