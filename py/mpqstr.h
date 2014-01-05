// See mpqstrraw.h for a list of qstr's that are available as constants.
// Reference them as MP_QSTR_xxxx.
//
// Note: it would be possible to define MP_QSTR_xxx as qstr_from_str_static("xxx")
// for qstrs that are referenced this way, but you don't want to have them in ROM.

enum {
    MP_QSTR_nil = 0,
#define Q(id) MP_QSTR_##id,
#include "mpqstrraw.h"
#undef Q
    MP_QSTR_number_of,
} category_t;
