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
    ProgPtr prog(new Program());

    std::string prelude = "(deftype int32)(deftype uint32)(deftype float)(deftype double)(deftype Array__string)(defactor __Program__)";

    if (argc < 2) {
       std::cerr << "Please pass in a test .ali file" << std::endl;
       exit(1);
    }
    else {
       std::cout << "Loading: " << argv[1] << std::endl;
       d = "(prog " + prelude + load_file(argv[1]) + ")";
    }
    std::vector<std::string> data = lex(d);
    //debug_print(data);

    ExPtr ex;
    std::vector<std::string>::iterator iter, end;

    iter = data.begin();
    end = data.end();

    ex = parse(iter, end);
    all_passes(prog, ex);

    debug_print(ex, "");
    debug_print(prog);
}
