// non-local return
// exception handling, basically a stack of setjmp/longjmp buffers

#include <limits.h>

typedef struct _nlr_buf_t nlr_buf_t;
struct _nlr_buf_t {
    // the entries here must all be machine word size
    nlr_buf_t *prev;
    void *ret_val;
#if defined(__i386__)
    void *regs[6];
#elif defined(__x86_64__)
  #if defined(__CYGWIN__)
    void *regs[12];
  #else
    void *regs[8];
  #endif
#elif defined(__thumb2__)
    void *regs[10];
#else
#error Unknown arch in nlr.h
#endif
};

unsigned int nlr_push(nlr_buf_t *);
void nlr_pop(void);
void nlr_jump(void *val) __attribute__((noreturn));

// This must be implemented by a port.  It's called by nlr_jump
// if no nlr buf has been pushed.  It must not return, but rather
// should bail out with a fatal error.
void nlr_jump_fail(void *val);

// use nlr_raise instead of nlr_jump so that debugging is easier
#ifndef DEBUG
#define nlr_raise(val) nlr_jump(val)
#else
#define nlr_raise(val) \
    do { \
        void *_val = val; \
        assert(_val != NULL); \
        assert(mp_obj_is_exception_instance(_val)); \
        nlr_jump(_val); \
    } while (0)
#endif
