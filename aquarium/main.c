// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

// Temporary little file for testing purposes

#include <stdlib.h>

#include "actor.h"
#include "concurrency.h"
#include "scheduler.h"


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

/*
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
*/

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

        m = create_message();

        m->task = msg_pass;
        m->recipient = this_ptr->next;
        m->args[0].Int32 = token;

        //printf("Actor %p is messaging %i to %p\n", this_ptr, token, this_ptr->next);

        msg_actor(message->scheduler, this_ptr->next, m);
    }

    this_ptr->actor.actor_state = ACTOR_STATE_IDLE;
    return TASK_DONE;
}

int msg_setIdAndNext(struct Message *message) {
    struct Passer *this_ptr = (struct Passer *)(message->recipient);
    int id = message->args[0].Int32;
    struct Passer *next = message->args[1].VoidPtr;

    this_ptr->id = id;
    this_ptr->next = next;

    this_ptr->actor.actor_state = ACTOR_STATE_IDLE;
    return TASK_DONE;
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

    for (i = 1; i < s->num_schedulers; ++i) {
        create_thread(scheduler_loop, s->schedulers[i]);
    }
    scheduler_loop(s);

    return 0;
}
*/


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

