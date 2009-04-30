// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <iostream>
#include <stdlib.h>

#include "parser.hpp"
#include "analyzer.hpp"

int find_meta(ProgPtr prog, int meta_id) {
    if (prog->meta_lookup.find(meta_id) != prog->meta_lookup.end()) {
        return prog->meta_lookup[meta_id];
    }
    else {
        return -1;
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

std::string get_name_for_type(ProgPtr prog, int type_id) {
    if ((type_id >= 0) && (type_id < (int)prog->types.size())) {
        return prog->types[type_id]->readable_name;
    }
    else {
        return "typeless";
    }
}
void debug_print(ProgPtr prog, MetaPtr meta, std::string prepend) {
    std::cout << prepend << "fn: " << meta->filename << " start: (" << meta->start_pos.line << ", " << meta->start_pos.column;
    std::cout << ") end: (" << meta->end_pos.line << ", " << meta->end_pos.column << ") def: " << meta->definition_id;
    std::cout << " type: " << get_name_for_type(prog, meta->type_id) << " orig: " << meta->orig_name << std::endl;
}

void debug_print(ProgPtr prog, VarPtr var, std::string prepend) {
    std::cout << prepend << var->readable_name << " : " << get_name_for_type(prog, var->type_id) << std::endl;
}

void debug_print(ProgPtr prog, TypePtr type, std::string prepend) {
    std::cout << prepend << type->readable_name << " ";
    if (type->type == Class_Type::Actor) {
        std::cout << "(Actor)" << std::endl;
    }
    else {
        std::cout << "(Basic)" << std::endl;
    }

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
        std::cout << get_name_for_type(prog, func->parameter_types[i]) << " ";
    }
    std::cout << "] : " << get_name_for_type(prog, func->return_type) << std::endl;

    for (std::map<std::string, int>::iterator iter = func->variables.begin(), end = func->variables.end(); iter != end; ++iter) {
        debug_print(prog, prog->variables[iter->second], prepend + "  ");
    }

}

void debug_print(ProgPtr prog) {
    std::cout << "Metadata: " << std::endl;
    for (unsigned int i = 0; i < prog->metas.size(); ++i) {
        std::cout << "  " << i << ") ";
        debug_print(prog, prog->metas[i], "  ");
    }
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

void add_meta(ProgPtr prog, ExPtr ex) {
    require_minimum_size(ex, 9);

    int meta_id = atoi(ex->args[0]->command.c_str());

    if (find_meta(prog, meta_id) != -1 ) {
        std::cerr << "Conflicting definitions for meta: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    else {
        std::string filename = ex->args[1]->command;
        int start_line = atoi(ex->args[2]->command.c_str());
        int start_col = atoi(ex->args[3]->command.c_str());
        int end_line = atoi(ex->args[4]->command.c_str());
        int end_col = atoi(ex->args[5]->command.c_str());
        int definition_id = atoi(ex->args[6]->command.c_str());
        int type_id = atoi(ex->args[7]->command.c_str());
        std::string orig_name = ex->args[8]->command;

        MetaPtr new_meta(new Metadata());
        new_meta->filename = filename;
        new_meta->start_pos.line = start_line;
        new_meta->start_pos.column = start_col;
        new_meta->end_pos.line = end_line;
        new_meta->end_pos.column = end_col;
        new_meta->definition_id = definition_id;
        new_meta->type_id = type_id;
        new_meta->orig_name = orig_name;

        prog->metas.push_back(new_meta);
        prog->meta_lookup[meta_id] = prog->metas.size() - 1;
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
        std::cerr << "Can't find parent type '" << ex->args[1]->command << "' for variable: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    int var_id = find_var(prog, ex->args[0]->command);
    if (var_id != -1) {
        std::cerr << "Duplicate definition of variable: " << ex->args[0]->command << std::endl;
        exit(1);
    }
    VarPtr new_var(new Variable());
    new_var->type_id = type_id;
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
            new_func->parameter_types.push_back(var->type_id);
            new_func->variables[arg->args[i]->args[0]->command] = prog->variables.size() - 1;
        }
        //new_type->parameter_types
        new_func->return_type = return_type;

        prog->functions.push_back(new_func);
        prog->function_lookup[name] = prog->functions.size() - 1;
    }
}

void analyze_meta_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if (arg->command == "meta") {
            add_meta(prog, arg);
        }
    }
}

void analyze_type_decl_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if (arg->command == "defactor") {
            add_type(prog, arg, Class_Type::Actor);
        }
        else if (arg->command == "deftype") {
            add_type(prog, arg, Class_Type::Basic);
        }
    }
}

void analyze_type_def_pass(ProgPtr prog, ExPtr ex) {
    for (unsigned int i = 0; i < ex->args.size(); ++i) {
        ExPtr arg = ex->args[i];
        if ((arg->command == "defactor") || (arg->command == "deftype")) {
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
                add_function(prog, arg->args[0]->command, prog->type_lookup[arg->args[1]->command], arg->args[2], arg->args[3], Function_Type::Function, false);
            }
            else {
                std::cerr << "Can not find type in function call: " << arg->args[1]->command << std::endl;
                exit(1);
            }
        }
        else if (arg->command == "defextern") {
            if (find_type(prog, arg->args[1]->command) != -1) {
                require_minimum_size(arg, 4);
                add_function(prog, arg->args[0]->command, prog->type_lookup[arg->args[1]->command], arg->args[2], arg->args[3], Function_Type::Function, true);
            }
            else {
                std::cerr << "Can not find type in function call: " << arg->args[1]->command << std::endl;
                exit(1);
            }
        }
        else if (arg->command == "defmethod") {
            require_minimum_size(arg, 5);
            add_function(prog, arg->args[1]->command, find_type(prog, arg->args[2]->command), arg->args[3], arg->args[4], Function_Type::Method, false);
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

void analyze_expression(ProgPtr prog, ExPtr ex, FuncPtr func) {
    if (ex->args.size() == 0) {
        int var_id = find_var(prog, ex->command);
        if (var_id == -1) {
            std::cerr << "Variable not found: " << ex->command << std::endl;
            exit(1);
        }
        else {
            VarPtr var = prog->variables[var_id];
            if (ex->meta_id == -1) {
                MetaPtr meta(new Metadata());
                prog->metas.push_back(meta);
                ex->meta_id = prog->metas.size() - 1;
            }
            MetaPtr meta = prog->metas[ex->meta_id];
            meta->type_id = var->type_id;
            return;
        }
    }

    if (ex->command == "block") {
        for (unsigned int i = 0; i < ex->args.size(); ++i) {
            analyze_expression(prog, ex->args[i], func);
        }
    }
    else if (ex->command == "return") {
        if (ex->args.size() > 0) {
            analyze_expression(prog, ex->args[0], func);
            int meta_id = ex->args[0]->meta_id;
            if (meta_id == -1) {
                std::cerr << "Return missing typed value" << std::endl;
                exit(1);
            }

            MetaPtr meta = prog->metas[ex->args[0]->meta_id];
            if (meta->type_id != func->return_type) {
                std::cerr << "Return type does not match prototype: " << func->readable_name << std::endl;
                exit(1);
            }

        }
    }
    else if (ex->command == "+") {
        require_minimum_size(ex, 2);
        for (unsigned int i = 0; i < ex->args.size(); ++i) {
            analyze_expression(prog, ex->args[i], func);
        }

        if (ex->args[0]->meta_id == -1) {
            std::cerr << "Can not analyze: " << ex->args[0]->command << std::endl;
            exit(1);
        }

        MetaPtr meta = prog->metas[ex->args[0]->meta_id];
        if (meta->type_id == -1) {
            std::cerr << "Unknown type for: " << ex->args[0]->command << std::endl;
            exit(1);
        }

        int required_type_id = meta->type_id;
        for (unsigned int i = 1; i < ex->args.size(); ++i) {
            int arg_meta_id = ex->args[i]->meta_id;
            if (arg_meta_id == -1) {
                std::cerr << "Unknown metadata for: " << ex->args[i]->command << std::endl;
                exit(1);
            }
            MetaPtr arg_meta = prog->metas[ex->args[i]->meta_id];
            if (arg_meta->type_id != required_type_id) {
                std::cerr << "Mismatched types for: " << ex->args[i]->command << std::endl;
                exit(1);
            }
        }

        if (ex->meta_id == -1) {
            MetaPtr meta(new Metadata());
            prog->metas.push_back(meta);
            ex->meta_id = prog->metas.size() - 1;
        }
        meta = prog->metas[ex->meta_id];
        meta->type_id = required_type_id;

    }
}

void analyze_func_def_pass(ProgPtr prog) {
    for (unsigned int i = 0; i < prog->functions.size(); ++i) {
        analyze_expression(prog, prog->functions[i]->root, prog->functions[i]);
    }
}

void all_passes(ProgPtr prog, ExPtr ex) {
    analyze_meta_pass(prog, ex);
    analyze_type_decl_pass(prog, ex);
    analyze_type_def_pass(prog, ex);
    analyze_func_decl_pass(prog, ex);
    analyze_func_def_pass(prog);
}


