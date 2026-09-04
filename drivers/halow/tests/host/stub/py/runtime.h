// Host-test stub: heap + root-pointer surface only.
//
// m_malloc_maybe is modelled faithfully: it RETURNS NULL on failure rather than
// raising, which is the whole reason the driver uses it. If the driver is ever
// changed to m_malloc (which raises), this stub will not compile -- that is
// deliberate, because a raise out of morselib's frames is unrecoverable.
#ifndef HALOW_HOSTTEST_RUNTIME_H
#define HALOW_HOSTTEST_RUNTIME_H
#include <stdlib.h>
#include "py/mphal.h"

typedef struct _halow_test_state_t {
    void *halow_heap;
    void *mp_halow_spi;
} halow_test_state_t;
extern halow_test_state_t halow_test_state;
#define MP_STATE_PORT(x) (halow_test_state.x)

// Lets a test force allocation failure and prove the driver degrades instead of
// crashing -- the on-device failure mode with no debugger attached.
extern int halow_test_malloc_fail;
void *m_malloc_maybe(size_t n);
void m_free(void *p);
#endif
