// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <map>

#include "analyzer.hpp"
#include "expression.hpp"

#define VarPtr std::tr1::shared_ptr<Variable>
#define TypePtr std::tr1::shared_ptr<Type>
#define FuncPtr std::tr1::shared_ptr<Function>

class Variable {
    std::string readable_name;
    int type;
};

class Type {
    std::string readable_name;
    std::map<std::string, int> methods;
    std::map<std::string, int> attributes;
};

class Function {
    std::string readable_name;
    std::map<std::string, int> variables;
    std::vector<int> parameter_types;
};

class Program {
    std::vector<VarPtr> variables;
    std::vector<TypePtr> types;
    std::vector<FuncPtr> functions;
};
