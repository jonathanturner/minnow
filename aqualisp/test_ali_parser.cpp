// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <vector>
#include <iostream>

#include "parser.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> data;

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

    ExPtr exp;
    std::vector<std::string>::iterator iter, end;

    iter = data.begin();
    end = data.end();

    exp = parse(iter, end);

    debug_print(exp, "");

    return 0;
}
