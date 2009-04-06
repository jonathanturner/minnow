// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

// Taken from "Thread Scheduler for Multiprogrammed Multiprocessors"
// by Arora, Blumofe, and Plaxton

#include <stdlib.h>

#include "concurrency.h"
#include "work_queue.h"

struct Actor *pop_top_actor(struct Work_Queue *work_queue) {
    union Age old_age = work_queue->age;
    int local_bottom = work_queue->bot;
    if (local_bottom <= old_age.Packed.top) {
        return NULL;
    }
    struct Actor *retval = work_queue->actor_deq[old_age.Packed.top];
    union Age new_age = old_age;
    ++new_age.Packed.top;
    atomic_cas_int(&(work_queue->age.Int), old_age.Int, new_age.Int);
    if (old_age.Int == new_age.Int) {
        return retval;
    }
    else {
        //todo: this should be ABORT
        return NULL;
    }
}

struct Actor *pop_bottom_actor(struct Work_Queue *work_queue) {
    int local_bottom = work_queue->bot;
    if (local_bottom == 0) {
        return NULL;
    }
    --local_bottom;
    work_queue->bot = local_bottom;
    struct Actor *retval = work_queue->actor_deq[local_bottom];
    union Age old_age = work_queue->age;
    if (local_bottom > old_age.Packed.top) {
        return retval;
    }
    work_queue->bot = 0;
    union Age new_age;
    new_age.Packed.top = 0;
    new_age.Packed.tag = old_age.Packed.tag + 1;
    if (local_bottom == old_age.Packed.top) {
        atomic_cas_int(&(work_queue->age.Int), old_age.Int, new_age.Int);
        if (old_age.Int == new_age.Int) {
            return retval;
        }
    }
    work_queue->age = new_age;
    return NULL;
}

struct Work_Queue *create_work_queue() {
    struct Work_Queue *retval = (struct Work_Queue*)malloc(sizeof(struct Work_Queue));
    int i;

    for (i = 0; i < DEQUEUE_SIZE; ++i) {
        retval->actor_deq[i] = NULL;
    }
    retval->age.Int = 0;

    retval->bot = 0;

    return retval;
}