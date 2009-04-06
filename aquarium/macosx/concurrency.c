// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>
#include <stdio.h>

#include <sys/sysctl.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "concurrency.h"

struct Thread {
    pthread_t thread_id;
};

inline BOOL atomic_cas(volatile void **orig, volatile void *cmp, volatile void *new) {
    //printf("CAS\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

inline BOOL atomic_cas_int(int *orig, int cmp, int new) {
    //printf("CAS Int\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

void *thread_create(void*(*func)(void*), void* arg) {
    struct Thread *retval = (struct Thread*)malloc(sizeof(struct Thread));

    int error_val = pthread_create(&retval->thread_id, NULL, func, arg);
    if (error_val != 0) {
        printf("Internal error: thread could not be created.  Error no: %i\n", error_val);
        exit(-1);
    }

    return retval;
}

void thread_join(void *thread) {
    struct Thread *t = (struct Thread*)thread;

    pthread_join(t->thread_id, NULL);
}

void thread_exit() {
    pthread_exit(NULL);
}

int num_hw_threads() {
    int num, return_val;
    size_t size = sizeof(num);

    return_val = sysctlbyname("hw.ncpu", &num, &size, NULL, 0);
    if (return_val == 0) {
        return num;
    }
    else {
        return -1;
    }
}

void sleep_in_ms(unsigned int amount) {
    usleep(amount * 1000);
}
