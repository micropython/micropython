#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"

#if MICROPY_ENABLE_LEXER_UNIX

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    uint size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *data = m_new(char, size);
    int read_size = read(fd, data, size);
    close(fd);
    if (read_size != size) {
        printf("error reading file %s\n", filename);
        m_del(char, data, size);
        return NULL;
    }

    return mp_lexer_new_from_str_len(qstr_from_str(filename), data, size, size);
}

/******************************************************************************/
/* unix implementation of import                                              */

// TODO properly!

static const char *import_base_dir = NULL;

void mp_import_set_directory(const char *dir) {
    import_base_dir = dir;
}

mp_lexer_t *mp_import_open_file(qstr mod_name) {
    vstr_t *vstr = vstr_new();
    if (import_base_dir != NULL) {
        vstr_printf(vstr, "%s/", import_base_dir);
    }
    vstr_printf(vstr, "%s.py", qstr_str(mod_name));
    return mp_lexer_new_from_file(vstr_str(vstr)); // TODO does lexer need to copy the string? can we free it here?
}

#endif // MICROPY_ENABLE_LEXER_UNIX
