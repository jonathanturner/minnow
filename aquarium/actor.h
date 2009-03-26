// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef ACTOR_H_
#define ACTOR_H_

#include <stdio.h>

#include "common.h"
#include "message_queue.h"

struct Actor {
    struct Message_Queue *mail;
};

#endif /* ACTOR_H_ */
