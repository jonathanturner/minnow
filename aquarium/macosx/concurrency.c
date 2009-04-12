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

struct Mutex {
    pthread_mutex_t mutex_id;
    pthread_mutexattr_t mutex_attr;
};

inline CBOOL atomic_cas(volatile void **orig, volatile void *cmp, volatile void *new) {
    //printf("CAS\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

inline CBOOL atomic_cas_int(volatile int *orig, int cmp, int new) {
    //printf("CAS Int\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

void *create_mutex() {
    struct Mutex *retval = (struct Mutex*)malloc(sizeof(struct Mutex));
    if (retval == NULL) {
        printf("Internal error: not enough space to create mutex, exiting\n");
        exit(1);
    }

    int error_val = pthread_mutexattr_init(&(retval->mutex_attr));
    if (error_val != 0) {
        printf("Internal error: mutex could not be created.  Error no: %i\n", error_val);
        exit(1);
    }

    error_val = pthread_mutex_init(&(retval->mutex_id), &(retval->mutex_attr));
    if (error_val != 0) {
        printf("Internal error: mutex could not be created.  Error no: %i\n", error_val);
        exit(1);
    }

    return retval;
}

void delete_mutex(void *mutex) {
    struct Mutex *m = (struct Mutex *)mutex;

    pthread_mutex_destroy(&(m->mutex_id));
    free(m);
}

void lock_mutex(void *mutex) {
    struct Mutex *m = (struct Mutex *)mutex;

    pthread_mutex_lock(&(m->mutex_id));
}

void unlock_mutex(void *mutex) {
    struct Mutex *m = (struct Mutex *)mutex;

     pthread_mutex_unlock(&(m->mutex_id));
 }

void *create_thread(void*(*func)(void*), void* arg) {
    struct Thread *retval = (struct Thread*)malloc(sizeof(struct Thread));

    int error_val = pthread_create(&retval->thread_id, NULL, func, arg);
    if (error_val != 0) {
        printf("Internal error: thread could not be created.  Error no: %i\n", error_val);
        exit(1);
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
