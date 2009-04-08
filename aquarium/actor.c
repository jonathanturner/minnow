// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <stdio.h>
#include <stdlib.h>

#include "actor.h"

struct Actor *create_actor() {
    struct Actor *retval = (struct Actor*)malloc(sizeof(struct Actor));

    if (retval == NULL) {
        //todo: throw exception
        printf("Could not allocate memory for actor, exiting\n");
        exit(1);
    }
    retval->mail = create_message_queue();
    retval->internal_coro = Coro_new();
    retval->actor_state = ACTOR_STATE_IDLE;

    return retval;
}

void initialize_actor(struct Actor *actor) {
    actor->mail = create_message_queue();
    actor->internal_coro = Coro_new();
    actor->actor_state = ACTOR_STATE_IDLE;
}
