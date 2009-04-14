// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef WORK_QUEUE_H_
#define WORK_QUEUE_H_

#include "actor.h"
#include "concurrency.h"

#define DEQUEUE_SIZE 100000

union Age {
    struct {
        int top:17;
        int tag:15;
    } Packed __attribute__((__packed__));
    int Int;
};

struct Work_Queue {
    struct Actor *head;
    struct Actor *tail;

    void *mutex_lock;
};

void enqueue_actor(struct Work_Queue *work_queue, struct Actor *actor);
struct Actor *dequeue_actor(struct Work_Queue *work_queue);
CBOOL is_empty(struct Work_Queue *work_queue);

struct Work_Queue *create_work_queue();

#endif /* WORK_QUEUE_H_ */
