// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef CONCURRENCY_H_
#define CONCURRENCY_H_

#include "common.h"

inline CBOOL atomic_cas(volatile void **orig, volatile void *cmp, volatile void *new);
inline CBOOL atomic_cas_int(volatile int *orig, int cmp, int new);

void *create_mutex();
void delete_mutex(void *mutex);
void lock_mutex(void *mutex);
void unlock_mutex(void *mutex);

void *create_thread(void*(*func)(void*), void* arg);
void join_thread(void *thread);
void exit_thread();
int num_hw_threads();
void sleep_in_ms(unsigned int amount);


#endif /* CONCURRENCY_H_ */
