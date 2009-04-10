// This file is distributed under the BSD license.
// See LICENSE.TXT for details.
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "concurrency.h"
#include "message.h"
#include "message_queue.h"

/*
 * Taken from Chapter 10 of Art of Multiprocessor Programming, Herlihy & Shavit
 *
 * Because each msg queue has one reader the ABA
 * problem is avoided.
 */

inline CBOOL atomic_msg_cas(struct Message **orig, struct Message *cmp, struct Message *new) {
    return atomic_cas((void**)orig, (void*)cmp, (void*)new);
}

/**
 * Messages the actor, and returns whether or not the message was the first msg to the actor
 */
CBOOL enqueue_msg(struct Message_Queue *queue, struct Message *message) {
    CBOOL retval;

    if (queue->head->next == NULL)
        retval = CTRUE;
    else
        retval = CFALSE;

    while(CTRUE) {
        volatile struct Message *last = queue->tail;
        struct Message *next = last->next;

        if (last == queue->tail) {
            if (next == NULL) {
                if (atomic_msg_cas(&(last->next), next, message)) {
                    atomic_msg_cas(&(queue->tail), last, message);
                    return retval;
                }
            }
            else {
                atomic_msg_cas(&(queue->tail), last, next);
            }
        }
        retval = CFALSE;
    }
}

struct Message *dequeue_msg(struct Message_Queue *queue) {

    while (CTRUE) {
        volatile struct Message *first = queue->head;
        volatile struct Message *last = queue->tail;
        struct Message *next = first->next;

        if (first == queue->head) {
            if (first == last) {
                if (next == NULL) {
                    return NULL;
                }
                atomic_msg_cas(&(queue->tail), last, next);
            }
            else {
                struct Message *retval = next;
                struct Message *delslot = queue->head;
                if (atomic_msg_cas(&(queue->head), first, next)) {
                    free(first);
                    return retval;
                }
            }
        }
    }
}

struct Message_Queue *create_message_queue() {
    struct Message_Queue *retval = (struct Message_Queue *)malloc(sizeof(struct Message_Queue));

    if (retval == NULL) {
        //todo: throw exception
        printf("Could not allocate space for message queue, exiting.\n");
        exit(1);
    }

    struct Message *guard = create_message();
    retval->head = guard;
    retval->tail = guard;

    return retval;
}
