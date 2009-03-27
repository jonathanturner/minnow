// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>

#include "common.h"
#include "concurrency.h"
#include "scheduler.h"

struct Scheduler *create_scheduler() {
    struct Scheduler *retval = (struct Scheduler*)malloc(sizeof(struct Scheduler));
    int i;

    for (i = 0; i < DEQUEUE_SIZE; ++i) {
        retval->actor_deq[i] = NULL;
    }
    retval->age.Int = 0;

    retval->bot = 0;

    return retval;
}

// Taken from "Thread Scheduler for Multiprogrammed Multiprocessors"
// by Arora, Blumofe, and Plaxton

struct Actor *pop_top_actor(struct Scheduler *scheduler) {
    union Age old_age = scheduler->age;
    int local_bottom = scheduler->bot;
    if (local_bottom <= old_age.Packed.top) {
        return NULL;
    }
    struct Actor *retval = scheduler->actor_deq[old_age.Packed.top];
    union Age new_age = old_age;
    ++new_age.Packed.top;
    atomic_cas_int(&(scheduler->age.Int), old_age.Int, new_age.Int);
    if (old_age.Int == new_age.Int) {
        return retval;
    }
    else {
        //todo: this should be ABORT
        return NULL;
    }
}

struct Actor *pop_bottom_actor(struct Scheduler *scheduler) {
    int local_bottom = scheduler->bot;
    if (local_bottom == 0) {
        return NULL;
    }
    --local_bottom;
    scheduler->bot = local_bottom;
    struct Actor *retval = scheduler->actor_deq[local_bottom];
    union Age old_age = scheduler->age;
    if (local_bottom > old_age.Packed.top) {
        return retval;
    }
    scheduler->bot = 0;
    union Age new_age;
    new_age.Packed.top = 0;
    new_age.Packed.tag = old_age.Packed.tag + 1;
    if (local_bottom == old_age.Packed.top) {
        atomic_cas_int(&(scheduler->age.Int), old_age.Int, new_age.Int);
        if (old_age.Int == new_age.Int) {
            return retval;
        }
    }
    scheduler->age = new_age;
    return NULL;
}

//public API
void push_bottom_actor(void *scheduler, void *actor) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a = (struct Actor*) actor;
    int local_bottom = s->bot;
    s->actor_deq[local_bottom] = a;
    ++local_bottom;
    s->bot = local_bottom;
}
