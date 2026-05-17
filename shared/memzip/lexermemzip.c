#include <stdlib.h>

#include "py/lexer.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "memzip.h"

mp_lexer_t *mp_lexer_new_from_file(qstr filename)
{
    void *data;
    size_t len;

    if (memzip_locate(qstr_str(filename), &data, &len) != MZ_OK) {
        mp_raise_OSError(MP_ENOENT);
    }

    return mp_lexer_new_from_str_len(filename, (const char *)data, (mp_uint_t)len, 0);
}

