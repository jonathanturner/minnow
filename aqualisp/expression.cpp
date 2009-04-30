// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include "expression.hpp"

ExPtr create_expression() {
    ExPtr retval(new Expression());

    retval->meta_id = -1;

    return retval;
}
