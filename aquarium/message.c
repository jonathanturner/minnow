// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#include <stdlib.h>
#include <stdio.h>

#include "message.h"

struct Message *create_message() {
    struct Message *retval = (struct Message *)malloc(sizeof(struct Message));

    if (retval == NULL) {
        //todo: throw exception
        printf("Could not allocate memory for message, exiting\n");
        exit(1);
    }

    retval->next = NULL;
    retval->recipient = NULL;
    //retval->scheduler = NULL;

    return retval;
}
