// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef CONCURRENCY_H_
#define CONCURRENCY_H_

#include "common.h"

inline BOOL atomic_cas(void **orig, void *cmp, void *new);
inline BOOL atomic_cas_int(int *orig, int cmp, int new);

#endif /* CONCURRENCY_H_ */
