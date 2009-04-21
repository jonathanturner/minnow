// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef EXPRESSION_HPP_
#define EXPRESSION_HPP_

#include <tr1/memory>

#define ExPtr std::tr1::shared_ptr<Expression>

struct Expression {
    std::string token;
    std::vector<ExPtr > args;
};


#endif /* EXPRESSION_HPP_ */
