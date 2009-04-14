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
    message->next = NULL;
    CBOOL is_first = CFALSE;

    lock_mutex(queue->mutex_lock);
    {
        if (queue->head == queue->tail) {
            is_first = CTRUE;
        }
        queue->tail->next = message;
        queue->tail = message;
    }
    unlock_mutex(queue->mutex_lock);

    return is_first;
}

 CBOOL dequeue_msg(struct Message_Queue *queue) {
    CBOOL retval = CFALSE;
    struct Message *delslot = NULL;

    lock_mutex(queue->mutex_lock);
    {
        if (queue->head->next != NULL) {
            delslot = queue->head;
            queue->head = queue->head->next;
            if (queue->head->next != NULL) {
                retval = CTRUE;
            }
        }
    }
    unlock_mutex(queue->mutex_lock);

    if (delslot != NULL)
        free(delslot);

    return retval;
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

    retval->mutex_lock = create_mutex();

    return retval;
}
