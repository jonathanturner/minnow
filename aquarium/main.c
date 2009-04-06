// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

int base_count;

int test_task(void *v) {
    struct Message *message = (struct Message *)v;

    int i;

    /*
    for (i = 0; i < 10000; ++i) {
        ++base_count;
    }
    */

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
