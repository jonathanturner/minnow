// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef EXPRESSION_HPP_
#define EXPRESSION_HPP_

#include <string>
#include <vector>
#include <tr1/memory>

#define ExPtr std::tr1::shared_ptr<Expression>

struct Expression {
    std::string command;
    std::vector<ExPtr > args;
};


#endif /* EXPRESSION_HPP_ */
