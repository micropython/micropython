#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "misc.h"
#include "mpconfig.h"

// returned value is always at least 1 greater than argument
#define ROUND_ALLOC(a) (((a) & ((~0) - 7)) + 8)

void vstr_init(vstr_t *vstr, int alloc) {
    vstr->alloc = alloc;
    vstr->len = 0;
    vstr->buf = m_new(char, vstr->alloc);
    if (vstr->buf == NULL) {
        vstr->had_error = true;
        return;
    }
    vstr->buf[0] = 0;
    vstr->had_error = false;
    vstr->fixed_buf = false;
}

void vstr_init_fixed_buf(vstr_t *vstr, int alloc, char *buf) {
    assert(alloc > 0); // need at least room for the null byte
    vstr->alloc = alloc;
    vstr->len = 0;
    vstr->buf = buf;
    vstr->buf[0] = 0;
    vstr->had_error = false;
    vstr->fixed_buf = true;
}

void vstr_clear(vstr_t *vstr) {
    if (!vstr->fixed_buf) {
        m_del(char, vstr->buf, vstr->alloc);
    }
    vstr->buf = NULL;
}

vstr_t *vstr_new(void) {
    vstr_t *vstr = m_new(vstr_t, 1);
    if (vstr == NULL) {
        return NULL;
    }
    vstr_init(vstr, 32);
    return vstr;
}

vstr_t *vstr_new_size(int alloc) {
    vstr_t *vstr = m_new(vstr_t, 1);
    if (vstr == NULL) {
        return NULL;
    }
    vstr_init(vstr, alloc);
    return vstr;
}

void vstr_free(vstr_t *vstr) {
    if (vstr != NULL) {
        if (!vstr->fixed_buf) {
            m_del(char, vstr->buf, vstr->alloc);
        }
        m_del_obj(vstr_t, vstr);
    }
}

void vstr_reset(vstr_t *vstr) {
    vstr->len = 0;
    vstr->buf[0] = 0;
    vstr->had_error = false;
}

bool vstr_had_error(vstr_t *vstr) {
    return vstr->had_error;
}

char *vstr_str(vstr_t *vstr) {
    if (vstr->had_error) {
        return NULL;
    }
    return vstr->buf;
}

int vstr_len(vstr_t *vstr) {
    if (vstr->had_error) {
        return 0;
    }
    return vstr->len;
}

// Extend vstr strictly to by requested size, return pointer to newly added chunk
char *vstr_extend(vstr_t *vstr, int size) {
    if (vstr->fixed_buf) {
        return NULL;
    }
    char *new_buf = m_renew(char, vstr->buf, vstr->alloc, vstr->alloc + size);
    if (new_buf == NULL) {
        vstr->had_error = true;
        return NULL;
    }
    char *p = new_buf + vstr->alloc;
    vstr->alloc += size;
    vstr->buf = new_buf;
    return p;
}

// Shrink vstr to be given size
bool vstr_set_size(vstr_t *vstr, int size) {
    if (vstr->fixed_buf) {
        return false;
    }
    char *new_buf = m_renew(char, vstr->buf, vstr->alloc, size);
    if (new_buf == NULL) {
        vstr->had_error = true;
        return false;
    }
    vstr->buf = new_buf;
    vstr->alloc = vstr->len = size;
    return true;
}

// Shrink vstr allocation to its actual length
bool vstr_shrink(vstr_t *vstr) {
    return vstr_set_size(vstr, vstr->len);
}

STATIC bool vstr_ensure_extra(vstr_t *vstr, int size) {
    if (vstr->len + size + 1 > vstr->alloc) {
        if (vstr->fixed_buf) {
            return false;
        }
        int new_alloc = ROUND_ALLOC((vstr->len + size + 1) * 2);
        char *new_buf = m_renew(char, vstr->buf, vstr->alloc, new_alloc);
        if (new_buf == NULL) {
            vstr->had_error = true;
            return false;
        }
        vstr->alloc = new_alloc;
        vstr->buf = new_buf;
    }
    return true;
}

void vstr_hint_size(vstr_t *vstr, int size) {
    // it's not an error if we fail to allocate for the size hint
    bool er = vstr->had_error;
    vstr_ensure_extra(vstr, size);
    vstr->had_error = er;
}

char *vstr_add_len(vstr_t *vstr, int len) {
    if (vstr->had_error || !vstr_ensure_extra(vstr, len)) {
        return NULL;
    }
    char *buf = vstr->buf + vstr->len;
    vstr->len += len;
    vstr->buf[vstr->len] = 0;
    return buf;
}

void vstr_add_byte(vstr_t *vstr, byte b) {
    byte *buf = (byte*)vstr_add_len(vstr, 1);
    if (buf == NULL) {
        return;
    }
    buf[0] = b;
}

void vstr_add_char(vstr_t *vstr, unichar c) {
    // TODO UNICODE
    byte *buf = (byte*)vstr_add_len(vstr, 1);
    if (buf == NULL) {
        return;
    }
    buf[0] = c;
}

void vstr_add_str(vstr_t *vstr, const char *str) {
    vstr_add_strn(vstr, str, strlen(str));
}

void vstr_add_strn(vstr_t *vstr, const char *str, int len) {
    if (vstr->had_error || !vstr_ensure_extra(vstr, len)) {
        // if buf is fixed, we got here because there isn't enough room left
        // so just try to copy as much as we can, with room for null byte
        if (vstr->fixed_buf && vstr->len + 1 < vstr->alloc) {
            len = vstr->alloc - vstr->len - 1;
            goto copy;
        }
        return;
    }
copy:
    memmove(vstr->buf + vstr->len, str, len);
    vstr->len += len;
    vstr->buf[vstr->len] = 0;
}

/*
void vstr_add_le16(vstr_t *vstr, unsigned short v) {
    byte *buf = (byte*)vstr_add_len(vstr, 2);
    if (buf == NULL) {
        return;
    }
    encode_le16(buf, v);
}

void vstr_add_le32(vstr_t *vstr, unsigned int v) {
    byte *buf = (byte*)vstr_add_len(vstr, 4);
    if (buf == NULL) {
        return;
    }
    encode_le32(buf, v);
}
*/

char *vstr_ins_blank_bytes(vstr_t *vstr, uint byte_pos, uint byte_len) {
    if (vstr->had_error) {
        return NULL;
    }
    uint l = vstr->len;
    if (byte_pos > l) {
        byte_pos = l;
    }
    if (byte_len > 0) {
        // ensure room for the new bytes
        if (!vstr_ensure_extra(vstr, byte_len)) {
            return NULL;
        }
        // copy up the string to make room for the new bytes; +1 for the null byte
        memmove(vstr->buf + byte_pos + byte_len, vstr->buf + byte_pos, l - byte_pos + 1);
        // increase the length
        vstr->len += byte_len;
    }
    return vstr->buf + byte_pos;
}

void vstr_ins_byte(vstr_t *vstr, uint byte_pos, byte b) {
    char *s = vstr_ins_blank_bytes(vstr, byte_pos, 1);
    if (s != NULL) {
        *s = b;
    }
}

void vstr_ins_char(vstr_t *vstr, uint char_pos, unichar chr) {
    // TODO UNICODE
    char *s = vstr_ins_blank_bytes(vstr, char_pos, 1);
    if (s != NULL) {
        *s = chr;
    }
}

void vstr_cut_head_bytes(vstr_t *vstr, uint bytes_to_cut) {
    vstr_cut_out_bytes(vstr, 0, bytes_to_cut);
}

void vstr_cut_tail_bytes(vstr_t *vstr, uint len) {
    if (vstr->had_error) {
        return;
    }
    if (len > vstr->len) {
        vstr->len = 0;
    } else {
        vstr->len -= len;
    }
    vstr->buf[vstr->len] = 0;
}

void vstr_cut_out_bytes(vstr_t *vstr, uint byte_pos, uint bytes_to_cut) {
    if (vstr->had_error || byte_pos >= vstr->len) {
        return;
    } else if (byte_pos + bytes_to_cut >= vstr->len) {
        vstr->len = byte_pos;
        vstr->buf[vstr->len] = 0;
    } else {
        // move includes +1 for null byte at the end
        memmove(vstr->buf + byte_pos, vstr->buf + byte_pos + bytes_to_cut, vstr->len - byte_pos - bytes_to_cut + 1);
        vstr->len -= bytes_to_cut;
    }
}

void vstr_printf(vstr_t *vstr, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vstr_vprintf(vstr, fmt, ap);
    va_end(ap);
}

void vstr_vprintf(vstr_t *vstr, const char *fmt, va_list ap) {
    if (vstr->had_error || !vstr_ensure_extra(vstr, strlen(fmt))) {
        return;
    }

    while (1) {
        // try to print in the allocated space
        // need to make a copy of the va_list because we may call vsnprintf multiple times
        int size = vstr->alloc - vstr->len;
        va_list ap2;
        va_copy(ap2, ap);
        int n = vsnprintf(vstr->buf + vstr->len, size, fmt, ap2);
        va_end(ap2);

        // if that worked, return
        if (n > -1 && n < size) {
            vstr->len += n;
            return;
        }

        // else try again with more space
        if (n > -1) { // glibc 2.1
            // n + 1 is precisely what is needed
            if (!vstr_ensure_extra(vstr, n + 1)) {
                return;
            }
        } else { // glibc 2.0
            // increase to twice the old size
            if (!vstr_ensure_extra(vstr, size * 2)) {
                return;
            }
        }
    }
}

/** testing *****************************************************/

/*
int main(void) {
    vstr_t *vstr = vstr_new();
    int i;
    for (i = 0; i < 10; i++) {
        vstr_printf(vstr, "%d) this is a test %d %s\n", i, 1234, "'a string'");
        vstr_add_str(vstr, "-----");
        vstr_printf(vstr, "this is another test %d %s\n", 1234, "'a second string'");
        printf("%s", vstr->buf);
    }
}
*/
