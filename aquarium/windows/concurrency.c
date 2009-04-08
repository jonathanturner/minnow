// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>

#include "common.h"
#include "concurrency.h"

struct Thread {
    HANDLE thread_handle;
    DWORD thread_id;
};

struct Function_Package {
    void*(*func)(void*);
    void* arg;
};

DWORD __stdcall run_function_package(void *v) {
    struct Function_Package *fp = (struct Function_Package*)v;
    fp->func(fp->arg);
    free(fp);
    return 0;
}

inline CBOOL atomic_cas(volatile void **orig, volatile void *cmp, volatile void *new) {
    //printf("CAS\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

inline CBOOL atomic_cas_int(int *orig, int cmp, int new) {
    //printf("CAS Int\n");
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

void *create_thread(void*(*func)(void*), void* arg) {
    struct Thread *retval = (struct Thread*)malloc(sizeof(struct Thread));
    struct Function_Package *fp = (struct Function_Package *)malloc(sizeof(struct Function_Package));
    
    fp->func = func;
    fp->arg = arg;

    retval->thread_handle = CreateThread(0, 0, run_function_package,
            fp, 0, &retval->thread_id);

    if (retval->thread_handle == NULL) {
        printf("Internal error: thread could not be created.  Error no: %i\n", GetLastError());
        exit(-1);
    }

    return retval;
}

void join_thread(void *thread) {
    struct Thread *t = (struct Thread*)thread;

    WaitForSingleObject(t->thread_handle, INFINITE);
}

void exit_thread() {
    //todo: needs implementation
}

int num_hw_threads() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

void sleep_in_ms(unsigned int amount) {
    Sleep(amount);
}
