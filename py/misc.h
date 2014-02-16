// a mini library of useful types and functions

#ifndef _INCLUDED_MINILIB_H
#define _INCLUDED_MINILIB_H

/** types *******************************************************/

#include <stdbool.h>

typedef unsigned char byte;
typedef unsigned int uint;

/** generic ops *************************************************/

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

/** memomry allocation ******************************************/

// TODO make a lazy m_renew that can increase by a smaller amount than requested (but by at least 1 more element)

#define m_new(type, num) ((type*)(m_malloc(sizeof(type) * (num))))
#define m_new0(type, num) ((type*)(m_malloc0(sizeof(type) * (num))))
#define m_new_obj(type) (m_new(type, 1))
#define m_new_obj_var(obj_type, var_type, var_num) ((obj_type*)m_malloc(sizeof(obj_type) + sizeof(var_type) * (var_num)))
#define m_renew(type, ptr, old_num, new_num) ((type*)(m_realloc((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num))))
#define m_del(type, ptr, num) m_free(ptr, sizeof(type) * (num))
#define m_del_obj(type, ptr) (m_del(type, ptr, 1))
#define m_del_var(obj_type, var_type, var_num, ptr) (m_free(ptr, sizeof(obj_type) + sizeof(var_type) * (var_num)))

void *m_malloc(int num_bytes);
void *m_malloc0(int num_bytes);
void *m_realloc(void *ptr, int old_num_bytes, int new_num_bytes);
void m_free(void *ptr, int num_bytes);

int m_get_total_bytes_allocated(void);
int m_get_current_bytes_allocated(void);
int m_get_peak_bytes_allocated(void);

/** unichar / UTF-8 *********************************************/

typedef int unichar; // TODO

unichar utf8_get_char(const char *s);
char *utf8_next_char(const char *s);

bool unichar_isspace(unichar c);
bool unichar_isalpha(unichar c);
bool unichar_isprint(unichar c);
bool unichar_isdigit(unichar c);
bool unichar_isxdigit(unichar c);

/** string ******************************************************/

/*
#define streq(s1, s2) (strcmp((s1), (s2)) == 0)
*/

long strtonum(const char *restrict s, int base);

/** variable string *********************************************/

typedef struct _vstr_t {
    int alloc;
    int len;
    char *buf;
    struct {
        bool had_error : 1;
        bool fixed_buf : 1;
    };
} vstr_t;

// convenience macro to declare a vstr with a fixed size buffer on the stack
#define VSTR_FIXED(vstr, alloc) vstr_t vstr; char vstr##_buf[(alloc)]; vstr_init_fixed_buf(&vstr, (alloc), vstr##_buf);

void vstr_init(vstr_t *vstr, int alloc);
void vstr_init_fixed_buf(vstr_t *vstr, int alloc, char *buf);
void vstr_clear(vstr_t *vstr);
vstr_t *vstr_new(void);
vstr_t *vstr_new_size(int alloc);
void vstr_free(vstr_t *vstr);
void vstr_reset(vstr_t *vstr);
bool vstr_had_error(vstr_t *vstr);
char *vstr_str(vstr_t *vstr);
int vstr_len(vstr_t *vstr);
void vstr_hint_size(vstr_t *vstr, int size);
char *vstr_extend(vstr_t *vstr, int size);
bool vstr_set_size(vstr_t *vstr, int size);
bool vstr_shrink(vstr_t *vstr);
char *vstr_add_len(vstr_t *vstr, int len);
void vstr_add_byte(vstr_t *vstr, byte v);
void vstr_add_char(vstr_t *vstr, unichar chr);
void vstr_add_str(vstr_t *vstr, const char *str);
void vstr_add_strn(vstr_t *vstr, const char *str, int len);
//void vstr_add_le16(vstr_t *vstr, unsigned short v);
//void vstr_add_le32(vstr_t *vstr, unsigned int v);
void vstr_cut_tail(vstr_t *vstr, int len);
void vstr_printf(vstr_t *vstr, const char *fmt, ...);

/** non-dynamic size-bounded variable buffer/string *************/

#define CHECKBUF(buf, max_size) char buf[max_size + 1]; uint buf##_len = max_size; char *buf##_p = buf;
#define CHECKBUF_RESET(buf, max_size) buf##_len = max_size; buf##_p = buf;
#define CHECKBUF_APPEND(buf, src, src_len) \
        { int l = MIN(src_len, buf##_len); \
        memcpy(buf##_p, src, l); \
        buf##_len -= l; \
        buf##_p += l; }
#define CHECKBUF_APPEND_0(buf) { *buf##_p = 0; }
#define CHECKBUF_LEN(buf) (buf##_p - buf)

#ifdef va_start
void vstr_vprintf(vstr_t *vstr, const char *fmt, va_list ap);
#endif

// Debugging helpers
int DEBUG_printf(const char *fmt, ...);

#endif // _INCLUDED_MINILIB_H
