// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

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

CBOOL msg_pass(struct Message *message) {
    //struct Scheduler *s = (struct Scheduler *)message->scheduler;
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    struct Scheduler *s = (struct Scheduler *)(this_ptr->actor.scheduler);
    int token = message->args[0].Int32;

    if (token == 0) {
        printf("%i\n", this_ptr->id);
        exit(0);
    }
    else {
        token -= 1;

        struct Message *m;

        //m = create_message();
        m = get_free_message(s);

        //printf("Passing: %p %i to %p\n", message->recipient, token, this_ptr->next);
        m->task = msg_pass;
        m->recipient = this_ptr->next;
        m->args[0].Int32 = token;

        //printf("Actor %p is messaging %i to %p\n", this_ptr, token, this_ptr->next);

        msg_actor(s, this_ptr->next, m);
    }

    //this_ptr->actor.actor_state = ACTOR_STATE_IDLE;
    return CTRUE;
}

CBOOL msg_setIdAndNext(struct Message *message) {
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    int id = message->args[0].Int32;
    struct Passer *next = message->args[1].VoidPtr;

    //printf("Setting: %p to %i and %p\n", message->recipient, id, next);
    this_ptr->id = id;
    this_ptr->next = next;

    //this_ptr->actor.actor_state = ACTOR_STATE_IDLE;
    return CTRUE;
}

int main(int argc, char *argv[]) {
    int loop_amount = atoi(argv[1]);

    struct Scheduler *s = create_all_schedulers(num_hw_threads());
    int i;

    struct Passer *head = create_passer();
    struct Passer *curr = head;
    struct Message *m;

    for (i = 1; i < THREADRING_SIZE; ++i) {
        struct Passer *next = create_passer();

        //m = create_message();
        m = get_free_message(s);
        m->task = msg_setIdAndNext;
        m->recipient = curr;
        m->args[0].Int32 = i;
        m->args[1].VoidPtr = next;

        msg_actor(s, curr, m);

        curr = next;
    }

    //m = create_message();
    m = get_free_message(s);
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

    for (i = 1; i < s->num_schedulers; ++i) {
        //create_thread(scheduler_loop, s->schedulers[i]);
    }
    scheduler_loop(s);

    return 0;
}
