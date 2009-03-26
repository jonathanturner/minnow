// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "common.h"

struct Message {
    Type_Union args[8];
    void *recipient;
    struct Message *next;
};

#endif /* MESSAGE_H_ */
