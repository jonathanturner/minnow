// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "common.h"
#include "actor.h"
#include "work_queue.h"

#define DEQUEUE_SIZE 100000

struct Scheduler {
    struct Work_Queue *work_queues[2];
    int which_active;
};

struct Scheduler *create_scheduler();

//public API
void push_bottom_actor(void *scheduler, void *actor);


#endif /* SCHEDULER_H_ */
