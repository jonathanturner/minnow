// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "common.h"
#include "actor.h"
#include "work_queue.h"

#define DEQUEUE_SIZE 100000
#define TIMESLICE_SIZE 2000

struct Scheduler {
    BOOL is_running;

    struct Scheduler **schedulers;
    int num_schedulers;

    struct Work_Queue *work_queues[2];
    int which_active;
};

struct Scheduler *create_scheduler();
void scheduler_loop(void *scheduler);
void push_bottom_actor_to_alt(void *scheduler, void *actor);

//public API
void push_bottom_actor(void *scheduler, void *actor);
void msg_actor(void *scheduler, void *actor, void *msg);

#endif /* SCHEDULER_H_ */
