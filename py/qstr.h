// See qstrraw.h for a list of qstr's that are available as constants.
// Reference them as MP_QSTR_xxxx.
//
// Note: it would be possible to define MP_QSTR_xxx as qstr_from_str_static("xxx")
// for qstrs that are referenced this way, but you don't want to have them in ROM.

enum {
    MP_QSTR_NULL = 0, // indicates invalid/no qstr
    MP_QSTR_ = 1, // the empty qstr
#define Q(id, str) MP_QSTR_##id,
// TODO having 'build/py.' here is a bit of a hack, should take config variable from Makefile
#include "build/py.qstrdefs.generated.h"
#undef Q
    MP_QSTR_number_of,
} category_t;

typedef machine_uint_t qstr;

#define QSTR_FROM_STR_STATIC(s) (qstr_from_strn((s), strlen(s)))

void qstr_init(void);

qstr qstr_from_str(const char *str);
qstr qstr_from_strn(const char *str, uint len);
//qstr qstr_from_str_static(const char *str);
qstr qstr_from_strn_take(char *str, uint alloc_len, uint len);
//qstr qstr_from_strn_copy(const char *str, int len);

byte* qstr_build_start(uint len, byte **q_ptr);
qstr qstr_build_end(byte *q_ptr);

machine_uint_t qstr_hash(qstr q);
const char* qstr_str(qstr q);
uint qstr_len(qstr q);
const byte* qstr_data(qstr q, uint *len);
