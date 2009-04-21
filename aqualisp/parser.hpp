// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <vector>

#include "expression.hpp"

std::vector<std::string> lex(std::string &s);

ExPtr parse(std::vector<std::string>::iterator &iter, std::vector<std::string>::iterator &end);

void debug_print(const ExPtr root, std::string prepend);
void debug_print(const std::vector<std::string> &tokens);

#endif /* PARSER_HPP_ */
