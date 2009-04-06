// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "common.h"

#define TASK_DONE       1
#define TASK_INCOMPLETE 2

struct Message {
    Type_Union args[8];
    int (*task)(struct Message *msg);
    void *recipient;
    void *scheduler;
    struct Message *next;
};

struct Message *create_message();

#endif /* MESSAGE_H_ */
