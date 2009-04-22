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
    public: enum Type {Actor, Struct};
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
    std::map<std::string, int> variables;
    std::vector<int> parameter_types;
    int return_type;
    ExPtr root;
};

struct Program {
    std::vector<VarPtr> variables;
    std::vector<TypePtr> types;
    std::vector<FuncPtr> functions;

    std::map<std::string, int> variable_lookup;
    std::map<std::string, int> type_lookup;
    std::map<std::string, int> function_lookup;

};

void analyze_type_decl_pass(ExPtr ex, ProgPtr prog);
void analyze_type_def_pass(ExPtr ex, ProgPtr prog);
void analyze_func_decl_pass(ExPtr ex, ProgPtr prog);
void analyze_func_def_pass(ExPtr ex, ProgPtr prog);

void debug_print(VarPtr var);
void debug_print(TypePtr type);
void debug_print(FuncPtr func);
void debug_print(ProgPtr prog);


#endif /* ANALYZER_HPP_ */