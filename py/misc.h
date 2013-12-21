// a mini library of useful types and functions

#ifndef _INCLUDED_MINILIB_H
#define _INCLUDED_MINILIB_H

/** types *******************************************************/

typedef int bool;
enum {
    false = 0,
    true = 1
};

typedef unsigned char byte;
typedef unsigned int uint;

/** memomry allocation ******************************************/

#define m_new(type, num) ((type*)(m_malloc(sizeof(type) * (num))))
#define m_new0(type, num) ((type*)(m_malloc0(sizeof(type) * (num))))
#define m_renew(type, ptr, num) ((type*)(m_realloc((ptr), sizeof(type) * (num))))
#define m_new_obj(type) (m_new(type, 1))
#define m_new_obj_var(obj_type, var_type, var_num) ((obj_type*)m_malloc(sizeof(obj_type) + sizeof(var_type) * (var_num)))

void m_free(void *ptr);
void *m_malloc(int num_bytes);
void *m_malloc0(int num_bytes);
void *m_realloc(void *ptr, int num_bytes);

int m_get_total_bytes_allocated(void);

/** unichar / UTF-8 *********************************************/

typedef int unichar; // TODO

unichar g_utf8_get_char(const char *s);
char *g_utf8_next_char(const char *s);

bool g_unichar_isspace(unichar c);
bool g_unichar_isalpha(unichar c);
bool g_unichar_isprint(unichar c);
bool g_unichar_isdigit(unichar c);

//char *g_strdup(const char *s);

/** blob ********************************************************/

/*
unsigned short decode_le16(byte *buf);
unsigned int decode_le32(byte *buf);
void encode_le16(byte *buf, unsigned short i);
void encode_le32(byte *buf, unsigned int i);
*/

/** string ******************************************************/

/*
#define streq(s1, s2) (strcmp((s1), (s2)) == 0)
*/

/** variable string *********************************************/

typedef struct _vstr_t {
    int alloc;
    int len;
    char *buf;
    bool had_error;
} vstr_t;

void vstr_init(vstr_t *vstr);
void vstr_clear(vstr_t *vstr);
vstr_t *vstr_new(void);
void vstr_free(vstr_t *vstr);
void vstr_reset(vstr_t *vstr);
bool vstr_had_error(vstr_t *vstr);
char *vstr_str(vstr_t *vstr);
int vstr_len(vstr_t *vstr);
void vstr_hint_size(vstr_t *vstr, int size);
char *vstr_add_len(vstr_t *vstr, int len);
void vstr_add_byte(vstr_t *vstr, byte v);
void vstr_add_char(vstr_t *vstr, unichar chr);
void vstr_add_str(vstr_t *vstr, const char *str);
void vstr_add_strn(vstr_t *vstr, const char *str, int len);
//void vstr_add_le16(vstr_t *vstr, unsigned short v);
//void vstr_add_le32(vstr_t *vstr, unsigned int v);
void vstr_cut_tail(vstr_t *vstr, int len);
void vstr_printf(vstr_t *vstr, const char *fmt, ...);

#ifdef va_start
void vstr_vprintf(vstr_t *vstr, const char *fmt, va_list ap);
#endif

/** unique string ***********************************************/

typedef unsigned int qstr;

void qstr_init(void);
qstr qstr_from_str_static(const char *str);
qstr qstr_from_str_take(char *str);
qstr qstr_from_strn_copy(const char *str, int len);
const char* qstr_str(qstr qstr);

#endif // _INCLUDED_MINILIB_H
