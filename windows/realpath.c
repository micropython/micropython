#include <stdlib.h>
#include <errno.h>
#include <io.h>

#ifndef R_OK
    #define R_OK 4
#endif

// Make sure a path only has forward slashes.
char *to_unix_path(char *p) {
    if (p != NULL) {
        char *pp = p;
        while (*pp != 0) {
            if (*pp == '\\')
                *pp = '/';
            ++pp;
        }
    }
    return p;
}

// Implement realpath() using _fullpath and make it use the same error codes as realpath() on unix.
// Also have it return a path with forward slashes only as some code relies on this,
// but _fullpath() returns backward slashes no matter what.
char *realpath(const char *path, char *resolved_path) {
    char *ret = NULL;
    if (path == NULL) {
        errno = EINVAL;
    } else if (access(path, R_OK) == 0) {
        ret = resolved_path;
        if (ret == NULL)
            ret = malloc(_MAX_PATH);
        if (ret == NULL) {
            errno = ENOMEM;
        } else {
            ret = _fullpath(ret, path, _MAX_PATH);
            if (ret == NULL)
                errno = EIO;
        }
    }
    return to_unix_path(ret);
}
