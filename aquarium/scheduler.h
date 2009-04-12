// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "common.h"
#include "actor.h"
#include "work_queue.h"

#define DEQUEUE_SIZE 100000
#define TIMESLICE_SIZE 2000
#define MAX_IDLE_ITERS 10

struct Scheduler {
    CBOOL is_running;

    struct Scheduler **schedulers;
    int num_schedulers;

    struct Work_Queue *work_queue;

    volatile int idle_count;

    struct Message *cache_msg;
};

struct Scheduler *create_scheduler();
void *scheduler_loop(void *scheduler);

void msg_actor(void *scheduler, void *actor, void *msg);
struct Scheduler *create_all_schedulers(int count);

#endif /* SCHEDULER_H_ */
