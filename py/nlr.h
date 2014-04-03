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
