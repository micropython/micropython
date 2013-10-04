#include <assert.h>
#include <string.h>

#include "misc.h"

static int qstrs_alloc;
static int qstrs_len;
static const char **qstrs;

void qstr_init() {
    qstrs_alloc = 400;
    qstrs_len = 1;
    qstrs = m_new(const char*, qstrs_alloc);
    qstrs[0] = "nil";
}

static qstr qstr_add(const char *str) {
    if (qstrs_len >= qstrs_alloc) {
        qstrs_alloc *= 2;
        qstrs = m_renew(const char*, qstrs, qstrs_alloc);
    }
    qstrs[qstrs_len++] = str;
    return qstrs_len - 1;
}

qstr qstr_from_str_static(const char *str) {
    for (int i = 0; i < qstrs_len; i++) {
        if (strcmp(qstrs[i], str) == 0) {
            return i;
        }
    }
    return qstr_add(str);
}

qstr qstr_from_str_take(char *str) {
    for (int i = 0; i < qstrs_len; i++) {
        if (strcmp(qstrs[i], str) == 0) {
            m_free(str);
            return i;
        }
    }
    return qstr_add(str);
}

qstr qstr_from_strn_copy(const char *str, int len) {
    for (int i = 0; i < qstrs_len; i++) {
        if (strncmp(qstrs[i], str, len) == 0 && qstrs[i][len] == '\0') {
            return i;
        }
    }
    return qstr_add(strndup(str, len));
}

const char *qstr_str(qstr qstr) {
    return qstrs[qstr];
}
