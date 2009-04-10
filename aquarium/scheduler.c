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
    retval->idle_count = 0;
    retval->is_running = CTRUE;
    retval->cache_msg = NULL;

    return retval;
}

// Taken from "Thread Scheduler for Multiprogrammed Multiprocessors"
// by Arora, Blumofe, and Plaxton

//public API
void push_bottom_actor(void *scheduler, void *actor) {
    struct Scheduler *s = (struct Scheduler*)scheduler;
    struct Actor *a = (struct Actor*) actor;
    struct Work_Queue *wq = s->work_queues[s->which_active];

    //int local_bottom = wq->bot;
    wq->actor_deq[wq->bot] = a;
    ++wq->bot;
    //++local_bottom;
    //wq->bot = local_bottom;
    //atomic_cas_int(&(wq->bot), wq->bot, local_bottom);
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

    //int local_bottom = wq->bot;
    wq->actor_deq[wq->bot] = a;
    ++wq->bot;
    //++local_bottom;
    //wq->bot = local_bottom;
    //atomic_cas_int(&(wq->bot), wq->bot, local_bottom);
}

struct Actor *steal_actor(struct Scheduler *scheduler) {
    struct Actor *retval = NULL;

    //todo: change victim picking to pseudo random, or else we'll kill ourselves over the first guy in the list
    int i;
    for (i = 0; i < scheduler->num_schedulers; ++i) {
        struct Scheduler *victim = scheduler->schedulers[i];

        int j;
        //When we try our victim, we try both of its work queues so, in essence, we are actually stealing from either "side".
        for (j = 0; j < 2; ++j) {
            //if ((victim->work_queues[j]->bot - victim->work_queues[j]->age.Packed.top) >= 1) {
            if (!is_empty(victim->work_queues[j])) {
                retval = pop_top_actor(victim->work_queues[j]);
                if (retval != NULL) {
                    return retval;
                }
            }
        }
    }

    return NULL;
}

void take_actor_if_idle(struct Scheduler *s, struct Actor *a) {
    while (1) {
        switch (a->actor_state) {
            case (ACTOR_STATE_IDLE) : {
                if (atomic_cas_int(&(a->actor_state), ACTOR_STATE_IDLE, ACTOR_STATE_SCHEDULED)) {
                    //successfully taken
                    atomic_cas_int(&(a->actor_state), ACTOR_STATE_SCHEDULED, ACTOR_STATE_MSG);
                    a->scheduler = s;
                    push_bottom_actor(s, a);
                    return;
                }
                /*
                else {
                    if (atomic_cas_int(&(a->actor_state), ACTOR_STATE_SCHEDULED, ACTOR_STATE_MSG)) {
                        return;
                    }
                }
                */
            }
            break;
            case (ACTOR_STATE_SCHEDULED) : {
                if (atomic_cas_int(&(a->actor_state), ACTOR_STATE_SCHEDULED, ACTOR_STATE_MSG)) {
                    return;
                }
            }
            break;
            default : {
                return;
            }
        }
    }
}

void msg_actor(void *scheduler, void *actor, void *msg) {
    struct Scheduler *s = (struct Scheduler *)scheduler;
    struct Actor *a = (struct Actor *)actor;
    struct Message *m = (struct Message *)msg;

    enqueue_msg(a->mail, m);
    take_actor_if_idle(s, a);
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
        a = pop_bottom_actor(s->work_queues[s->which_active]);

        if (a == NULL) {
            //if ((s->work_queues[0]->bot == s->work_queues[0]->age.Packed.top) &&
            //        (s->work_queues[1]->bot == s->work_queues[1]->age.Packed.top)) {
            if ((is_empty(s->work_queues[0])) && (is_empty(s->work_queues[1]))) {
            //if ((s->work_queues[0]->bot == 0) &&
            //        (s->work_queues[1]->bot == 0)) {
                while (a == NULL) {
                    a = steal_actor(s);
                    if (a != NULL) {
                        a->scheduler = s;
                        s->idle_count = 0;
                        //printf("+++ Steal %p successful for %p +++\n", a, s);
                        break;
                    }
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
            else {
                if (s->which_active == 0)
                    s->which_active = 1;
                else
                    s->which_active = 0;

                continue;
            }
        }
        else {
            s->idle_count = 0;
        }

        a->timeslice_remaining = TIMESLICE_SIZE;

        struct Message *message = a->mail->head->next;
        while ((a->timeslice_remaining > 0) && (message != NULL)) {
            //message->scheduler = s;

            if (message->task(message)) {
                dequeue_msg(a->mail);
                message = a->mail->head->next;
                if (message == NULL) {
                    atomic_cas_int(&(a->actor_state), ACTOR_STATE_MSG, ACTOR_STATE_SCHEDULED);
                }
            }
            else {
                break;
            }
        }
        if (a->mail->head->next != NULL) {
            push_bottom_actor_to_alt(s, a);
        }
        else {
            if (!atomic_cas_int(&(a->actor_state), ACTOR_STATE_SCHEDULED, ACTOR_STATE_IDLE)) {
                printf("FAILED TO CAS: %p state: %i sched: %p  me: %p\n", a, a->actor_state, a->scheduler, s);

                push_bottom_actor_to_alt(s, a);
            }
        }
    }

    return NULL;
}


