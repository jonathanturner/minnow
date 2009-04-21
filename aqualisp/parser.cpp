#include <iostream>
#include <vector>

#include <ctype.h>

#include "expression.hpp"
#include "parser.hpp"

void debug_print(const ExPtr root, std::string prepend) {
    if (root->args.size() > 0) {
        std::cout << prepend << '{' << root->token << '}' << std::endl;
    }
    else {
        std::cout << prepend << root->token << std::endl;
    }
    for (unsigned int i = 0; i < root->args.size(); ++i) {
        debug_print(root->args[i], prepend+" ");
    }
}

void debug_print(const std::vector<std::string> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i] << " ";
    }
}

std::vector<std::string> lex(std::string &s) {
    std::vector<std::string> ret_val;
    std::string::iterator iter = s.begin(), end = s.end(), leading;

    ret_val.push_back("(");
    ret_val.push_back("prog");

    while (iter != end) {
        if (*iter == '(') {
            ret_val.push_back("(");
            ++iter;
        }
        else if (*iter == ')') {
            ret_val.push_back(")");
            ++iter;
        }
        else if (*iter == '[') {
            ret_val.push_back("[");
            ++iter;
        }
        else if (*iter == ']') {
            ret_val.push_back("]");
            ++iter;
        }
        else if (isspace(*iter)) {
            ++iter;
        }
        else if (*iter == '"') {
            leading = iter;
            ++iter;
            while ((iter != end) && (*iter != '"')) {
                ++iter;
            }
            if (iter != end) ++iter;
            ret_val.push_back(std::string(leading, iter));
        }
        else {
            leading = iter;
            while ((iter != end) && (*iter != ')') && (*iter != '(') && (*iter != ']') && (*iter != '[')&& (!isspace(*iter))) {
                ++iter;
            }

            ret_val.push_back(std::string(leading, iter));
        }
    }

    ret_val.push_back(")");

    return ret_val;
}

ExPtr parse(std::vector<std::string>::iterator &iter, std::vector<std::string>::iterator &end) {
   ExPtr p(new Expression);

   if (iter == end) {
       return ExPtr();
   }

   if (*iter == "(") {
       ++iter;
       p->token = *iter;
       ++iter;
       while ((iter != end) && (*iter != ")"))  {
           ExPtr exp = parse(iter, end);
           if (exp.get() != NULL) {
               p->args.push_back(exp);
           }
       }
       if (iter != end) ++iter;
       return p;
   }

   else if (*iter == "[") {
       p->token = "list";
       ++iter;
       while ((iter != end) && (*iter != "]"))  {
           ExPtr exp = parse(iter, end);
           if (exp.get() != NULL) {
               p->args.push_back(exp);
           }
       }
       if (iter != end) ++iter;
       return p;
   }
   else {
       p->token = *iter;
       ++iter;
       return p;
   }
}




