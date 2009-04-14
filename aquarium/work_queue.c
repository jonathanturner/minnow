// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

// Taken from "Thread Scheduler for Multiprogrammed Multiprocessors"
// by Arora, Blumofe, and Plaxton

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "work_queue.h"

void enqueue_actor(struct Work_Queue *work_queue, struct Actor *actor) {
    actor->next = NULL;

    lock_mutex(work_queue->mutex_lock);
    {
        work_queue->tail->next = actor;
        work_queue->tail = actor;
    }
    unlock_mutex(work_queue->mutex_lock);
}

struct Actor *dequeue_actor(struct Work_Queue *work_queue) {
    struct Actor *retval = NULL;

    lock_mutex(work_queue->mutex_lock);
    {
        if (work_queue->head->next != NULL) {
            retval = work_queue->head->next;
            if (retval == work_queue->tail) {
                work_queue->tail = work_queue->head;
            }
            work_queue->head->next = work_queue->head->next->next;
        }
    }
    unlock_mutex(work_queue->mutex_lock);
    return retval;
}

CBOOL is_empty(struct Work_Queue *work_queue) {
    return (work_queue->head == NULL);
}

struct Work_Queue *create_work_queue() {
    struct Work_Queue *retval = (struct Work_Queue*)malloc(sizeof(struct Work_Queue));

    struct Actor *guard = create_actor();
    retval->head = guard;
    retval->tail = guard;

    retval->mutex_lock = create_mutex();

    return retval;
}
