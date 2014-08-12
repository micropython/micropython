#include <string.h>
#include <stdarg.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "pfenv.h"

typedef struct _strn_pfenv_t {
    char *cur;
    size_t remain;
} strn_pfenv_t;

STATIC void strn_print_strn(void *data, const char *str, unsigned int len) {
    strn_pfenv_t *strn_pfenv = data;
    if (len > strn_pfenv->remain) {
        len = strn_pfenv->remain;
    }
    memcpy(strn_pfenv->cur, str, len);
    strn_pfenv->cur += len;
    strn_pfenv->remain -= len;
}

int mp_vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    strn_pfenv_t strn_pfenv;
    strn_pfenv.cur = str;
    strn_pfenv.remain = size;
    pfenv_t pfenv;
    pfenv.data = &strn_pfenv;
    pfenv.print_strn = strn_print_strn;
    int len = pfenv_vprintf(&pfenv, fmt, ap);
    // add terminating null byte
    if (size > 0) {
        if (strn_pfenv.remain == 0) {
            strn_pfenv.cur[-1] = 0;
        } else {
            strn_pfenv.cur[0] = 0;
        }
    }
    return len;
}
