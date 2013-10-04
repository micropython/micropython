#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "misc.h"
#include "lexer.h"

py_lexer_t *py_lexer_from_file(const char *filename) {
    // TODO abstract away file functionality
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("cannot open file %s\n", filename);
        return NULL;
    }
    uint size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *data = m_new(char, size);
    read(fd, data, size);
    close(fd);

    return py_lexer_from_str_len(filename, data, size, true);
}
