#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "memzip.h"

extern uint8_t _staticfs[];

MEMZIP_RESULT memzip_locate(const char *filename, void **data, size_t *len)
{
    const MEMZIP_FILE_HDR *file_hdr = (const MEMZIP_FILE_HDR *)_staticfs;
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
            if (file_hdr->compression_method != 0) {
                return MZ_FILE_COMPRESSED;
            }

            *data = mem_data;
            *len = file_hdr->uncompressed_size;
            return MZ_OK;
        }
        mem_data += file_hdr->uncompressed_size;
        file_hdr = (const MEMZIP_FILE_HDR *)mem_data;
    }
    return MZ_NO_FILE;
}
