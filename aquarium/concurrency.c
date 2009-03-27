// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#include "common.h"
#include "concurrency.h"

inline BOOL atomic_cas(void **orig, void *cmp, void *new) {
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

inline BOOL atomic_cas_int(int *orig, int cmp, int new) {
    return __sync_bool_compare_and_swap(orig, cmp, new);
}

