// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef ACTOR_H_
#define ACTOR_H_

#include <stdio.h>

#include "common.h"
#include "message_queue.h"

struct Actor {
    struct Message_Queue *mail;
    void *scheduler;
    int timeslice_remaining;
};

//public API
struct Actor *create_actor();
void initialize_actor(struct Actor *actor);

#endif /* ACTOR_H_ */
