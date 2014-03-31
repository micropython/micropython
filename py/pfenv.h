#define PF_FLAG_LEFT_ADJUST     (0x001)
#define PF_FLAG_SHOW_SIGN       (0x002)
#define PF_FLAG_SPACE_SIGN      (0x004)
#define PF_FLAG_NO_TRAILZ       (0x008)
#define PF_FLAG_SHOW_PREFIX     (0x010)
#define PF_FLAG_SHOW_COMMA      (0x020)
#define PF_FLAG_PAD_AFTER_SIGN  (0x040)
#define PF_FLAG_CENTER_ADJUST   (0x080)
#define PF_FLAG_ADD_PERCENT     (0x100)
#define PF_FLAG_PAD_NAN_INF     (0x200)

typedef struct _pfenv_t {
    void *data;
    void (*print_strn)(void *, const char *str, unsigned int len);
} pfenv_t;

void pfenv_vstr_add_strn(void *data, const char *str, unsigned int len);

int pfenv_print_strn(const pfenv_t *pfenv, const char *str, unsigned int len, int flags, char fill, int width);
int pfenv_print_int(const pfenv_t *pfenv, unsigned int x, int sgn, int base, int base_char, int flags, char fill, int width);
#if MICROPY_ENABLE_FLOAT
int pfenv_print_float(const pfenv_t *pfenv, mp_float_t f, char fmt, int flags, char fill, int width, int prec);
#endif
