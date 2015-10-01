#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "py/mpconfig.h"
#include "py/misc.h"
#include "memzip_extras.h"

extern uint8_t _staticfs[];

const MEMZIP_FILE_HDR *memzip_iterate(memzip_iterate_callback_t *callback, void *user) {

    const MEMZIP_FILE_HDR *file_hdr = (const MEMZIP_FILE_HDR *)_staticfs;
    uint8_t *mem_data;

    while (file_hdr->signature == MEMZIP_FILE_HEADER_SIGNATURE) {
        const char *file_hdr_filename = (const char *)&file_hdr[1];
        mem_data = (uint8_t *)file_hdr_filename;
        mem_data += file_hdr->filename_len;
        mem_data += file_hdr->extra_len;
		if (callback(file_hdr, file_hdr_filename, user) == 0)
			break;
        mem_data += file_hdr->uncompressed_size;
        file_hdr = (const MEMZIP_FILE_HDR *)mem_data;
    }
	return file_hdr;
}
