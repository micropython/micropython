// non-local return
// exception handling, basically a stack of setjmp/longjmp buffers

#include <limits.h>
#include <setjmp.h>

typedef struct _nlr_buf_t nlr_buf_t;
struct _nlr_buf_t {
    // the entries here must all be machine word size
    nlr_buf_t *prev;
    void *ret_val;
#if !MICROPY_NLR_SETJMP
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
    #define MICROPY_NLR_SETJMP (1)
    #warning "No native NLR support for this arch, using setjmp implementation"
#endif
#endif

#if MICROPY_NLR_SETJMP
    jmp_buf jmpbuf;
#endif
};

#if MICROPY_NLR_SETJMP
extern nlr_buf_t *nlr_setjmp_top;
void nlr_setjmp_jump(void *val) __attribute__((noreturn));
// nlr_push() must be defined as a macro, because "The stack context will be
// invalidated if the function which called setjmp() returns."
#define nlr_push(buf) ((buf)->prev = nlr_setjmp_top, nlr_setjmp_top = (buf), setjmp((buf)->jmpbuf))
#define nlr_pop() { nlr_setjmp_top = nlr_setjmp_top->prev; }
#define nlr_jump(val) nlr_setjmp_jump(val)
#else
unsigned int nlr_push(nlr_buf_t *);
void nlr_pop(void);
void nlr_jump(void *val) __attribute__((noreturn));
#endif

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
