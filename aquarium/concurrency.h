// This file is distributed under the BSD license.
// See LICENSE.TXT for details.


#ifndef CONCURRENCY_H_
#define CONCURRENCY_H_

#include "common.h"

inline BOOL atomic_cas(void **orig, void *cmp, void *new);

#endif /* CONCURRENCY_H_ */
