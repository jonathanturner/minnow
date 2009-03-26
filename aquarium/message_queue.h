// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

#include "message.h"

struct Message_Queue {
    struct Message *head;
    struct Message *tail;
};

void enqueue_msg(struct Message_Queue *queue, struct Message *message);
struct Message *dequeue_msg(struct Message_Queue *queue);

#endif /* MESSAGE_QUEUE_H_ */
