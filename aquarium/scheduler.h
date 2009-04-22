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
#define MAX_MSG_CACHE 10000

struct Scheduler {
    CBOOL is_running;

    struct Scheduler **schedulers;
    int num_schedulers;

    struct Work_Queue *work_queue;

    volatile int idle_count;

    struct Message *msg_cache;
    int num_cache_msgs;
};

struct Scheduler *create_scheduler();
void *scheduler_loop(void *scheduler);

void msg_actor(void *scheduler, void *actor, void *msg);
struct Scheduler *create_all_schedulers(int count);

struct Message *get_free_message(void *scheduler);
void recycle_message(void *scheduler, void *msg);

#endif /* SCHEDULER_H_ */
