// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>

#include "common.h"
#include "concurrency.h"
#include "scheduler.h"
#include "work_queue.h"

struct Scheduler *create_scheduler() {
    struct Scheduler *retval = (struct Scheduler*)malloc(sizeof(struct Scheduler));

    retval->work_queue = create_work_queue();

    retval->idle_count = 0;
    retval->is_running = CTRUE;
    retval->msg_cache = NULL;
    retval->num_cache_msgs = 0;

    return retval;
}

struct Actor *steal_actor(struct Scheduler *scheduler) {
    struct Actor *retval = NULL;

    //todo: change victim picking to pseudo random, or else we'll kill ourselves over the first guy in the list
    int i;
    for (i = 0; i < scheduler->num_schedulers; ++i) {
        struct Scheduler *victim = scheduler->schedulers[i];

        //When we try our victim, we try both of its work queues so, in essence, we are actually stealing from either "side".
        if (!is_empty(victim->work_queue)) {
            retval = dequeue_actor(victim->work_queue);
            //printf("[%p]", retval);
            if (retval != NULL) {
                return retval;
            }
        }
    }

    return NULL;
}

void msg_actor(void *scheduler, void *actor, void *msg) {
    struct Scheduler *s = (struct Scheduler *)scheduler;
    struct Actor *a = (struct Actor *)actor;
    struct Message *m = (struct Message *)msg;

    if (enqueue_msg(a->mail, m)) {
        //printf("Actor %p stolen on msging by %p\n", a, s);
        a->scheduler = s;
        enqueue_actor(s->work_queue, a);
    }
}

struct Message *get_free_message(void *scheduler) {
    struct Scheduler *s = (struct Scheduler *)scheduler;

    if (s->msg_cache == NULL) {
        return create_message();
    }
    else {
        struct Message *m = s->msg_cache;
        s->msg_cache = s->msg_cache->next;
        --s->num_cache_msgs;
        m->next = NULL;
        m->recipient = NULL;
        return m;
    }
}

void recycle_message(void *scheduler, void *msg){
    struct Scheduler *s = (struct Scheduler *)scheduler;
    struct Message *m = (struct Message *)msg;

    //if (s->num_cache_msgs < MAX_MSG_CACHE) {
        m->next = s->msg_cache;
        s->msg_cache = m;
        ++s->num_cache_msgs;
    /*}
    else {
        free(msg);
    }*/
}

CBOOL check_for_all_schedulers_idle(struct Scheduler *scheduler) {
    int i;
    CBOOL retval = CTRUE;
    for (i = 0; i < scheduler->num_schedulers; ++i) {
        if (scheduler->schedulers[i]->idle_count <= MAX_IDLE_ITERS) {
            retval = CFALSE;
        }
    }
    return retval;
}

struct Scheduler *create_all_schedulers(int count) {
    if (count < 1) {
        printf("Number of schedulers must be at least one.\n");
        exit(1);
    }
    struct Scheduler **s = (struct Scheduler **)malloc(sizeof(struct Scheduler *) * count);

    int i;
    for (i = 0; i < count; ++i) {
        s[i] = create_scheduler();
        //printf("Created scheduler: %p\n", s[i]);
    }

    for (i = 0; i < count; ++i) {
        s[i]->schedulers = s;
        s[i]->num_schedulers = count;
    }

    return s[0];
}

void *scheduler_loop(void *scheduler) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a = NULL;

    while (s->is_running) {
        //a = pop_bottom_actor(s->work_queues[s->which_active]);
        a = dequeue_actor(s->work_queue);
        //printf("Scheduler: %p  activating actor: %p\n", s, a);

        while (a == NULL) {
            a = steal_actor(s);
            if (a != NULL) {
                a->scheduler = s;
                s->idle_count = 0;
                //printf("+++ Steal %p successful for %p +++\n", a, s);
            }
            else {
                //printf("--- Steal failed for %p ---\n", s);
                sleep_in_ms(15);
                ++s->idle_count;
                //arbitrary count limit
                if (s->idle_count > MAX_IDLE_ITERS) {
                    if (check_for_all_schedulers_idle(s)) {
                        printf("DEBUG:System idle, exiting.\n");
                        exit(0);
                    }
                }
            }
        }

        s->idle_count = 0;
        a->timeslice_remaining = TIMESLICE_SIZE;

        struct Message *message = a->mail->head->next;

        while ((a->timeslice_remaining > 0) && (message != NULL)) {
            if (message->task(message)) {
                struct Message *prev_head = a->mail->head;
                if (dequeue_msg(a->mail)) {
                    //enqueue_actor(s->work_queue, a);
                    message = a->mail->head->next;
                }
                else {
                    message = NULL;
                }
                if (prev_head != a->mail->head) {
                    //message is complete, so recycle it
                    recycle_message(s, prev_head);
                }
            }
            else {
                enqueue_actor(s->work_queue, a);
                message = NULL;
            }
        }
        if (message != NULL) {
            enqueue_actor(s->work_queue, a);
        }
    }

    return NULL;
}


