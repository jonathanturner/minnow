// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <iostream>
#include <stdlib.h>

#include "analyzer.hpp"


void debug_print(VarPtr var) {
    std::cout << var->readable_name << " " << var->type << std::endl;
}

void debug_print(TypePtr type) {
    std::cout << type->readable_name << " " << type->type << std::endl;
}

void debug_print(FuncPtr func) {
    std::cout << func->return_type << " " << func->readable_name << " ";
    for (unsigned int i = 0; i < func->parameter_types.size(); ++i) {
        std::cout << func->parameter_types[i] << " ";
    }
    std::cout << std::endl;
}

void debug_print(ProgPtr prog) {
    std::cout << "Variables: " << std::endl;
    for (unsigned int i = 0; i < prog->variables.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog->variables[i]);
    }
    std::cout << "Types: " << std::endl;
    for (unsigned int i = 0; i < prog->types.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog->types[i]);
    }
    std::cout << "Functions: " << std::endl;
    for (unsigned int i = 0; i < prog->functions.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog->functions[i]);
    }
}

void analyze_type_decl_pass(ExPtr ex, ProgPtr prog) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if (arg->command == "defactor") {
            if (prog->type_lookup.find(arg->args[0]->command) != prog->type_lookup.end()) {
                std::cout << "Conflicting type definitions for: " << arg->args[0]->command << std::endl;
                exit(1);
            }
            else {
                TypePtr new_type(new Type());
                new_type->readable_name = arg->args[0]->command;
                new_type->type = Class_Type::Actor;
                new_type->root = arg;

                prog->types.push_back(new_type);
                prog->type_lookup[arg->args[0]->command] = prog->types.size() - 1;
            }
        }
    }
}

void analyze_type_def_pass(ExPtr ex, ProgPtr prog) {

}

void analyze_func_decl_pass(ExPtr ex, ProgPtr prog) {

}

void analyze_func_def_pass(ExPtr ex, ProgPtr prog) {

}



