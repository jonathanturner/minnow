// This file is distributed under the BSD license.
// See LICENSE.TXT for details.
#include <stdio.h>

#include "common.h"
#include "concurrency.h"
#include "message.h"
#include "message_queue.h"

/*
 * Taken from Chapter 10 of Art of Multiprocessor Programming, Herlihy & Shavit
 *
 * Because each msg queue has one, and only one, reader, the ABA
 * problem is avoided.
 */

inline BOOL atomic_msg_cas(struct Message **orig, struct Message *cmp, struct Message *new) {
    return atomic_cas((void**)orig, (void*)cmp, (void*)new);
}

void enqueue_msg(struct Message_Queue *queue, struct Message *message) {
    while(TRUE) {
        struct Message *last = queue->tail;
        struct Message *next = last->next;

        if (last == queue->tail) {
            if (next == NULL) {
                if (atomic_msg_cas(&(last->next), next, message)) {
                    atomic_msg_cas(&(queue->tail), last, message);
                    return;
                }
                else {
                    atomic_msg_cas(&(queue->tail), last, next);
                }
            }
        }
    }
}

struct Message *dequeue_msg(struct Message_Queue *queue) {
    while (TRUE) {
        struct Message *first = queue->head;
        struct Message *last = queue->tail;
        struct Message *next = first->next;

        if (first == queue->head) {
            if (first == last) {
                if (next == NULL) {
                    //throw exception
                    return NULL;
                }
                atomic_msg_cas(&(queue->tail), last, next);
            }
            else {
                struct Message *retval = next;
                if (atomic_msg_cas(&(queue->head), first, next)) {
                    return retval;
                }
            }
        }
    }
}

