#include <stdint.h>
#include <stdlib.h>

#include "misc.h"
#include "lexer.h"
#include "memzip.h"

mp_lexer_t *mp_lexer_new_from_memzip_file(const char *filename)
{
    void *data;
    size_t len;

    if (memzip_locate(filename, &data, &len) != MZ_OK) {
        return NULL;
    }
    return mp_lexer_new_from_str_len(filename, (const char *)data, (uint)len, 0);
}

