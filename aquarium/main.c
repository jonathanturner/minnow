// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

struct Counter {
    struct Actor actor;

    int id;
    //int base_count;
};

volatile int total_done;

void safe_inc() {
    while (!atomic_cas_int(&total_done, total_done, total_done+1));
}

void test_task(struct Message *v) {
    struct Message *message = (struct Message *)v;
    struct Counter *c = (struct Counter *)message->recipient;
    int base_count = 0;
    while (1) {
        base_count += 10;
        if (base_count >= 10000) {
            safe_inc();
            printf("Done\n");

            //if (total_done == 3) {
            //    exit(0);
            //}
            c->actor.actor_state = ACTOR_STATE_IDLE;
            return;
        }
        sleep_in_ms(5);
        if ((base_count % 1000) == 0) {
            printf("==== %i in actor %i in %p =====\n", base_count, c->id, c->actor.scheduler);
        }
    }

}

struct Counter *create_counter() {
    struct Counter *retval = (struct Counter *)malloc(sizeof(struct Counter));
    initialize_actor(&(retval->actor));

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
        printf("Created scheduler: %p\n", s[i]);
    }

    for (i = 0; i < count; ++i) {
        s[i]->schedulers = s;
        s[i]->num_schedulers = count;
    }

    return s[0];
}

int main() {
    printf("Hello: %i\n", num_hw_threads());

    struct Scheduler *s = create_all_schedulers(num_hw_threads());

    int i;
    total_done = 0;

    for (i = 0; i < 10; ++i) {
        struct Counter *c = create_counter();
        c->id = i;

        struct Message *m = create_message();
        m->task = test_task;
        m->recipient = c;

        msg_actor(s, c, m);
    }

    for (i = 1; i < s->num_schedulers; ++i) {
        create_thread(scheduler_loop, s->schedulers[i]);
    }

    scheduler_loop(s);

    return 0;
}

/*
#define THREADRING_SIZE 503

struct Passer {
    struct Actor actor;

    int id;
    struct Passer *next;
};

struct Passer *create_passer() {
    struct Passer *retval = (struct Passer *)malloc(sizeof(struct Passer));
    initialize_actor(&(retval->actor));
    retval->next = NULL;

    return retval;
}

int msg_pass(struct Message *message) {
    struct Scheduler *s = (struct Scheduler *)message->scheduler;
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    int token = message->args[0].Int32;

    if (token == 0) {
        printf("%i\n", this_ptr->id);
        exit(0);
    }
    else {
        token -= 1;

        struct Message *m;

        if (s->cache_msg != NULL) {
            m = s->cache_msg;
            s->cache_msg = NULL;
            m->scheduler = NULL;
            m->next = NULL;
        }

        else {
            //printf("creating a message for %i\n", token);

            m = create_message();
        }

        m->task = msg_pass;
        m->recipient = this_ptr->next;
        m->args[0].Int32 = token;

        //printf("Actor %p is messaging %i to %p\n", this_ptr, token, this_ptr->next);

        msg_actor(message->scheduler, this_ptr->next, m);
    }

    return TASK_DONE;
}

int msg_setIdAndNext(struct Message *message) {
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    int id = message->args[0].Int32;
    struct Passer *next = message->args[1].VoidPtr;

    this_ptr->id = id;
    this_ptr->next = next;

    return TASK_DONE;
}

int main(int argc, char *argv[]) {
    int loop_amount = atoi(argv[1]);

    struct Scheduler *s = create_scheduler();
    int i;

    struct Passer *head = create_passer();
    struct Passer *curr = head;
    struct Message *m;

    for (i = 1; i < THREADRING_SIZE; ++i) {
        struct Passer *next = create_passer();


        m = create_message();
        m->task = msg_setIdAndNext;
        m->recipient = curr;
        m->args[0].Int32 = i;
        m->args[1].VoidPtr = next;

        msg_actor(s, curr, m);

        curr = next;
    }

    m = create_message();
    m->task = msg_setIdAndNext;
    m->recipient = curr;
    m->args[0].Int32 = THREADRING_SIZE;
    m->args[1].VoidPtr = head;

    msg_actor(s, curr, m);

    m = create_message();
    m->task = msg_pass;
    m->recipient = head;
    m->args[0].Int32 = loop_amount;

    msg_actor(s, head, m);

    scheduler_loop(s);

    return 0;
}
*/

