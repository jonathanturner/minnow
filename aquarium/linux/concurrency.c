// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>
#include <stdio.h>

#include <sys/sysctl.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "concurrency.h"

struct Thread {
    pthread_t thread_id;
};

inline CBOOL atomic_cas(volatile void **orig, volatile void *cmp, volatile void *new) {
    //printf("CAS\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

inline CBOOL atomic_cas_int(volatile int *orig, int cmp, int new) {
    //printf("CAS Int\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

void *create_thread(void*(*func)(void*), void* arg) {
    struct Thread *retval = (struct Thread*)malloc(sizeof(struct Thread));

    int error_val = pthread_create(&retval->thread_id, NULL, func, arg);
    if (error_val != 0) {
        printf("Internal error: thread could not be created.  Error no: %i\n", error_val);
        exit(-1);
    }

    return retval;
}

void join_thread(void *thread) {
    struct Thread *t = (struct Thread*)thread;

    pthread_join(t->thread_id, NULL);
}

void exit_thread() {
    pthread_exit(NULL);
}

int num_hw_threads() {
    return get_nprocs();
}

void sleep_in_ms(unsigned int amount) {
    usleep(amount * 1000);
}
