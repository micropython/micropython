#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "py/mpconfig.h"
#include "py/misc.h"
#include "memzip.h"

extern uint8_t memzip_data[];

const MEMZIP_FILE_HDR *memzip_find_file_header(const char *filename) {

    const MEMZIP_FILE_HDR *file_hdr = (const MEMZIP_FILE_HDR *)memzip_data;
    uint8_t *mem_data;

    /* Zip file filenames don't have a leading /, so we strip it off */

    if (*filename == '/') {
        filename++;
    }
    while (file_hdr->signature == MEMZIP_FILE_HEADER_SIGNATURE) {
        const char *file_hdr_filename = (const char *)&file_hdr[1];
        mem_data = (uint8_t *)file_hdr_filename;
        mem_data += file_hdr->filename_len;
        mem_data += file_hdr->extra_len;
        if (!strncmp(file_hdr_filename, filename, file_hdr->filename_len)) {
            /* We found a match */
            return file_hdr;
        }
        mem_data += file_hdr->uncompressed_size;
        file_hdr = (const MEMZIP_FILE_HDR *)mem_data;
    }
    return NULL;
}

bool memzip_is_dir(const char *filename) {
    const MEMZIP_FILE_HDR *file_hdr = (const MEMZIP_FILE_HDR *)memzip_data;
    uint8_t *mem_data;

    if (strcmp(filename, "/") == 0) {
        // The root directory is a directory.
        return true;
    }

    // Zip filenames don't have a leading /, so we strip it off
    if (*filename == '/') {
        filename++;
    }
    size_t filename_len = strlen(filename);

    while (file_hdr->signature == MEMZIP_FILE_HEADER_SIGNATURE) {
        const char *file_hdr_filename = (const char *)&file_hdr[1];
        if (filename_len < file_hdr->filename_len &&
            strncmp(file_hdr_filename, filename, filename_len) == 0 &&
            file_hdr_filename[filename_len] == '/') {
            return true;
        }

        mem_data = (uint8_t *)file_hdr_filename;
        mem_data += file_hdr->filename_len;
        mem_data += file_hdr->extra_len;
        mem_data += file_hdr->uncompressed_size;
        file_hdr = (const MEMZIP_FILE_HDR *)mem_data;
    }
    return NULL;

}

MEMZIP_RESULT memzip_locate(const char *filename, void **data, size_t *len)
{
    const MEMZIP_FILE_HDR *file_hdr = memzip_find_file_header(filename);
    if (file_hdr == NULL) {
        return MZ_NO_FILE;
    }
    if (file_hdr->compression_method != 0) {
        return MZ_FILE_COMPRESSED;
    }

    uint8_t *mem_data;
    mem_data = (uint8_t *)&file_hdr[1];
    mem_data += file_hdr->filename_len;
    mem_data += file_hdr->extra_len;

    *data = mem_data;
    *len = file_hdr->uncompressed_size;
    return MZ_OK;
}

MEMZIP_RESULT memzip_stat(const char *path, MEMZIP_FILE_INFO *info) {
    const MEMZIP_FILE_HDR *file_hdr = memzip_find_file_header(path);
    if (file_hdr == NULL) {
        if (memzip_is_dir(path)) {
            info->file_size = 0;
            info->last_mod_date = 0;
            info->last_mod_time = 0;
            info->is_dir = 1;
            return MZ_OK;
        }
        return MZ_NO_FILE;
    }
    info->file_size = file_hdr->uncompressed_size;
    info->last_mod_date = file_hdr->last_mod_date;
    info->last_mod_time = file_hdr->last_mod_time;
    info->is_dir = 0;

    return MZ_OK;
}
