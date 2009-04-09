// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef ACTOR_H_
#define ACTOR_H_

#include <stdio.h>

#include "common.h"
#include "message_queue.h"

#define ACTOR_STATE_IDLE    1
#define ACTOR_STATE_BUSY    2

struct Actor {
    struct Message_Queue *mail;
    void *scheduler;
    int timeslice_remaining;
    volatile int actor_state;
};

//public API
struct Actor *create_actor();
void initialize_actor(struct Actor *actor);

#endif /* ACTOR_H_ */
