#include "validator.hpp"

#define RED         "\033[31m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define RESET       "\033[0m"

#define GDEBUG(X)   if (shell.debug) std::cout << CYAN << X << RESET << std::endl;
#define GWARNING(X) std::cerr << YELLOW << "[TYPE]: [WARNINGS]:\n" << X << RESET << std::endl;
#define GERROR(X)   std::cerr << RED << "[TYPE]: [ERROR]: " << X << RESET << std::endl; std::exit(1);

#define BLOCK_B(X)                                           \
    if (shell.debug) {                                       \
        std::cout << CYAN;                                   \
        for (int i=0; i<spaces; i++) {                       \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "<" << X << ">" << RESET << std::endl;  \
        spaces += 4;                                         \
    }

#define BLOCK_E(X)                                           \
    if (shell.debug) {                                       \
        spaces -= 4;                                         \
        std::cout << CYAN;                                   \
        for (int i=0; i<spaces; i++) {                       \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "</" << X << ">" << RESET << std::endl; \
    }

#define BLOCK_C(X)                                           \
    if (shell.debug) {                                       \
        std::cout << YELLOW;                                 \
        for (int i=0; i<spaces; i++) {                       \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "[" << X << "]" << RESET << std::endl;  \
    }

extern alias::Shell shell;

namespace ast {

bool operator < (const State &a, const State &b) {
    return a.heap < b.heap;
}

Validator::Validator() {
    states.push_back(State());
}

int Validator::getIndex(std::string id) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return i;
        }
    }
    throw "Variable not declared";
}

Type Validator::getType(std::string id) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return variable_type[i];
        }
    }
    throw "Variable not declared";
}

void Validator::simplifyStates() {
    std::set <State> _states;
    for (State state : states)
        _states.insert(state);
    states.clear();
    for (State state : _states)
        states.push_back(state);
}

void Validator::printStates() {
    BLOCK_C("[")
    for (State state : states) {
        std::string s = "{";
        for (auto i : state.heap) {
            if (i.first == -1)
                s += "OUT    ";
            else
                s += "_" + std::to_string(i.first) + " + " + std::to_string(i.second) + "    ";
        }
        s += "}";
        BLOCK_C(s)
    }
    BLOCK_C("]")
}

void Validator::visit(ast::Block *block) {
    BLOCK_B("Block")

    size_t variable_stack_size = variable_stack.size();
    if (!block->defs.empty()) {
        for (auto i = --block->defs.end(); i >= block->defs.begin(); i--) {
            (*i)->accept(this);
        }
    }

    while (variable_stack.size() > variable_stack_size)
        variable_stack.pop_back();

    BLOCK_E("Block")
}

void Validator::visit(ast::If *if_) {
    BLOCK_B("If")

    std::vector <State> _states = states;
    std::cout << -1 << std::endl;
    if_->block1->accept(this);
    std::cout << -1 << std::endl;
    std::vector <State> _states1 = states;
    states = _states;
    std::vector <State> _states2 = states;
    if (if_->block2) {
        std::cout << -1 << std::endl;
        if_->block2->accept(this);
        std::cout << -1 << std::endl;
        _states2 = states;
    }
    states.clear();
    for (State &state : _states1)
        states.push_back(state);
    for (State &state : _states2)
        states.push_back(state);
    
    printStates();

    BLOCK_E("If")
}

void Validator::visit(ast::Definition *definition) {
    BLOCK_B("Definition")

    variable_stack.push_back({definition->name, variable_index});
    variable_type.push_back(definition->type);
    for (State &state : states) {
        state.heap.push_back({-1, 0});
    }
    variable_index++;

    std::cout << (int)states.size() << std::endl;

    BLOCK_E("Definition")
}

void Validator::visit(ast::Assignment *assignment) {
    BLOCK_B("Assignment")

    if (assignment->deref1 == false && assignment->deref2 == false && assignment->addr2 == false) {
        if (assignment->alloc != 0) {
            int index1 = getIndex(assignment->name1);
            heap_size.push_back(assignment->alloc);
            for (State &state : states) {
                state.heap[index1] = {heap_index, 0};
            }
            heap_index++;
        }
        else if (getType(assignment->name1) == Type::Ptr && getType(assignment->name2) == Type::Ptr) {
            int index1 = getIndex(assignment->name1);
            int index2 = getIndex(assignment->name2);
            for (State &state : states) {
                if (state.heap[index2].first == -1){
                    state.heap[index1] = {-1, 0};
                }
                else{
                    state.heap[index1] = {state.heap[index2].first, state.heap[index2].second + assignment->phase};
                }
            }
        }
        else if (getType(assignment->name1) == Type::Ptr && getType(assignment->name2) == Type::Int){
            int index1 = getIndex(assignment->name1);
            std::vector <State> _states;
            for (State &state : states) {
                for (int i = -1; i < heap_index; i++) {
                    for (int j = 0; j < ((i == -1) ? 1 : heap_size[i]); j++) {
                        State _state = state;
                        _state.heap[index1] = {i, j};
                        _states.push_back(_state);
                    }
                }
            }
            states = _states;
        }

        simplifyStates();
        printStates();
    }

    for (State &state : states) {
        std::vector <int> used(heap_size.size());
        for (auto i : state.heap) {
            if (i.first != -1) {
                used[i.first] = 1;
            }
        }
        for (int i : used) {
            if (!i) {
                std::cout << "-2" << std::endl;
            }
        }
    }

    std::cout << (int)states.size() << std::endl;

    BLOCK_E("Assignment")
}

void Validator::visit(ast::Assumption *assumption) {
    BLOCK_B("Assumption")

    int index1 = getIndex(assumption->name1);
    int index2 = getIndex(assumption->name2);
    std::vector <State> _states;
    for (State &state : states) {
        if (state.heap[index1].first == state.heap[index2].first && 
            state.heap[index1].second - state.heap[index2].second == assumption->phase) {
            _states.push_back(state);
        }
    }
    states = _states;
    
    printStates();
    std::cout << (int)states.size() << std::endl;

    BLOCK_E("Assupmtion")
}

} // namespace ast
