// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef EXPRESSION_HPP_
#define EXPRESSION_HPP_

#include <string>
#include <vector>
#include <tr1/memory>

#define ExPtr std::tr1::shared_ptr<Expression>
#define MetaPtr std::tr1::shared_ptr<Metadata>

struct Position {
    int line;
    int column;
};

struct Metadata {
    std::string filename;
    struct Position start_pos;
    struct Position end_pos;
    int definition_id;
    int type_id;
    std::string orig_name;
};

struct Expression {
    std::string command;
    std::vector<ExPtr > args;
    int meta_id;
};

ExPtr create_expression();

#endif /* EXPRESSION_HPP_ */
