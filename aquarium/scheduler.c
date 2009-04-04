// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>

#include "common.h"
#include "concurrency.h"
#include "scheduler.h"
#include "work_queue.h"

struct Scheduler *create_scheduler() {
    struct Scheduler *retval = (struct Scheduler*)malloc(sizeof(struct Scheduler));
    int i;

    for (i = 0; i < 2; ++i) {
        retval->work_queues[i] = create_work_queue();
    }

    retval->which_active = 0;

    return retval;
}

// Taken from "Thread Scheduler for Multiprogrammed Multiprocessors"
// by Arora, Blumofe, and Plaxton

//public API
void push_bottom_actor(void *scheduler, void *actor) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a = (struct Actor*) actor;
    struct Work_Queue *wq = s->work_queues[s->which_active];

    int local_bottom = wq->bot;
    wq->actor_deq[local_bottom] = a;
    ++local_bottom;
    wq->bot = local_bottom;
}
