#include <iostream>
#include <fstream>
#include <vector>
#include <tr1/memory>

#include <ctype.h>

struct Expression {
    std::string token;
    std::vector<std::tr1::shared_ptr<Expression> > args;
};

void debug_print(const std::tr1::shared_ptr<Expression> root, std::string prepend) {
    std::cout << prepend << root->token << std::endl;
    for (unsigned int i = 0; i < root->args.size(); ++i) {
        debug_print(root->args[i], prepend+" ");
    }
}

void debug_print(const std::vector<std::string> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i] << " ";
    }
}

template<typename InItr>
std::tr1::shared_ptr<Expression> parse(InItr &iter, InItr &end) {
   std::tr1::shared_ptr<Expression> p(new Expression);

   if (iter == end) {
       return std::tr1::shared_ptr<Expression>();
   }

   if (*iter == "(") {
       ++iter;
       p->token = *iter;
       ++iter;
       while ((iter != end) && (*iter != ")"))  {
           std::tr1::shared_ptr<Expression> exp = parse(iter, end);
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
           std::tr1::shared_ptr<Expression> exp = parse(iter, end);
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

std::string load_file(const char *filename) {
    std::ifstream infile (filename, std::ios::in | std::ios::ate);

    if (!infile.is_open()) {
        std::cerr << "Can not open " << filename << std::endl;
        exit(0);
    }

    std::streampos size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::cout << "File size: " << size << std::endl;

    std::vector<char> v(size);
    infile.read(&v[0], size);

    std::string ret_val (v.empty() ? std::string() : std::string (v.begin(), v.end()).c_str());

    return ret_val;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> data;

    /*
    data.push_back("(");
    data.push_back("+");
    data.push_back("11");
    //data.push_back("234");
    data.push_back("(");
    data.push_back("inc");
    data.push_back("3");
    data.push_back(")");
    data.push_back("10");

    data.push_back(")");
    */


    std::string d;

    if (argc < 2) {
       d  = "(+ 41 (inc [1 2])10)";
    }
    else {
       std::cout << "Loading: " << argv[1] << std::endl;
       d = load_file(argv[1]);
    }
    data = lex(d);
    //debug_print(data);

    std::tr1::shared_ptr<Expression> exp;
    std::vector<std::string>::iterator iter, end;

    iter = data.begin();
    end = data.end();

    exp = parse(iter, end);

    debug_print(exp, "");

    return 0;
}

