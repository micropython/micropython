#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"

#if MICROPY_ENABLE_LEXER_UNIX

#include <sys/stat.h>
#include <sys/types.h>

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

#endif // MICROPY_ENABLE_LEXER_UNIX
