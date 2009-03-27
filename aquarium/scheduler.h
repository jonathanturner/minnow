// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "common.h"
#include "actor.h"

#define DEQUEUE_SIZE 100000

union Age {
    struct {
        int top:17;
        int tag:15;
    } Packed __attribute__((__packed__));
    int Int;
};

struct Scheduler {
    struct Actor *actor_deq[DEQUEUE_SIZE];
    union Age age;
    int bot;
};

struct Scheduler *create_scheduler();
struct Actor *pop_top_actor(struct Scheduler *scheduler);
struct Actor *pop_bottom_actor(struct Scheduler *scheduler);

//public API
void push_bottom_actor(void *scheduler, void *actor);


#endif /* SCHEDULER_H_ */
