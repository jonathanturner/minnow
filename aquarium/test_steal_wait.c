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

CBOOL test_task(struct Message *v) {
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
            //c->actor.actor_state = ACTOR_STATE_IDLE;
            return CTRUE;
        }
        sleep_in_ms(5);
        if ((base_count % 1000) == 0) {
            printf("==== %i in actor %i in %p =====\n", base_count, c->id, c->actor.scheduler);
        }
    }
    return CTRUE;
}

struct Counter *create_counter() {
    struct Counter *retval = (struct Counter *)malloc(sizeof(struct Counter));
    initialize_actor(&(retval->actor));

    return retval;
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
