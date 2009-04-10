// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"

#define BIGBANG_SIZE 1500

struct BigBang {
    struct Actor actor;

    int total_recv;
    int id;
};

struct BigBang *create_bigbang() {
    struct BigBang *retval = (struct BigBang *)malloc(sizeof(struct BigBang));
    initialize_actor(&(retval->actor));

    retval->total_recv = 0;

    return retval;
}

int msg_recv(struct Message *message) {
    struct BigBang *this_ptr = (struct BigBang *)(message->recipient);

    //free(message);
    ++this_ptr->total_recv;
    if (this_ptr->total_recv == (BIGBANG_SIZE)) {
        printf("Id: %i reached goal of %i\n", this_ptr->id, this_ptr->total_recv);
    }

    this_ptr->actor.actor_state = ACTOR_STATE_IDLE;
    return TASK_DONE;
}

int msg_send_all(struct Message *message) {
    struct Scheduler *s = (struct Scheduler *)message->scheduler;
    struct BigBang *this_ptr = (struct BigBang *)(message->recipient);

    int id = message->args[0].Int32;
    struct BigBang **actor_list = message->args[1].VoidPtr;

    int i;
    for (i = 0; i < BIGBANG_SIZE; ++i) {
        struct Message *m;

        m = create_message();

        m->task = msg_recv;
        m->recipient = actor_list[i];

        //printf("Actor %p is messaging %i to %p\n", this_ptr, token, this_ptr->next);

        msg_actor(message->scheduler, actor_list[i], m);
    }
    printf("Actor %i done messaging\n", id);
    this_ptr->actor.actor_state = ACTOR_STATE_IDLE;

    return TASK_DONE;
}

int main(int argc, char *argv[]) {
    //int loop_amount = atoi(argv[1]);

    struct Scheduler *s = create_all_schedulers(num_hw_threads());
    int i;
    struct Message *m;

    struct BigBang **bigbangs = (struct BigBang **)malloc(BIGBANG_SIZE * sizeof(struct BigBang*));

    for (i = 0; i < BIGBANG_SIZE; ++i) {
        bigbangs[i] = create_bigbang();
        bigbangs[i]->id = i;
    }

    for (i = 0; i < BIGBANG_SIZE; ++i) {

        m = create_message();
        m->task = msg_send_all;
        m->recipient = bigbangs[i];
        m->args[0].Int32 = i;
        m->args[1].VoidPtr = bigbangs;

        msg_actor(s, bigbangs[i], m);
    }

    for (i = 1; i < s->num_schedulers; ++i) {
        create_thread(scheduler_loop, s->schedulers[i]);
    }
    scheduler_loop(s);

    return 0;
}
