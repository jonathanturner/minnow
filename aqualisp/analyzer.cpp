// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <iostream>
#include <stdlib.h>

#include "parser.hpp"
#include "analyzer.hpp"


void debug_print(ProgPtr prog, VarPtr var, std::string prepend) {
    std::cout << prepend << var->readable_name << " : " << var->type << std::endl;
}

void debug_print(ProgPtr prog, TypePtr type, std::string prepend) {
    std::cout << prepend << type->readable_name << " " << type->type << std::endl;

    for (std::map<std::string, int>::iterator iter = type->attributes.begin(), end = type->attributes.end(); iter != end; ++iter) {
        debug_print(prog, prog->variables[iter->second], prepend + "  ");
    }

    for (std::map<std::string, int>::iterator iter = type->methods.begin(), end = type->methods.end(); iter != end; ++iter) {
        debug_print(prog, prog->functions[iter->second], prepend + "  ");
    }
}

void debug_print(ProgPtr prog, FuncPtr func, std::string prepend) {
    std::cout << prepend << func->readable_name << " [";
    for (unsigned int i = 0; i < func->parameter_types.size(); ++i) {
        std::cout << func->parameter_types[i] << " ";
    }
    std::cout << "] : " << func->return_type << std::endl;

    for (std::map<std::string, int>::iterator iter = func->variables.begin(), end = func->variables.end(); iter != end; ++iter) {
        debug_print(prog, prog->variables[iter->second], prepend + "  ");
    }

}

void debug_print(ProgPtr prog) {
    std::cout << "Variables: " << std::endl;
    for (unsigned int i = 0; i < prog->variables.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog, prog->variables[i], "  ");
    }
    std::cout << "Types: " << std::endl;
    for (unsigned int i = 0; i < prog->types.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog, prog->types[i], "  ");
    }
    std::cout << "Functions: " << std::endl;
    for (unsigned int i = 0; i < prog->functions.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog, prog->functions[i], "  ");
    }
}

void require_minimum_size(ExPtr ex, unsigned int min) {
    if (ex->args.size() < min) {
        std::cerr << "Number of arguments doesn't not meet minimum requirement for: " << std::endl;
        debug_print(ex, "");
        exit(1);
    }
}
int find_type(ProgPtr prog, const std::string &type_name) {
    if (prog->type_lookup.find(type_name) != prog->type_lookup.end()) {
        return prog->type_lookup[type_name];
    }
    else {
        return -1;
    }
}
int find_var(ProgPtr prog, const std::string &var_name) {
    if (prog->variable_lookup.find(var_name) != prog->variable_lookup.end()) {
        return prog->variable_lookup[var_name];
    }
    else {
        return -1;
    }
}
void add_type(ProgPtr prog, ExPtr ex, Class_Type::Type type) {
    require_minimum_size(ex, 1);
    if (find_type(prog, ex->args[0]->command) != -1) {
        std::cerr << "Conflicting type definitions for: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    else {
        TypePtr new_type(new Type());
        new_type->readable_name = ex->args[0]->command;
        new_type->type = type;
        new_type->root = ex;

        prog->types.push_back(new_type);
        prog->type_lookup[ex->args[0]->command] = prog->types.size() - 1;
    }
}

void add_variable(ProgPtr prog, ExPtr ex) {
    require_minimum_size(ex, 2);
    if (ex->command != "var") {
        std::cerr << "Variable declarations should start with 'var'" << std::endl;
        exit(1);
    }
    int type_id = find_type(prog, ex->args[1]->command);
    if (type_id == -1) {
        std::cerr << "Can't find parent type '" << ex->args[1]->command << " for variable: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    int var_id = find_var(prog, ex->args[0]->command);
    if (var_id != -1) {
        std::cerr << "Duplicate definition of variable: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    VarPtr new_var(new Variable());
    new_var->type = type_id;
    new_var->readable_name = ex->args[0]->command;
    new_var->root = ex;

    prog->variables.push_back(new_var);
    prog->variable_lookup[ex->args[0]->command] = prog->variables.size() - 1;
}

void add_function(ProgPtr prog, std::string &name, int return_type, ExPtr arg, ExPtr root, Function_Type::Type ftype, bool is_extern) {
    //require_minimum_size(ex, 1);
    if (prog->function_lookup.find(name) != prog->function_lookup.end()) {
        std::cerr << "Conflicting function definitions for: " << name << std::endl;
        exit(1);
    }
    else {
        FuncPtr new_func(new Function());
        new_func->readable_name = name;
        new_func->type = ftype;
        new_func->root = root;
        new_func->is_extern = is_extern;
        if (arg->command != "list") {
            std::cerr << "Missing parameter list in arguments for " << name << std::endl;
            exit(1);
        }

        for (unsigned int i = 0; i < arg->args.size(); ++i) {
            add_variable(prog, arg->args[i]);
            VarPtr var = prog->variables[prog->variables.size() - 1];
            new_func->parameter_types.push_back(var->type);
            new_func->variables[arg->args[i]->args[0]->command] = prog->variables.size() - 1;
        }
        //new_type->parameter_types
        new_func->return_type = return_type;

        prog->functions.push_back(new_func);
        prog->function_lookup[name] = prog->functions.size() - 1;
    }
}

void analyze_type_decl_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if (arg->command == "defactor") {
            add_type(prog, arg, Class_Type::Actor);
        }
        else if (arg->command == "defstruct") {
            add_type(prog, arg, Class_Type::Struct);
        }
    }
}

void analyze_type_def_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if ((arg->command == "defactor") || (arg->command == "defstruct")) {
            if (find_type(prog, arg->args[0]->command) != -1) {
                TypePtr type = prog->types[find_type(prog, arg->args[0]->command)];
                for (unsigned int j = 1; j < arg->args.size(); ++j) {
                    add_variable(prog, arg->args[j]);
                    type->attributes[arg->args[j]->args[0]->command] = prog->variables.size() - 1;
                }
            }
            else {
                std::cerr << "Internal error, type parsed incorrectly: " << arg->args[0]->command << std::endl;
                exit(1);
            }
        }
    }
}

void analyze_func_decl_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if (arg->command == "defaction") {
            require_minimum_size(arg, 4);
            add_function(prog, arg->args[1]->command, find_type(prog, "void"), arg->args[2], arg->args[3], Function_Type::Action, false);
            int t = find_type(prog, arg->args[0]->command);
            if (t == -1) {
                std::cerr << "Can not find parent type for action: " << arg->args[1]->command << std::endl;
                exit(1);
            }
            TypePtr parent = prog->types[t];
            if (parent->type != Class_Type::Actor) {
                std::cerr << "Parent type not actor for action: " << arg->args[1]->command << std::endl;
                exit(1);
            }
            parent->methods[arg->args[1]->command] = prog->functions.size()-1;
        }
        else if (arg->command == "deffun") {
            if (find_type(prog, arg->args[1]->command) != -1) {
                require_minimum_size(arg, 4);
                add_function(prog, arg->args[0]->command, prog->type_lookup[arg->args[1]->command], arg->args[2], arg->args[3], Function_Type::Action, false);
            }
            else {
                std::cerr << "Can not find type in function call: " << arg->args[1]->command << std::endl;
                exit(1);
            }
        }
        else if (arg->command == "defextern") {
            if (find_type(prog, arg->args[1]->command) != -1) {
                require_minimum_size(arg, 4);
                add_function(prog, arg->args[0]->command, prog->type_lookup[arg->args[1]->command], arg->args[2], arg->args[3], Function_Type::Action, true);
            }
            else {
                std::cerr << "Can not find type in function call: " << arg->args[1]->command << std::endl;
                exit(1);
            }
        }
        else if (arg->command == "defmethod") {
            require_minimum_size(arg, 5);
            add_function(prog, arg->args[1]->command, find_type(prog, arg->args[2]->command), arg->args[3], arg->args[4], Function_Type::Action, false);
            int t = find_type(prog, arg->args[0]->command);
            if (t == -1) {
                std::cerr << "Can not find parent type for method: " << arg->args[1]->command << std::endl;
                exit(1);
            }
            TypePtr parent = prog->types[t];
            parent->methods[arg->args[1]->command] = prog->functions.size()-1;
        }
    }
}

void analyze_func_def_pass(ProgPtr prog, ExPtr ex) {

}



