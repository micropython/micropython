#include <stdint.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#if 0
#include "ff.h"
#endif

mp_import_stat_t mp_import_stat(const char *path) {
#if 0
    FILINFO fno;
    FRESULT res = f_stat(path, &fno);
    if (res == FR_OK) {
        if ((fno.fattrib & AM_DIR) != 0) {
            return MP_IMPORT_STAT_DIR;
        } else {
            return MP_IMPORT_STAT_FILE;
        }
    }
#endif
    return MP_IMPORT_STAT_NO_EXIST;
}
