// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <iostream>
#include <stdlib.h>

#include "utils.hpp"
#include "parser.hpp"
#include "expression.hpp"
#include "analyzer.hpp"

int main(int argc, char *argv[]) {
    std::string d;

    if (argc < 2) {
       std::cerr << "Please pass in a test .ali file" << std::endl;
       exit(1);
    }
    else {
       std::cout << "Loading: " << argv[1] << std::endl;
       d = load_file(argv[1]);
    }
    std::vector<std::string> data = lex(d);
    //debug_print(data);

    ExPtr exp;
    std::vector<std::string>::iterator iter, end;

    iter = data.begin();
    end = data.end();

    exp = parse(iter, end);

    debug_print(exp, "");

    ProgPtr prog(new Program());
    analyze_type_decl_pass(exp, prog);

    debug_print(prog);
}
