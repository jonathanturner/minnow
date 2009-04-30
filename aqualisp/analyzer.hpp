// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef ANALYZER_HPP_
#define ANALYZER_HPP_

#include <map>

#include "expression.hpp"

#define VarPtr std::tr1::shared_ptr<Variable>
#define TypePtr std::tr1::shared_ptr<Type>
#define FuncPtr std::tr1::shared_ptr<Function>
#define ProgPtr std::tr1::shared_ptr<Program>

struct Variable {
    std::string readable_name;
    int type;
    ExPtr root;
};

class Class_Type {
    public: enum Type {Actor, Basic};
};

class Function_Type {
    public: enum Type {Action, Function, Method};
};

struct Type {
    std::string readable_name;
    Class_Type::Type type;
    std::map<std::string, int> methods;
    std::map<std::string, int> attributes;
    ExPtr root;
};

struct Function {
    std::string readable_name;
    Function_Type::Type type;
    std::map<std::string, int> variables;
    std::vector<int> parameter_types;
    int return_type;
    ExPtr root;
    bool is_extern;
};

struct Program {
    std::vector<VarPtr> variables;
    std::vector<TypePtr> types;
    std::vector<FuncPtr> functions;
    std::vector<MetaPtr> metas;

    std::map<std::string, int > variable_lookup;
    std::map<std::string, int > type_lookup;
    std::map<std::string, int > function_lookup;
    std::map<int, int> meta_lookup;
};

void analyze_meta_pass(ProgPtr prog, ExPtr ex);
void analyze_type_decl_pass(ProgPtr prog, ExPtr ex);
void analyze_type_def_pass(ProgPtr prog, ExPtr ex);
void analyze_func_decl_pass(ProgPtr prog, ExPtr ex);
void analyze_func_def_pass(ProgPtr prog, ExPtr ex);

void all_passes(ProgPtr prog, ExPtr ex);

void debug_print(ProgPtr prog, VarPtr var, std::string prepend);
void debug_print(ProgPtr prog, TypePtr type, std::string prepend);
void debug_print(ProgPtr prog, FuncPtr func, std::string prepend);
void debug_print(ProgPtr prog);


#endif /* ANALYZER_HPP_ */
