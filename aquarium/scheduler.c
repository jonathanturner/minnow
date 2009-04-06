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
    retval->is_running = TRUE;

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

    printf("push_bottom\n");
    printf("a: %p\n", a);
    printf("wq->actor_deq[0] = %p\n", wq->actor_deq[0]);
}

void push_bottom_actor_to_alt(void *scheduler, void *actor) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a = (struct Actor*) actor;
    int alt;
    struct Work_Queue *wq;

    if (s->which_active == 0)
        alt = 1;
    else
        alt = 0;

    wq = s->work_queues[alt];

    int local_bottom = wq->bot;
    wq->actor_deq[local_bottom] = a;
    ++local_bottom;
    wq->bot = local_bottom;
}

void scheduler_loop(void *scheduler) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a;

    while (s->is_running) {
        a = pop_bottom_actor(s->work_queues[s->which_active]);
        printf("a: %p\n", a);
        if (a == NULL) {
            if (s->which_active == 0)
                s->which_active = 1;
            else
                s->which_active = 0;

            exit(0);
            continue;
        }
        a->timeslice_remaining = TIMESLICE_SIZE;

        if (a != NULL) {
            struct Message *message = a->mail->head->next;
            message->scheduler = s;
            while ((a->timeslice_remaining > 0) && (message != NULL)) {
                if (message->task(message) == TASK_DONE) {
                    struct Message *m = dequeue_msg(a->mail);
                    free(m);
                    message = a->mail->head->next;
                }
                else {
                    break;
                }
            }
            if (message != NULL) {
                push_bottom_actor_to_alt(s, a);
            }
        }
    }

}
