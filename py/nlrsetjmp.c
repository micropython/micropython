#include <setjmp.h>
#include <stdio.h>
#include "mpconfig.h"
#include "nlr.h"

#if MICROPY_NLR_SETJMP

nlr_buf_t *nlr_setjmp_top;

void nlr_setjmp_jump(void *val) {
    nlr_buf_t *buf = nlr_setjmp_top;
    nlr_setjmp_top = buf->prev;
    buf->ret_val = val;
    longjmp(buf->jmpbuf, 1);
}

#endif
