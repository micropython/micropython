/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Dave Hylands
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

#include "extmod/moduzipfile.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "py/builtin.h"
#include "py/mpfile.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_UZLIB
#include "uzlib/tinf.h"
#endif

#if MICROPY_PY_UZIPFILE || MICROPY_PY_ZIPIMPORT

#if !MICROPY_PY_FILE_LIKE
#error MICROPY PY_UZIPFILE or MICRPPY PY_ZIPIMPORT needs MICROPY PY_FILE_LIKE to be set
#endif

#pragma pack(push, 1)

#if MP_ENDIANNESS_LITTLE
// Zip files are specified as little endian
#define ZF_UINT16(x)    (x)
#define ZF_UINT32(x)    (x)
#else
#define ZF_UINT16(x)    ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))
#define ZF_UINT32(x)    ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8) | (((x) & 0xff000000) >> 24))
#endif

#define ZIP_FILE_HEADER_SIGNATURE "PK\x03\x04"
typedef struct
{
    uint32_t    signature;
    uint16_t    version;
    uint16_t    flags;
    uint16_t    compression_method;
    uint16_t    last_mod_time;
    uint16_t    last_mod_date;
    uint32_t    crc32;
    uint32_t    compressed_size;
    uint32_t    uncompressed_size;
    uint16_t    filename_len;
    uint16_t    extra_len;

    /* char     filename[filename_len]  */
    /* uint8_t  extra[extra_len]        */

} ZIP_FILE_HDR;

#define ZIP_CENTRAL_DIRECTORY_SIGNATURE "PK\x01\x02"
typedef struct
{
    uint32_t    signature;
    uint16_t    version_made_by;
    uint16_t    version_read_with;
    uint16_t    flags;
    uint16_t    compression_method;
    uint16_t    last_mod_time;
    uint16_t    last_mod_date;
    uint32_t    crc32;
    uint32_t    compressed_size;
    uint32_t    uncompressed_size;
    uint16_t    filename_len;
    uint16_t    extra_len;
    uint16_t    file_comment_len;
    uint16_t    disk_num;
    uint16_t    internal_file_attributes;
    uint32_t    external_file_attributes;
    uint32_t    file_header_offset;

    /* char     filename[filename_len]  */
    /* uint8_t  extra[extra_len]        */

} ZIP_CENTRAL_DIRECTORY_HDR;

#define ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE "PK\x05\x06"
typedef struct
{
    uint32_t    signature;
    uint16_t    disk_num;
    uint16_t    central_directory_disk;
    uint16_t    num_central_directories_this_disk;
    uint16_t    total_central_directories;
    uint32_t    central_directory_size;
    uint32_t    central_directory_offset;
    uint16_t    comment_len;

    /* char     comment[comment_len]    */

} ZIP_END_OF_CENTRAL_DIRECTORY;

#pragma pack(pop)

typedef struct {
    uint32_t    file_size;
    uint16_t    last_mod_date;
    uint16_t    last_mod_time;
    uint8_t     is_dir;

} ZIP_FILE_INFO;

#define ZIP_STORED      0
#define ZIP_DEFLATED    8

typedef struct {
    mp_obj_base_t   base;
    const char     *filename;
    mp_file_t      *file;
    off_t           end_directory_posn;
    off_t           directory_posn;
    off_t           first_file_posn;
} ZipFile_t;

typedef struct {
    mp_obj_base_t   base;
    ZipFile_t      *zf;
    mp_obj_t        filename;
    off_t           seek_offset;
    off_t           data_posn;
    uint32_t        compressed_size;
    uint32_t        uncompressed_size;
    uint8_t        *uncompressed_data;
} ZipExtFile_t;

STATIC const mp_obj_type_t ZipFile_type;
STATIC const mp_obj_type_t ZipExtFile_type;

STATIC off_t zf_find_end_of_central_directory(ZipFile_t *zf);
STATIC ZipFile_t *zipfile_open_file_or_filename(const char *filename, mp_obj_t file_or_filename);

STATIC uint zipfile_find_archive(ZipFile_t *zf, const char *archive_name, ZIP_FILE_HDR *file_hdr, off_t *out_posn);
STATIC mp_obj_t zipfile_open_archive(ZipFile_t *zf, const char *archive_name, ZIP_FILE_HDR *file_hdr, off_t posn);

STATIC void zipfile_close(ZipFile_t *zf);

#if MICROPY_PY_ZIPIMPORT
// Special helper functions used for implementing zipfile import. If zip_path
// contains .zip then everything up until the .zip is considered to be the name
// of the zipfile, and everything after is considered to the archive name.

// Takes a path which combines the .zip filename and archive name, and returns
// a zipfile handle if the zipfile was opened successfully.
STATIC ZipFile_t *zipfile_import_open(const char *zip_archive_path, const char **archive_name) {
    *archive_name = NULL;
    char *s = strstr(zip_archive_path, ".zip/");
    if (s == NULL) {
        // No .zip in path, so we assume that there is no zipfile.
        return NULL;
    }
    size_t len = s - zip_archive_path + 4;  // +4 for .zip
    char *zip_path = m_new(char, len + 1);
    memcpy(zip_path, zip_archive_path, len);
    zip_path[len] = '\0';

    ZipFile_t *zf = zipfile_open_file_or_filename(zip_path, mp_const_none);
    m_del(char, zip_path, len + 1);
    zip_path = NULL;

    if (zf->end_directory_posn < 0) {
        // No end central directory - this isn't a zipfile.
        zipfile_close(zf);
        zf = NULL;
        return NULL;
    }
    *archive_name = &s[5];
    return zf;
}

// Takes a path which combines the .zip filename and archive name, and returns
// an indicator of whether the archive exists, and whether its a directory or
// a file.
STATIC mp_import_stat_t zipfile_import_core(const char *zip_archive_path, mp_obj_t *zef) {
    if (zef) {
        *zef = mp_const_none;
    }
    mp_import_stat_t imp_stat = MP_IMPORT_STAT_NO_EXIST;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        const char *archive_name;
        ZipFile_t *zf = zipfile_import_open(zip_archive_path, &archive_name);
        if (zf) {
            ZIP_FILE_HDR file_hdr;
            off_t posn;
            imp_stat = zipfile_find_archive(zf, archive_name, &file_hdr, &posn);
            if (zef && imp_stat == MP_IMPORT_STAT_FILE) {
                *zef = zipfile_open_archive(zf, archive_name, &file_hdr, posn);
            } else {
                zipfile_close(zf);
            }
        }
        zf = NULL;
        archive_name = NULL;
        nlr_pop();
    }
    return imp_stat;
}

// Takes a path which combines the .zip filename and archive name, and returns
// a file object if the archive was opened successfully.
mp_import_stat_t zipfile_import_stat(const char *zip_archive_path) {
    return zipfile_import_core(zip_archive_path, NULL);
}

// Takes a path which combines the .zip filename and archive name, and returns
// a file object if the archive was opened successfully.
mp_obj_t zipfile_import_open_archive(const char *zip_archive_path) {
    mp_obj_t zef;
    zipfile_import_core(zip_archive_path, &zef);
    return zef;
}

#endif // MICROPY_PY_ZIPIMPORT

STATIC void zipfile_close(ZipFile_t *zf) {
    mp_close(zf->file);
    zf->file = NULL;
    zf->filename = NULL;
    zf->end_directory_posn = -1;
    zf->directory_posn = -1;
    zf->first_file_posn = -1;
    m_del(ZipFile_t, zf, 1);
}

STATIC ZipFile_t *zipfile_open_file_or_filename(const char *filename, mp_obj_t file_or_filename) {
    ZipFile_t *zf = m_new0(ZipFile_t, 1);
    zf->base.type = &ZipFile_type;

    if (filename) {
        zf->filename = filename;
        zf->file = mp_open(zf->filename, "rb");
    } else if (MP_OBJ_IS_STR(file_or_filename)) {
        zf->filename = mp_obj_str_get_str(file_or_filename);
        zf->file = mp_open(zf->filename, "rb");
    } else {
        zf->filename = "file obj";
        zf->file = mp_file_from_file_obj(file_or_filename);
    }

    zf_find_end_of_central_directory(zf);
    zf->directory_posn = -1;
    zf->first_file_posn = -1;
    return zf;
}

STATIC off_t zf_find_end_of_central_directory(ZipFile_t *zf) {
    // The end-of-central-directory is located at the end of the zip file,
    // but may be followed by a variable length comment (max len 64K).

    zf->end_directory_posn = -1;

    off_t filesize = mp_seek(zf->file, 0, MP_SEEK_END);
    if (filesize < (off_t)sizeof(ZIP_END_OF_CENTRAL_DIRECTORY)) {
        return -1;
    }
    off_t posn = mp_seek(zf->file, -sizeof(ZIP_END_OF_CENTRAL_DIRECTORY), MP_SEEK_END);
    byte signature[4];
    if (mp_readinto(zf->file, &signature, sizeof(signature)) == sizeof(signature)
    &&  memcmp(signature, ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE, sizeof(signature)) == 0) {
        zf->end_directory_posn = posn;
        return zf->end_directory_posn;
    }

    // TODO: In the general case, the End of Central Directory may be followed
    // by a variable length comment. So we should search backwards in the file
    // for upto 64K to try and find the End of Central Directory.
    //
    // For now, we don't support zipfiles with a comment.

    return -1;
}

STATIC off_t zf_find_central_directory(ZipFile_t *zf) {
    zf->directory_posn = -1;
    if (zf->end_directory_posn < 0) {
        return -1;
    }

    mp_seek(zf->file, zf->end_directory_posn, MP_SEEK_SET);
    ZIP_END_OF_CENTRAL_DIRECTORY endDir;
    mp_readinto(zf->file, &endDir, sizeof(endDir));

    uint32_t cd_size = ZF_UINT32(endDir.central_directory_size);
    if (cd_size == 0) {
        // Empty ZIP file - no central directory
        return -1;
    }
    zf->directory_posn = ZF_UINT32(endDir.central_directory_offset);
    return zf->directory_posn;
}

STATIC off_t zf_find_first_file(ZipFile_t *zf) {
    if (zf_find_central_directory(zf) < 0) {
        return -1;
    }

    mp_seek(zf->file, zf->directory_posn, MP_SEEK_SET);
    ZIP_CENTRAL_DIRECTORY_HDR centralDir;
    mp_readinto(zf->file, &centralDir, sizeof(centralDir));

    zf->first_file_posn = ZF_UINT32(centralDir.file_header_offset);
    return zf->first_file_posn;
}

STATIC off_t zf_get_file_hdr(ZipFile_t *zf, off_t posn, ZIP_FILE_HDR *file_hdr) {
    mp_seek(zf->file, posn, MP_SEEK_SET);
    if (mp_readinto(zf->file, file_hdr, sizeof(*file_hdr)) != sizeof(*file_hdr)
    || memcmp(&file_hdr->signature, ZIP_FILE_HEADER_SIGNATURE, sizeof(file_hdr->signature)) != 0) {
        return -1;
    }

    // Calculate the position of the next file header and return it
    return posn + sizeof(*file_hdr)
         + ZF_UINT16(file_hdr->filename_len)
         + ZF_UINT16(file_hdr->extra_len)
         + ZF_UINT32(file_hdr->compressed_size);
}

STATIC uint zipfile_find_archive(ZipFile_t *zf, const char *archive_name, ZIP_FILE_HDR *file_hdr, off_t *out_posn) {
    mp_import_stat_t ret = MP_IMPORT_STAT_NO_EXIST;

    off_t posn = zf_find_central_directory(zf);
    if (posn >= 0) {
        posn = zf_find_first_file(zf);
    }
    // Directories stored in the archive end in /
    size_t fn_len = strlen(archive_name);
    char *fn_buf = m_new(char, fn_len + 2); // +1 for slash, +1 for null termination
    while (posn >= 0) {
        off_t next_posn = zf_get_file_hdr(zf, posn, file_hdr);
        if (next_posn >= 0) {
            uint16_t len = ZF_UINT16(file_hdr->filename_len);
            if (len <= fn_len) {
                mp_readinto(zf->file, fn_buf, len);
                fn_buf[len] = '\0';
            } else {
                mp_readinto(zf->file, fn_buf, fn_len + 1);
                fn_buf[fn_len + 1] = '\0';
            }

            // See if archive_name matches the directory portion
            if (strncmp(fn_buf, archive_name, fn_len) == 0 && fn_buf[fn_len] == '/') {
                ret = MP_IMPORT_STAT_DIR;
                break;
            }
            if (strcmp(fn_buf, archive_name) == 0) {
                ret = MP_IMPORT_STAT_FILE;
                break;
            }
        }
        posn = next_posn;
    }
    m_del(char, fn_buf, fn_len + 1);
    *out_posn = posn;
    return ret;
}

STATIC mp_obj_t zipfile_open_archive(ZipFile_t *zf, const char *archive_name, ZIP_FILE_HDR *file_hdr, off_t posn) {
    ZipExtFile_t *zef = m_new0(ZipExtFile_t, 1);
    zef->base.type = &ZipExtFile_type;
    zef->zf = zf;
    zef->filename = mp_obj_new_str(archive_name, strlen(archive_name), false);
    zef->seek_offset = 0;
    zef->data_posn = posn + sizeof(*file_hdr)
                   + ZF_UINT16(file_hdr->filename_len)
                   + ZF_UINT16(file_hdr->extra_len);
    zef->compressed_size = ZF_UINT32(file_hdr->compressed_size);
    zef->uncompressed_size = ZF_UINT32(file_hdr->uncompressed_size);

    uint16_t compression_method = ZF_UINT16(file_hdr->compression_method);
    if (compression_method == ZIP_STORED) {
        // We'll do file I/O as read is called.
#if MICROPY_PY_UZLIB
    } else if (compression_method == ZIP_DEFLATED) {
        // Since tinf only supports decompressing the whole thing, we read
        // the entire uncompressed data into memory, and decompress it.
        zef->uncompressed_data = m_new(uint8_t, zef->uncompressed_size);
        void *compressed = m_new(uint8_t, zef->compressed_size);
        mp_seek(zef->zf->file, zef->data_posn, MP_SEEK_SET);
        mp_int_t bytes_read = mp_readinto(zf->file, compressed, zef->compressed_size);
        if (bytes_read != zef->compressed_size) {
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EIO)));
        }
        unsigned int destlen = zef->uncompressed_size;
        int res = tinf_uncompress(zef->uncompressed_data, &destlen, compressed, zef->compressed_size);
        if (res != TINF_OK) {
            mp_int_t errcode = EILSEQ;  // Bad Data - we'll call it illegal byte sequence
            if (res == TINF_DEST_OVERFLOW) {
                errcode = EFBIG;        // The destination buffer should have been big enough.
            }
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(errcode)));
        }
        // It should always come out to be the right size, but just in case
        // we'll only index into the data that was actually uncompressed.
        zef->uncompressed_size = destlen;
        m_del(uint8_t, compressed, zef->compressed_size);
#endif
    } else {
        m_del(ZipExtFile_t, zef, 1);
        zef = NULL;
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Unsupported compression: %d", compression_method));
    }

    return MP_OBJ_FROM_PTR(zef);
}

// ZipExtFile is used as a file-like object for importing from zip files.

STATIC void ZipExtFile_print(const mp_print_t *print, mp_obj_t self, mp_print_kind_t kind) {
    (void)kind;
    ZipExtFile_t *zef = MP_OBJ_TO_PTR(self);
    mp_printf(print, "<ZipExtFile %s>", mp_obj_str_get_str(zef->filename));
}

STATIC mp_obj_t ZipExtFile_obj_close(mp_obj_t self) {
    ZipExtFile_t *zef = MP_OBJ_TO_PTR(self);
    if (zef->uncompressed_data) {
        zef->filename = mp_const_none;
        zef->seek_offset = 0;
        zef->data_posn = 0;
        zef->compressed_size = 0;
        m_del(uint8_t, zef->uncompressed_data, zef->uncompressed_size);
        zef->uncompressed_size = 0;
        zef->uncompressed_data = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ZipExtFile_obj_close_obj, ZipExtFile_obj_close);

STATIC mp_obj_t ZipExtFile_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return ZipExtFile_obj_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ZipExtFile_obj___exit___obj, 4, 4, ZipExtFile_obj___exit__);

STATIC const mp_rom_map_elem_t ZipExtFile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readall), MP_ROM_PTR(&mp_stream_readall_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ZipExtFile_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ZipExtFile_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&ZipExtFile_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(ZipExtFile_locals_dict, ZipExtFile_locals_dict_table);

STATIC mp_uint_t ZipExtFile_read(mp_obj_t self, void *buf, mp_uint_t size, int *errcode) {
    ZipExtFile_t *zef = MP_OBJ_TO_PTR(self);
    if (zef->seek_offset + size > zef->uncompressed_size) {
        size = zef->uncompressed_size - zef->seek_offset;
    }
    if (size == 0) {
        return 0;
    }
    mp_int_t bytes_read;
    if (zef->uncompressed_data) {
        memcpy(buf, &zef->uncompressed_data[zef->seek_offset], size);
        bytes_read = size;
    } else {
        mp_seek(zef->zf->file, zef->data_posn + zef->seek_offset, MP_SEEK_SET);
        bytes_read = mp_readinto(zef->zf->file, buf, size);
        if (bytes_read < 0) {
            *errcode = EIO;
            return MP_STREAM_ERROR;
        }
    }
    zef->seek_offset += bytes_read;
    return bytes_read;
}

STATIC const mp_stream_p_t ZipExtFile_stream_p = {
    .read = ZipExtFile_read,
};

STATIC const mp_obj_type_t ZipExtFile_type = {
    { &mp_type_type },
    .name = MP_QSTR_ZipExtFile,
    .print = ZipExtFile_print,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &ZipExtFile_stream_p,
    .locals_dict = (mp_obj_dict_t *)&ZipExtFile_locals_dict,
};

#endif // MICROPY_PY_UZIPFILE || MICROPY_PY_ZIPIMPORT

// =============================================================================
//
// Everything from this point onwards in the file is only needed to implement
// the ZipFile classes.
//
// =============================================================================

#if MICROPY_PY_UZIPFILE

STATIC void ZipFile_print(const mp_print_t *print, mp_obj_t self, mp_print_kind_t kind) {
    (void)kind;
    ZipFile_t *zf = MP_OBJ_TO_PTR(self);
    mp_printf(print, "<ZipFile %s>", zf->filename);
}

STATIC mp_obj_t ZipFile_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)}},
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_r)}},
    };
    enum { ARG_file, ARG_mode };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    mp_arg_parse_all(n_args, args, &kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    if (strcmp(mp_obj_str_get_str(vals[ARG_mode].u_obj), "r") != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Only 'r' mode is supported"));
    }

    ZipFile_t *zf = zipfile_open_file_or_filename(NULL, vals[ARG_file].u_obj);
    if (zf->end_directory_posn < 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "%s is not a zip file",
                                                zf->filename));
    }
    return MP_OBJ_FROM_PTR(zf);
}

STATIC mp_obj_t ZipFile_namelist(mp_obj_t self) {
    ZipFile_t *zf = MP_OBJ_TO_PTR(self);

    mp_obj_t namelist = mp_obj_new_list(0, NULL);
    off_t posn = zf_find_central_directory(zf);
    if (posn >= 0) {
        posn = zf_find_first_file(zf);
    }
    while (posn >= 0) {
        ZIP_FILE_HDR file_hdr;
        off_t next_posn = zf_get_file_hdr(zf, posn, &file_hdr);
        if (next_posn < 0) {
            break;
        }

        uint16_t len = ZF_UINT16(file_hdr.filename_len);
        vstr_t filename;
        vstr_init_len(&filename, len);
        mp_readinto(zf->file, vstr_str(&filename), len);
        mp_obj_list_append(namelist, mp_obj_new_str_from_vstr(&mp_type_str, &filename));

        posn = next_posn;
    }
    return namelist;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ZipFile_namelist_obj, ZipFile_namelist);

STATIC mp_obj_t ZipFile_open(mp_obj_t self, mp_obj_t archive_name_in) {
    ZipFile_t *zf = MP_OBJ_TO_PTR(self);
    const char *archive_name = mp_obj_str_get_str(archive_name_in);

    ZIP_FILE_HDR file_hdr;
    off_t posn;
    uint stat = zipfile_find_archive(zf, archive_name, &file_hdr, &posn);
    if (stat != MP_IMPORT_STAT_FILE) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_KeyError, "'%s' not in archive", archive_name));
    }
    return zipfile_open_archive(zf, archive_name, &file_hdr, posn);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ZipFile_open_obj, ZipFile_open);

STATIC mp_obj_t ZipFile_obj_close(mp_obj_t self) {
    ZipFile_t *zf = MP_OBJ_TO_PTR(self);
    zipfile_close(zf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ZipFile_obj_close_obj, ZipFile_obj_close);

STATIC mp_obj_t ZipFile_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return ZipFile_obj_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ZipFile_obj___exit___obj, 4, 4, ZipFile_obj___exit__);

STATIC const mp_rom_map_elem_t ZipFile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_namelist), MP_ROM_PTR(&ZipFile_namelist_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ZipFile_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ZipFile_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ZipFile_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&ZipFile_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(ZipFile_locals_dict, ZipFile_locals_dict_table);

STATIC const mp_obj_type_t ZipFile_type = {
    { &mp_type_type },
    .name = MP_QSTR_ZipFile,
    .print = ZipFile_print,
    .make_new = ZipFile_make_new,
    .locals_dict = (mp_obj_dict_t *)&ZipFile_locals_dict,
};

STATIC mp_obj_t zf_is_zipfile(mp_obj_t file_or_filename) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        ZipFile_t *zf = zipfile_open_file_or_filename(NULL, file_or_filename);
        bool is_zipfile = zf->end_directory_posn >= 0;
        zipfile_close(zf);
        nlr_pop();
        if (is_zipfile) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(zf_is_zipfile_obj, zf_is_zipfile);

STATIC const mp_rom_map_elem_t mp_module_zipfile_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uzipfile) },
    { MP_ROM_QSTR(MP_QSTR_ZIP_STORED), MP_ROM_INT(ZIP_STORED) },
    { MP_ROM_QSTR(MP_QSTR_ZIP_DEFLATED), MP_ROM_INT(ZIP_DEFLATED) },
    { MP_ROM_QSTR(MP_QSTR_is_zipfile), MP_ROM_PTR(&zf_is_zipfile_obj) },
    { MP_ROM_QSTR(MP_QSTR_ZipFile), MP_ROM_PTR(&ZipFile_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_zipfile_globals, mp_module_zipfile_globals_table);

const mp_obj_module_t mp_module_uzipfile = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uzipfile,
    .globals = (mp_obj_dict_t*)&mp_module_zipfile_globals,
};

#endif  // MICROPY_PY_UZIPFILE
