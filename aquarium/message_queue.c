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

inline BOOL atomic_msg_cas(struct Message **orig, struct Message *cmp, struct Message *new) {
    return atomic_cas((void**)orig, (void*)cmp, (void*)new);
}

void enqueue_msg(struct Message_Queue *queue, struct Message *message) {
    printf("queue->tail = %p\n", queue->tail);

    while(TRUE) {
        volatile struct Message *last = queue->tail;
        struct Message *next = last->next;

        printf("queue->tail = %p\n", queue->tail);

        printf("tail: %p  last: %p  next: %p\n", queue->tail, last, last->next);
        if (last == queue->tail) {
            if (next == NULL) {
                printf("Comparing: %p to %p\n", last->next, next);
                if (atomic_msg_cas(&(last->next), next, message)) {
                    printf("Comparing2: %p to %p\n", queue->tail, last);
                    atomic_msg_cas(&(queue->tail), last, message);
                    return;
                }
            }
            else {
                printf("Comparing3: %p to %p\n", queue->tail, last);
                atomic_msg_cas(&(queue->tail), last, next);
            }
        }
    }
}

struct Message *dequeue_msg(struct Message_Queue *queue) {
    while (TRUE) {
        volatile struct Message *first = queue->head;
        volatile struct Message *last = queue->tail;
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
