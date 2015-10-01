#ifndef _MEMZIP_EXTRAS_H
#define _MEMZIP_EXTRAS_H

#include "memzip.h"

typedef int (memzip_iterate_callback_t)(const MEMZIP_FILE_HDR *h, const char *name, void *user);

const MEMZIP_FILE_HDR *memzip_iterate(memzip_iterate_callback_t *callback, void *user);

#endif /* _MEMZIP_EXTRAS_H */
