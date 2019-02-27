/*
* This file is part of the MicroPython project, http://micropython.org/
*
* The MIT License (MIT)
*
* Copyright (c) 2015 Damien P. George
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "dirent.h"
#include "extmod/vfs.h"
#include <errno.h>
#include <Windows.h>

typedef struct DIR {
    HANDLE findHandle;
    WIN32_FIND_DATA findData;
    struct dirent result;
} DIR;

DIR *opendir(const char *name) {
    if (!name || !*name) {
        errno = ENOENT;
        return NULL;
    }

    DIR *dir = malloc(sizeof(DIR));
    if (!dir) {
        errno = ENOMEM;
        return NULL;
    }
    dir->result.d_ino = 0;
    dir->result.d_name = NULL;
    dir->findHandle = INVALID_HANDLE_VALUE;

    const size_t nameLen = strlen(name);
    char *path = malloc(nameLen + 3); // allocate enough for adding "/*"
    if (!path) {
        free(dir);
        errno = ENOMEM;
        return NULL;
    }
    strcpy(path, name);

    // assure path ends with wildcard
    const char lastChar = path[nameLen - 1];
    if (lastChar != '*') {
        const char *appendWC = (lastChar != '/' && lastChar != '\\') ? "/*" : "*";
        strcat(path, appendWC);
    }

    // init
    dir->findHandle = FindFirstFile(path, &dir->findData);
    free(path);
    if (dir->findHandle == INVALID_HANDLE_VALUE) {
        free(dir);
        errno = ENOENT;
        return NULL;
    }
    return dir;
}

int closedir(DIR *dir) {
    if (dir) {
        FindClose(dir->findHandle);
        free(dir);
        return 0;
    } else {
        errno = EBADF;
        return -1;
    }
}

struct dirent *readdir(DIR *dir) {
    if (!dir) {
        errno = EBADF;
        return NULL;
    }

    // first pass d_name is NULL so use result from FindFirstFile in opendir, else use FindNextFile
    if (!dir->result.d_name || FindNextFile(dir->findHandle, &dir->findData)) {
        dir->result.d_name = dir->findData.cFileName;
        dir->result.d_type = dir->findData.dwFileAttributes;
        return &dir->result;
    }

    return NULL;
}

int dttoif(int d_type) {
    if (d_type == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    // Could be a couple of things (symlink, junction, ...) and non-trivial to
    // figure out so just report it as unknown. Should we ever want this then
    // the proper code can be found in msvc's std::filesystem implementation.
    if (d_type & FILE_ATTRIBUTE_REPARSE_POINT) {
        return 0;
    }
    return (d_type & FILE_ATTRIBUTE_DIRECTORY) ? MP_S_IFDIR : MP_S_IFREG;
}
