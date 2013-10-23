// non-local return
// exception handling, basically a stack of setjmp/longjmp buffers

#include <limits.h>

//#ifndef __WORDSIZE
//#error __WORDSIZE needs to be defined
//#endif

typedef struct _nlr_buf_t nlr_buf_t;
struct _nlr_buf_t {
    // the entries here must all be machine word size
    nlr_buf_t *prev;
    void *ret_val;
#if __WORDSIZE == 32
    void *regs[6];
#elif __WORDSIZE == 64
    void *regs[8];
#else
    // hack for thumb
    void *regs[10];
//#error Unsupported __WORDSIZE
#endif
};

unsigned int nlr_push(nlr_buf_t *);
void nlr_pop(void);
void nlr_jump(void *val) __attribute__((noreturn));
