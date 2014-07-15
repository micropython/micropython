#include <stdio.h>
#include <stdint.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"

#include "memzip.h"

mp_import_stat_t mp_import_stat(const char *path) {
    MEMZIP_FILE_INFO info;

    if (memzip_stat(path, &info) != MZ_OK) {
        return MP_IMPORT_STAT_NO_EXIST;
    }

    if (info.is_dir) {
        return MP_IMPORT_STAT_DIR;
    }
    return MP_IMPORT_STAT_FILE;
}
