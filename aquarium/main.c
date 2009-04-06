// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

int test_task(void *v) {
    struct Message *message = (struct Message *)v;

    printf(".");

    return TASK_INCOMPLETE;
}

int main() {
    printf("Hello: %i\n", num_hw_threads());

    struct Scheduler *s = create_scheduler();
    struct Actor *a = create_actor();

    struct Message *m = create_message();
    m->task = test_task;
    m->recipient = a;

    enqueue_msg(a->mail, m);
    push_bottom_actor(s, a);

    scheduler_loop(s);

    return 0;
}
