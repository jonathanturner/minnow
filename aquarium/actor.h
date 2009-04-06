// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef ACTOR_H_
#define ACTOR_H_

#include <stdio.h>

#include "common.h"
#include "message_queue.h"

#define ACTOR_STATE_IDLE        1
#define ACTOR_STATE_SCHEDULED   2
#define ACTOR_STATE_MSG         3

struct Actor {
    int actor_state;
    struct Message_Queue *mail;
    void *scheduler;
    int timeslice_remaining;
};

//public API
struct Actor *create_actor();

#endif /* ACTOR_H_ */
