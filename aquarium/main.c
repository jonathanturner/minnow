// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

/*
int base_count;

int test_task(void *v) {
    struct Message *message = (struct Message *)v;

    int i;

    base_count += 200;

    if (base_count >= 1000000000) {
        exit(0);
    }

    printf("==== %i =====\n", base_count);

    return TASK_INCOMPLETE;
}

int main() {
    printf("Hello: %i\n", num_hw_threads());

    struct Scheduler *s = create_scheduler();
    struct Actor *a = create_actor();

    struct Message *m = create_message();
    m->task = test_task;
    m->recipient = a;

    msg_actor(s, a, m);
    base_count = 0;
    scheduler_loop(s);

    return 0;
}
*/

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
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    int token = message->args[0].Int32;

    if (token == 0) {
        printf("%i\n", this_ptr->id);
        exit(0);
    }
    else {
        token -= 1;

        message->task = msg_pass;
        message->recipient = this_ptr->next;
        message->args[0].Int32 = token;

        msg_actor(message->scheduler, this_ptr->next, message);
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
