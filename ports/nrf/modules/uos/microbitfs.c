/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Mark Shannon
 * Copyright (c) 2017 Ayke van Laethem
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

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "microbitfs.h"
#include "drivers/flash.h"
#include "modrandom.h"
#include "py/obj.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "mpconfigport.h"

#if MICROPY_MBFS

#define DEBUG_FILE 0
#if DEBUG_FILE
#define DEBUG(s) printf s
#else
#define DEBUG(s) (void)0
#endif

/**  How it works:
 * The File System consists of up to MAX_CHUNKS_IN_FILE_SYSTEM chunks of CHUNK_SIZE each,
 * plus one spare page which holds persistent configuration data and is used. for bulk erasing.
 * The spare page is either the first or the last page and will be switched by a bulk erase.
 * The exact number of chunks will depend on the amount of flash available.
 *
 * Each chunk consists of a one byte marker and a one byte tail
 * The marker shows whether this chunk is the start of a file, the midst of a file
 * (in which case it refers to the previous chunk in the file) or whether it is UNUSED
 * (and erased) or FREED (which means it is unused, but not erased).
 * Chunks are selected in a randomised round-robin fashion to even out wear on the flash
 * memory as much as possible.
 * A file consists of a linked list of chunks. The first chunk in a file contains its name
 * as well as the end chunk and offset.
 * Files are found by linear search of the chunks, this means that no meta-data needs to be stored
 * outside of the file, which prevents wear hot-spots. Since there are fewer than 250 chunks,
 * the search is fast enough.
 *
 * Chunks are numbered from 1 as we need to reserve 0 as the FREED marker.
 *
 * Writing to files relies on the persistent API which is high-level wrapper on top of the Nordic SDK.
 */

#define CHUNK_SIZE (1<<MBFS_LOG_CHUNK_SIZE)
#define DATA_PER_CHUNK (CHUNK_SIZE-2)

#define UNUSED_CHUNK 255
#define FREED_CHUNK  0
#define FILE_START 254
#define PERSISTENT_DATA_MARKER 253

/** Must be such that sizeof(file_header) < DATA_PER_CHUNK */
#define MAX_FILENAME_LENGTH 120

//Minimum number of free chunks to justify sweeping.
//If this is too low it may cause excessive wear
#define MIN_CHUNKS_FOR_SWEEP (FLASH_PAGESIZE / CHUNK_SIZE)

#define FILE_NOT_FOUND ((uint8_t)-1)

/** Maximum number of chunks allowed in filesystem. 252 chunks is 31.5kB */
#define MAX_CHUNKS_IN_FILE_SYSTEM 252

#define STATIC_ASSERT(e) extern char static_assert_failed[(e) ? 1 : -1]

typedef struct _file_descriptor_obj {
    mp_obj_base_t base;
    uint8_t start_chunk;
    uint8_t seek_chunk;
    uint8_t seek_offset;
    bool writable;
    bool open;
    bool binary;
} file_descriptor_obj;

typedef struct _file_header {
    uint8_t end_offset;
    uint8_t name_len;
    char filename[MAX_FILENAME_LENGTH];
} file_header;

typedef struct _file_chunk {
    uint8_t marker;
    union {
        char data[DATA_PER_CHUNK];
        file_header header;
    };
    uint8_t next_chunk;
} file_chunk;

typedef struct _persistent_config_t {
    // Must start with a marker, so that we can identify it.
    uint8_t marker; // Should always be PERSISTENT_DATA_MARKER
} persistent_config_t;

extern const mp_obj_type_t uos_mbfs_fileio_type;
extern const mp_obj_type_t uos_mbfs_textio_type;

// Page indexes count down from the end of ROM.
STATIC uint8_t first_page_index;
STATIC uint8_t last_page_index;
// The number of useable chunks in the file system.
STATIC uint8_t chunks_in_file_system;
// Index of chunk to start searches. This is randomised to even out wear.
STATIC uint8_t start_index;
STATIC file_chunk *file_system_chunks;

// Defined by the linker
extern byte _fs_start[];
extern byte _fs_end[];

STATIC_ASSERT((sizeof(file_chunk) == CHUNK_SIZE));

// From micro:bit memory.h
STATIC inline byte *rounddown(byte *addr, uint32_t align) {
    return (byte*)(((uint32_t)addr)&(-align));
}

// From micro:bit memory.h
STATIC inline byte *roundup(byte *addr, uint32_t align) {
    return (byte*)((((uint32_t)addr)+align-1)&(-align));
}


STATIC inline void *first_page(void) {
    return _fs_end - FLASH_PAGESIZE * first_page_index;
}

STATIC inline void *last_page(void) {
    return _fs_end - FLASH_PAGESIZE * last_page_index;
}

STATIC void init_limits(void) {
    // First determine where to end
    byte *end = _fs_end;
    end = rounddown(end, FLASH_PAGESIZE)-FLASH_PAGESIZE;
    last_page_index = (_fs_end - end)/FLASH_PAGESIZE;

    // Now find the start
    byte *start = roundup(end - CHUNK_SIZE*MAX_CHUNKS_IN_FILE_SYSTEM, FLASH_PAGESIZE);
    while (start < _fs_start) {
        start += FLASH_PAGESIZE;
    }
    first_page_index = (_fs_end - start)/FLASH_PAGESIZE;
    chunks_in_file_system = (end-start)>>MBFS_LOG_CHUNK_SIZE;
}

STATIC void randomise_start_index(void) {
    start_index = machine_rng_generate_random_word() % chunks_in_file_system + 1;
}

void microbit_filesystem_init(void) {
    init_limits();
    randomise_start_index();
    file_chunk *base = first_page();
    if (base->marker == PERSISTENT_DATA_MARKER) {
        file_system_chunks = &base[(FLASH_PAGESIZE>>MBFS_LOG_CHUNK_SIZE)-1];
    } else if (((file_chunk *)last_page())->marker == PERSISTENT_DATA_MARKER) {
        file_system_chunks = &base[-1];
    } else {
        flash_write_byte((uint32_t)&((file_chunk *)last_page())->marker, PERSISTENT_DATA_MARKER);
        file_system_chunks = &base[-1];
    }
}

STATIC void copy_page(void *dest, void *src) {
    DEBUG(("FILE DEBUG: Copying page from %lx to %lx.\r\n", (uint32_t)src, (uint32_t)dest));
    flash_page_erase((uint32_t)dest);
    file_chunk *src_chunk = src;
    file_chunk *dest_chunk = dest;
    uint32_t chunks = FLASH_PAGESIZE>>MBFS_LOG_CHUNK_SIZE;
    for (uint32_t i = 0; i < chunks; i++) {
        if (src_chunk[i].marker != FREED_CHUNK) {
            flash_write_bytes((uint32_t)&dest_chunk[i], (uint8_t*)&src_chunk[i], CHUNK_SIZE);
        }
    }
}

// Move entire file system up or down one page, copying all used chunks
// Freed chunks are not copied, so become erased.
// There should be no erased chunks before the sweep (or it would be unnecessary)
// but if there are this should work correctly.
//
// The direction of the sweep depends on whether the persistent data is in the first or last page
// The persistent data is copied to RAM, leaving its page unused.
// Then all the pages are copied, one by one, into the adjacent newly unused page.
// Finally, the persistent data is saved back to the opposite end of the filesystem from whence it came.
//
STATIC void filesystem_sweep(void) {
    persistent_config_t config;
    uint8_t *page;
    uint8_t *end_page;
    int step;
    uint32_t page_size = FLASH_PAGESIZE;
    DEBUG(("FILE DEBUG: Sweeping file system\r\n"));
    if (((file_chunk *)first_page())->marker == PERSISTENT_DATA_MARKER) {
        config = *(persistent_config_t *)first_page();
        page = first_page();
        end_page = last_page();
        step = page_size;
    } else {
        config = *(persistent_config_t *)last_page();
        page = last_page();
        end_page = first_page();
        step = -page_size;
    }
    while (page != end_page) {
        uint8_t *next_page = page+step;
        flash_page_erase((uint32_t)page);
        copy_page(page, next_page);
        page = next_page;
    }
    flash_page_erase((uint32_t)end_page);
    flash_write_bytes((uint32_t)end_page, (uint8_t*)&config, sizeof(config));
    microbit_filesystem_init();
}


STATIC inline byte *seek_address(file_descriptor_obj *self) {
    return (byte*)&(file_system_chunks[self->seek_chunk].data[self->seek_offset]);
}

STATIC uint8_t microbit_find_file(const char *name, int name_len) {
    for (uint8_t index = 1; index <= chunks_in_file_system; index++) {
        const file_chunk *p = &file_system_chunks[index];
        if (p->marker != FILE_START)
            continue;
        if (p->header.name_len != name_len)
            continue;
        if (memcmp(name, &p->header.filename[0], name_len) == 0) {
            DEBUG(("FILE DEBUG: File found. index %d\r\n", index));
            return index;
        }
    }
    DEBUG(("FILE DEBUG: File not found.\r\n"));
    return FILE_NOT_FOUND;
}

// Return a free, erased chunk.
// Search the chunks:
// 1  If an UNUSED chunk is found, then return that.
// 2. If an entire page of FREED chunks is found, then erase the page and return the first chunk
// 3. If the number of FREED chunks is >= MIN_CHUNKS_FOR_SWEEP, then
// 3a. Sweep the filesystem and restart.
// 3b. Fail and return FILE_NOT_FOUND
//
STATIC uint8_t find_chunk_and_erase(void) {
    // Start search at a random chunk to spread the wear more evenly.
    // Search for unused chunk
    uint8_t index = start_index;
    do {
        const file_chunk *p = &file_system_chunks[index];
        if (p->marker == UNUSED_CHUNK) {
            DEBUG(("FILE DEBUG: Unused chunk found: %d\r\n", index));
            return index;
        }
        index++;
        if (index == chunks_in_file_system+1) index = 1;
    } while (index != start_index);

    // Search for FREED page, and total up FREED chunks
    uint32_t freed_chunks = 0;
    index = start_index;
    uint32_t chunks_per_page = FLASH_PAGESIZE>>MBFS_LOG_CHUNK_SIZE;
    do {
        const file_chunk *p = &file_system_chunks[index];
        if (p->marker == FREED_CHUNK) {
            freed_chunks++;
        }
        if (FLASH_IS_PAGE_ALIGNED(p)) {
            uint32_t i;
            for (i = 0; i < chunks_per_page; i++) {
                if (p[i].marker != FREED_CHUNK)
                    break;
            }
            if (i == chunks_per_page) {
                DEBUG(("FILE DEBUG: Found freed page of chunks: %d\r\n", index));
                flash_page_erase((uint32_t)&file_system_chunks[index]);
                return index;
            }
        }
        index++;
        if (index == chunks_in_file_system+1) index = 1;
    } while (index != start_index);
    DEBUG(("FILE DEBUG: %lu free chunks\r\n", freed_chunks));
    if (freed_chunks < MIN_CHUNKS_FOR_SWEEP) {
        return FILE_NOT_FOUND;
    }
    // No freed pages, so sweep file system.
    filesystem_sweep();
    // This is guaranteed to succeed.
    return find_chunk_and_erase();
}

STATIC mp_obj_t microbit_file_name(file_descriptor_obj *fd) {
    return mp_obj_new_str(&(file_system_chunks[fd->start_chunk].header.filename[0]), file_system_chunks[fd->start_chunk].header.name_len);
}

STATIC file_descriptor_obj *microbit_file_descriptor_new(uint8_t start_chunk, bool write, bool binary);

STATIC void clear_file(uint8_t chunk) {
    do {
        flash_write_byte((uint32_t)&(file_system_chunks[chunk].marker), FREED_CHUNK);
        DEBUG(("FILE DEBUG: Freeing chunk %d.\n", chunk));
        chunk = file_system_chunks[chunk].next_chunk;
    } while (chunk <= chunks_in_file_system);
}

STATIC file_descriptor_obj *microbit_file_open(const char *name, size_t name_len, bool write, bool binary) {
    if (name_len > MAX_FILENAME_LENGTH) {
        return NULL;
    }
    uint8_t index = microbit_find_file(name, name_len);
    if (write) {
        if (index != FILE_NOT_FOUND) {
            // Free old file
            clear_file(index);
        }
        index = find_chunk_and_erase();
        if (index == FILE_NOT_FOUND) {
            mp_raise_OSError(MP_ENOSPC);
        }
        flash_write_byte((uint32_t)&(file_system_chunks[index].marker), FILE_START);
        flash_write_byte((uint32_t)&(file_system_chunks[index].header.name_len), name_len);
        flash_write_bytes((uint32_t)&(file_system_chunks[index].header.filename[0]), (uint8_t*)name, name_len);
    } else {
        if (index == FILE_NOT_FOUND) {
            return NULL;
        }
    }
    return microbit_file_descriptor_new(index, write, binary);
}

STATIC file_descriptor_obj *microbit_file_descriptor_new(uint8_t start_chunk, bool write, bool binary) {
    file_descriptor_obj *res = m_new_obj(file_descriptor_obj);
    if (binary) {
        res->base.type = &uos_mbfs_fileio_type;
    } else {
        res->base.type = &uos_mbfs_textio_type;
    }
    res->start_chunk = start_chunk;
    res->seek_chunk = start_chunk;
    res->seek_offset = file_system_chunks[start_chunk].header.name_len+2;
    res->writable = write;
    res->open = true;
    res->binary = binary;
    return res;
}

STATIC mp_obj_t microbit_remove(mp_obj_t filename) {
    mp_uint_t name_len;
    const char *name = mp_obj_str_get_data(filename, &name_len);
    mp_uint_t index = microbit_find_file(name, name_len);
    if (index == 255) {
        mp_raise_OSError(MP_ENOENT);
    }
    clear_file(index);
    return mp_const_none;
}

STATIC void check_file_open(file_descriptor_obj *self) {
    if (!self->open) {
        mp_raise_ValueError("I/O operation on closed file");
    }
}

STATIC int advance(file_descriptor_obj *self, uint32_t n, bool write) {
    DEBUG(("FILE DEBUG: Advancing from chunk %d, offset %d.\r\n", self->seek_chunk, self->seek_offset));
    self->seek_offset += n;
    if (self->seek_offset == DATA_PER_CHUNK) {
        self->seek_offset = 0;
        if (write) {
            uint8_t next_chunk = find_chunk_and_erase();
            if (next_chunk == FILE_NOT_FOUND) {
                clear_file(self->start_chunk);
                self->open = false;
                return MP_ENOSPC;
            }
            // Link next chunk to this one
            flash_write_byte((uint32_t)&(file_system_chunks[self->seek_chunk].next_chunk), next_chunk);
            flash_write_byte((uint32_t)&(file_system_chunks[next_chunk].marker), self->seek_chunk);
        }
        self->seek_chunk = file_system_chunks[self->seek_chunk].next_chunk;
    }
    DEBUG(("FILE DEBUG: Advanced to chunk %d, offset %d.\r\n", self->seek_chunk, self->seek_offset));
    return 0;
}

STATIC mp_uint_t microbit_file_read(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode) {
    file_descriptor_obj *self = (file_descriptor_obj *)obj;
    check_file_open(self);
    if (self->writable || file_system_chunks[self->start_chunk].marker == FREED_CHUNK) {
        *errcode = MP_EBADF;
        return MP_STREAM_ERROR;
    }
    uint32_t bytes_read = 0;
    uint8_t *data = buf;
    while (1) {
        mp_uint_t to_read = DATA_PER_CHUNK - self->seek_offset;
        if (file_system_chunks[self->seek_chunk].next_chunk == UNUSED_CHUNK) {
            uint8_t end_offset = file_system_chunks[self->start_chunk].header.end_offset;
            if (end_offset == UNUSED_CHUNK) {
                to_read = 0;
            } else {
                to_read = MIN(to_read, (mp_uint_t)end_offset-self->seek_offset);
            }
        }
        to_read = MIN(to_read, size-bytes_read);
        if (to_read == 0) {
            break;
        }
        memcpy(data+bytes_read, seek_address(self), to_read);
        advance(self, to_read, false);
        bytes_read += to_read;
    }
    return bytes_read;
}

STATIC mp_uint_t microbit_file_write(mp_obj_t obj, const void *buf, mp_uint_t size, int *errcode) {
    file_descriptor_obj *self = (file_descriptor_obj *)obj;
    check_file_open(self);
    if (!self->writable || file_system_chunks[self->start_chunk].marker == FREED_CHUNK) {
        *errcode = MP_EBADF;
        return MP_STREAM_ERROR;
    }
    uint32_t len = size;
    const uint8_t *data = buf;
    while (len) {
        uint32_t to_write = MIN(((uint32_t)(DATA_PER_CHUNK - self->seek_offset)), len);
        flash_write_bytes((uint32_t)seek_address(self), data, to_write);
        int err = advance(self, to_write, true);
        if (err) {
            *errcode = err;
            return MP_STREAM_ERROR;
        }
        data += to_write;
        len -= to_write;
    }
    return size;
}

STATIC void microbit_file_close(file_descriptor_obj *fd) {
    if (fd->writable) {
        flash_write_byte((uint32_t)&(file_system_chunks[fd->start_chunk].header.end_offset), fd->seek_offset);
    }
    fd->open = false;
}

STATIC mp_obj_t microbit_file_list(void) {
    mp_obj_t res = mp_obj_new_list(0, NULL);
    for (uint8_t index = 1; index <= chunks_in_file_system; index++) {
        if (file_system_chunks[index].marker == FILE_START) {
            mp_obj_t name = mp_obj_new_str(&file_system_chunks[index].header.filename[0], file_system_chunks[index].header.name_len);
            mp_obj_list_append(res, name);
        }
    }
    return res;
}

STATIC mp_obj_t microbit_file_size(mp_obj_t filename) {
    mp_uint_t name_len;
    const char *name = mp_obj_str_get_data(filename, &name_len);
    uint8_t chunk = microbit_find_file(name, name_len);
    if (chunk == 255) {
        mp_raise_OSError(MP_ENOENT);
    }
    mp_uint_t len = 0;
    uint8_t end_offset = file_system_chunks[chunk].header.end_offset;
    uint8_t offset = file_system_chunks[chunk].header.name_len+2;
    while (file_system_chunks[chunk].next_chunk != UNUSED_CHUNK) {
        len += DATA_PER_CHUNK - offset;
        chunk = file_system_chunks[chunk].next_chunk;
        offset = 0;
    }
    len += end_offset - offset;
    return mp_obj_new_int(len);
}

STATIC mp_uint_t file_read_byte(file_descriptor_obj *fd) {
    if (file_system_chunks[fd->seek_chunk].next_chunk == UNUSED_CHUNK) {
        uint8_t end_offset = file_system_chunks[fd->start_chunk].header.end_offset;
        if (end_offset == UNUSED_CHUNK || fd->seek_offset == end_offset) {
            return (mp_uint_t)-1;
        }
    }
    mp_uint_t res = file_system_chunks[fd->seek_chunk].data[fd->seek_offset];
    advance(fd, 1, false);
    return res;
}

// Now follows the code to integrate this filesystem into the uos module.

mp_lexer_t *uos_mbfs_new_reader(const char *filename) {
    file_descriptor_obj *fd = microbit_file_open(filename, strlen(filename), false, false);
    if (fd == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    mp_reader_t reader;
    reader.data = fd;
    reader.readbyte = (mp_uint_t(*)(void*))file_read_byte;
    reader.close = (void(*)(void*))microbit_file_close; // no-op
    return mp_lexer_new(qstr_from_str(filename), reader);
}

mp_import_stat_t uos_mbfs_import_stat(const char *path) {
    uint8_t chunk = microbit_find_file(path, strlen(path));
    if (chunk == FILE_NOT_FOUND) {
        return MP_IMPORT_STAT_NO_EXIST;
    } else {
        return MP_IMPORT_STAT_FILE;
    }
}

STATIC mp_obj_t uos_mbfs_file_name(mp_obj_t self) {
    file_descriptor_obj *fd = (file_descriptor_obj*)self;
    return microbit_file_name(fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uos_mbfs_file_name_obj, uos_mbfs_file_name);

STATIC mp_obj_t uos_mbfs_file_close(mp_obj_t self) {
    file_descriptor_obj *fd = (file_descriptor_obj*)self;
    microbit_file_close(fd);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uos_mbfs_file_close_obj, uos_mbfs_file_close);

STATIC mp_obj_t uos_mbfs_remove(mp_obj_t name) {
    return microbit_remove(name);
}
MP_DEFINE_CONST_FUN_OBJ_1(uos_mbfs_remove_obj, uos_mbfs_remove);

STATIC mp_obj_t uos_mbfs_file___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return uos_mbfs_file_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uos_mbfs_file___exit___obj, 4, 4, uos_mbfs_file___exit__);

typedef struct {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    uint8_t index;
} uos_mbfs_ilistdir_it_t;

STATIC mp_obj_t uos_mbfs_ilistdir_it_iternext(mp_obj_t self_in) {
    uos_mbfs_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    // Read until the next FILE_START chunk.
    for (; self->index <= chunks_in_file_system; self->index++) {
        if (file_system_chunks[self->index].marker != FILE_START) {
            continue;
        }

        // Get the file name as str object.
        mp_obj_t name = mp_obj_new_str(&file_system_chunks[self->index].header.filename[0], file_system_chunks[self->index].header.name_len);

        // make 3-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
        t->items[0] = name;
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG); // all entries are files
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number

        self->index++;
        return MP_OBJ_FROM_PTR(t);
    }

    return MP_OBJ_STOP_ITERATION;
}

STATIC mp_obj_t uos_mbfs_ilistdir(void) {
    uos_mbfs_ilistdir_it_t *iter = m_new_obj(uos_mbfs_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = uos_mbfs_ilistdir_it_iternext;
    iter->index = 1;

    return MP_OBJ_FROM_PTR(iter);
}
MP_DEFINE_CONST_FUN_OBJ_0(uos_mbfs_ilistdir_obj, uos_mbfs_ilistdir);

MP_DEFINE_CONST_FUN_OBJ_0(uos_mbfs_listdir_obj, microbit_file_list);

STATIC mp_obj_t microbit_file_writable(mp_obj_t self) {
    return mp_obj_new_bool(((file_descriptor_obj *)self)->writable);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(microbit_file_writable_obj, microbit_file_writable);

STATIC const mp_map_elem_t uos_mbfs_file_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&uos_mbfs_file_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_name), (mp_obj_t)&uos_mbfs_file_name_obj },
    { MP_ROM_QSTR(MP_QSTR___enter__), (mp_obj_t)&mp_identity_obj },
    { MP_ROM_QSTR(MP_QSTR___exit__), (mp_obj_t)&uos_mbfs_file___exit___obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writable), (mp_obj_t)&microbit_file_writable_obj },
    /* Stream methods */
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj},
};
STATIC MP_DEFINE_CONST_DICT(uos_mbfs_file_locals_dict, uos_mbfs_file_locals_dict_table);


STATIC const mp_stream_p_t textio_stream_p = {
    .read = microbit_file_read,
    .write = microbit_file_write,
    .is_text = true,
};

const mp_obj_type_t uos_mbfs_textio_type = {
    { &mp_type_type },
    .name = MP_QSTR_TextIO,
    .protocol = &textio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&uos_mbfs_file_locals_dict,
};


STATIC const mp_stream_p_t fileio_stream_p = {
    .read = microbit_file_read,
    .write = microbit_file_write,
};

const mp_obj_type_t uos_mbfs_fileio_type = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .protocol = &fileio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&uos_mbfs_file_locals_dict,
};

// From micro:bit fileobj.c
mp_obj_t uos_mbfs_open(size_t n_args, const mp_obj_t *args) {
    /// -1 means default; 0 explicitly false; 1 explicitly true.
    int read = -1;
    int text = -1;
    if (n_args == 2) {
        mp_uint_t len;
        const char *mode = mp_obj_str_get_data(args[1], &len);
        for (mp_uint_t i = 0; i < len; i++) {
            if (mode[i] == 'r' || mode[i] == 'w') {
                if (read >= 0) {
                    goto mode_error;
                }
                read = (mode[i] == 'r');
            } else if (mode[i] == 'b' || mode[i] == 't') {
                if (text >= 0) {
                    goto mode_error;
                }
                text = (mode[i] == 't');
            } else {
                goto mode_error;
            }
        }
    }
    mp_uint_t name_len;
    const char *filename = mp_obj_str_get_data(args[0], &name_len);
    file_descriptor_obj *res = microbit_file_open(filename, name_len, read == 0, text == 0);
    if (res == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    return res;
mode_error:
    mp_raise_ValueError("illegal mode");
}

STATIC mp_obj_t uos_mbfs_stat(mp_obj_t filename) {
    mp_obj_t file_size = microbit_file_size(filename);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = file_size;               // st_size
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // st_atime
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // st_mtime
    t->items[9] = MP_OBJ_NEW_SMALL_INT(0); // st_ctime
    return MP_OBJ_FROM_PTR(t);
}
MP_DEFINE_CONST_FUN_OBJ_1(uos_mbfs_stat_obj, uos_mbfs_stat);

#endif // MICROPY_MBFS
