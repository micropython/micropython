/*
 * The little filesystem
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LFS1_H
#define LFS1_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS1_VERSION 0x00010007
#define LFS1_VERSION_MAJOR (0xffff & (LFS1_VERSION >> 16))
#define LFS1_VERSION_MINOR (0xffff & (LFS1_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS1_DISK_VERSION 0x00010001
#define LFS1_DISK_VERSION_MAJOR (0xffff & (LFS1_DISK_VERSION >> 16))
#define LFS1_DISK_VERSION_MINOR (0xffff & (LFS1_DISK_VERSION >>  0))


/// Definitions ///

// Type definitions
typedef uint32_t lfs1_size_t;
typedef uint32_t lfs1_off_t;

typedef int32_t  lfs1_ssize_t;
typedef int32_t  lfs1_soff_t;

typedef uint32_t lfs1_block_t;

// Max name size in bytes
#ifndef LFS1_NAME_MAX
#define LFS1_NAME_MAX 255
#endif

// Max file size in bytes
#ifndef LFS1_FILE_MAX
#define LFS1_FILE_MAX 2147483647
#endif

// Possible error codes, these are negative to allow
// valid positive return values
enum lfs1_error {
    LFS1_ERR_OK       = 0,    // No error
    LFS1_ERR_IO       = -5,   // Error during device operation
    LFS1_ERR_CORRUPT  = -52,  // Corrupted
    LFS1_ERR_NOENT    = -2,   // No directory entry
    LFS1_ERR_EXIST    = -17,  // Entry already exists
    LFS1_ERR_NOTDIR   = -20,  // Entry is not a dir
    LFS1_ERR_ISDIR    = -21,  // Entry is a dir
    LFS1_ERR_NOTEMPTY = -39,  // Dir is not empty
    LFS1_ERR_BADF     = -9,   // Bad file number
    LFS1_ERR_FBIG     = -27,  // File too large
    LFS1_ERR_INVAL    = -22,  // Invalid parameter
    LFS1_ERR_NOSPC    = -28,  // No space left on device
    LFS1_ERR_NOMEM    = -12,  // No more memory available
};

// File types
enum lfs1_type {
    LFS1_TYPE_REG        = 0x11,
    LFS1_TYPE_DIR        = 0x22,
    LFS1_TYPE_SUPERBLOCK = 0x2e,
};

// File open flags
enum lfs1_open_flags {
    // open flags
    LFS1_O_RDONLY = 1,        // Open a file as read only
    LFS1_O_WRONLY = 2,        // Open a file as write only
    LFS1_O_RDWR   = 3,        // Open a file as read and write
    LFS1_O_CREAT  = 0x0100,   // Create a file if it does not exist
    LFS1_O_EXCL   = 0x0200,   // Fail if a file already exists
    LFS1_O_TRUNC  = 0x0400,   // Truncate the existing file to zero size
    LFS1_O_APPEND = 0x0800,   // Move to end of file on every write

    // internally used flags
    LFS1_F_DIRTY   = 0x10000, // File does not match storage
    LFS1_F_WRITING = 0x20000, // File has been written since last flush
    LFS1_F_READING = 0x40000, // File has been read since last flush
    LFS1_F_ERRED   = 0x80000, // An error occured during write
};

// File seek flags
enum lfs1_whence_flags {
    LFS1_SEEK_SET = 0,   // Seek relative to an absolute position
    LFS1_SEEK_CUR = 1,   // Seek relative to the current file position
    LFS1_SEEK_END = 2,   // Seek relative to the end of the file
};


// Configuration provided during initialization of the littlefs
struct lfs1_config {
    // Opaque user provided context that can be used to pass
    // information to the block device operations
    void *context;

    // Read a region in a block. Negative error codes are propogated
    // to the user.
    int (*read)(const struct lfs1_config *c, lfs1_block_t block,
            lfs1_off_t off, void *buffer, lfs1_size_t size);

    // Program a region in a block. The block must have previously
    // been erased. Negative error codes are propogated to the user.
    // May return LFS1_ERR_CORRUPT if the block should be considered bad.
    int (*prog)(const struct lfs1_config *c, lfs1_block_t block,
            lfs1_off_t off, const void *buffer, lfs1_size_t size);

    // Erase a block. A block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propogated to the user.
    // May return LFS1_ERR_CORRUPT if the block should be considered bad.
    int (*erase)(const struct lfs1_config *c, lfs1_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propogated to the user.
    int (*sync)(const struct lfs1_config *c);

    // Minimum size of a block read. This determines the size of read buffers.
    // This may be larger than the physical read size to improve performance
    // by caching more of the block device.
    lfs1_size_t read_size;

    // Minimum size of a block program. This determines the size of program
    // buffers. This may be larger than the physical program size to improve
    // performance by caching more of the block device.
    // Must be a multiple of the read size.
    lfs1_size_t prog_size;

    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, this should be
    // kept small as each file currently takes up an entire block.
    // Must be a multiple of the program size.
    lfs1_size_t block_size;

    // Number of erasable blocks on the device.
    lfs1_size_t block_count;

    // Number of blocks to lookahead during block allocation. A larger
    // lookahead reduces the number of passes required to allocate a block.
    // The lookahead buffer requires only 1 bit per block so it can be quite
    // large with little ram impact. Should be a multiple of 32.
    lfs1_size_t lookahead;

    // Optional, statically allocated read buffer. Must be read sized.
    void *read_buffer;

    // Optional, statically allocated program buffer. Must be program sized.
    void *prog_buffer;

    // Optional, statically allocated lookahead buffer. Must be 1 bit per
    // lookahead block.
    void *lookahead_buffer;

    // Optional, statically allocated buffer for files. Must be program sized.
    // If enabled, only one file may be opened at a time.
    void *file_buffer;
};

// Optional configuration provided during lfs1_file_opencfg
struct lfs1_file_config {
    // Optional, statically allocated buffer for files. Must be program sized.
    // If NULL, malloc will be used by default.
    void *buffer;
};

// File info structure
struct lfs1_info {
    // Type of the file, either LFS1_TYPE_REG or LFS1_TYPE_DIR
    uint8_t type;

    // Size of the file, only valid for REG files
    lfs1_size_t size;

    // Name of the file stored as a null-terminated string
    char name[LFS1_NAME_MAX+1];
};


/// littlefs data structures ///
typedef struct lfs1_entry {
    lfs1_off_t off;

    struct lfs1_disk_entry {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        union {
            struct {
                lfs1_block_t head;
                lfs1_size_t size;
            } file;
            lfs1_block_t dir[2];
        } u;
    } d;
} lfs1_entry_t;

typedef struct lfs1_cache {
    lfs1_block_t block;
    lfs1_off_t off;
    uint8_t *buffer;
} lfs1_cache_t;

typedef struct lfs1_file {
    struct lfs1_file *next;
    lfs1_block_t pair[2];
    lfs1_off_t poff;

    lfs1_block_t head;
    lfs1_size_t size;

    const struct lfs1_file_config *cfg;
    uint32_t flags;
    lfs1_off_t pos;
    lfs1_block_t block;
    lfs1_off_t off;
    lfs1_cache_t cache;
} lfs1_file_t;

typedef struct lfs1_dir {
    struct lfs1_dir *next;
    lfs1_block_t pair[2];
    lfs1_off_t off;

    lfs1_block_t head[2];
    lfs1_off_t pos;

    struct lfs1_disk_dir {
        uint32_t rev;
        lfs1_size_t size;
        lfs1_block_t tail[2];
    } d;
} lfs1_dir_t;

typedef struct lfs1_superblock {
    lfs1_off_t off;

    struct lfs1_disk_superblock {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        lfs1_block_t root[2];
        uint32_t block_size;
        uint32_t block_count;
        uint32_t version;
        char magic[8];
    } d;
} lfs1_superblock_t;

typedef struct lfs1_free {
    lfs1_block_t off;
    lfs1_block_t size;
    lfs1_block_t i;
    lfs1_block_t ack;
    uint32_t *buffer;
} lfs1_free_t;

// The littlefs type
typedef struct lfs1 {
    const struct lfs1_config *cfg;

    lfs1_block_t root[2];
    lfs1_file_t *files;
    lfs1_dir_t *dirs;

    lfs1_cache_t rcache;
    lfs1_cache_t pcache;

    lfs1_free_t free;
    bool deorphaned;
    bool moving;
} lfs1_t;


/// Filesystem functions ///

// Format a block device with the littlefs
//
// Requires a littlefs object and config struct. This clobbers the littlefs
// object, and does not leave the filesystem mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs1_format(lfs1_t *lfs1, const struct lfs1_config *config);

// Mounts a littlefs
//
// Requires a littlefs object and config struct. Multiple filesystems
// may be mounted simultaneously with multiple littlefs objects. Both
// lfs1 and config must be allocated while mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs1_mount(lfs1_t *lfs1, const struct lfs1_config *config);

// Unmounts a littlefs
//
// Does nothing besides releasing any allocated resources.
// Returns a negative error code on failure.
int lfs1_unmount(lfs1_t *lfs1);

/// General operations ///

// Removes a file or directory
//
// If removing a directory, the directory must be empty.
// Returns a negative error code on failure.
int lfs1_remove(lfs1_t *lfs1, const char *path);

// Rename or move a file or directory
//
// If the destination exists, it must match the source in type.
// If the destination is a directory, the directory must be empty.
//
// Returns a negative error code on failure.
int lfs1_rename(lfs1_t *lfs1, const char *oldpath, const char *newpath);

// Find info about a file or directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a negative error code on failure.
int lfs1_stat(lfs1_t *lfs1, const char *path, struct lfs1_info *info);


/// File operations ///

// Open a file
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum lfs1_open_flags that are bitwise-ored together.
//
// Returns a negative error code on failure.
int lfs1_file_open(lfs1_t *lfs1, lfs1_file_t *file,
        const char *path, int flags);

// Open a file with extra configuration
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum lfs1_open_flags that are bitwise-ored together.
//
// The config struct provides additional config options per file as described
// above. The config struct must be allocated while the file is open, and the
// config struct must be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs1_file_opencfg(lfs1_t *lfs1, lfs1_file_t *file,
        const char *path, int flags,
        const struct lfs1_file_config *config);

// Close a file
//
// Any pending writes are written out to storage as though
// sync had been called and releases any allocated resources.
//
// Returns a negative error code on failure.
int lfs1_file_close(lfs1_t *lfs1, lfs1_file_t *file);

// Synchronize a file on storage
//
// Any pending writes are written out to storage.
// Returns a negative error code on failure.
int lfs1_file_sync(lfs1_t *lfs1, lfs1_file_t *file);

// Read data from file
//
// Takes a buffer and size indicating where to store the read data.
// Returns the number of bytes read, or a negative error code on failure.
lfs1_ssize_t lfs1_file_read(lfs1_t *lfs1, lfs1_file_t *file,
        void *buffer, lfs1_size_t size);

// Write data to file
//
// Takes a buffer and size indicating the data to write. The file will not
// actually be updated on the storage until either sync or close is called.
//
// Returns the number of bytes written, or a negative error code on failure.
lfs1_ssize_t lfs1_file_write(lfs1_t *lfs1, lfs1_file_t *file,
        const void *buffer, lfs1_size_t size);

// Change the position of the file
//
// The change in position is determined by the offset and whence flag.
// Returns the old position of the file, or a negative error code on failure.
lfs1_soff_t lfs1_file_seek(lfs1_t *lfs1, lfs1_file_t *file,
        lfs1_soff_t off, int whence);

// Truncates the size of the file to the specified size
//
// Returns a negative error code on failure.
int lfs1_file_truncate(lfs1_t *lfs1, lfs1_file_t *file, lfs1_off_t size);

// Return the position of the file
//
// Equivalent to lfs1_file_seek(lfs1, file, 0, LFS1_SEEK_CUR)
// Returns the position of the file, or a negative error code on failure.
lfs1_soff_t lfs1_file_tell(lfs1_t *lfs1, lfs1_file_t *file);

// Change the position of the file to the beginning of the file
//
// Equivalent to lfs1_file_seek(lfs1, file, 0, LFS1_SEEK_CUR)
// Returns a negative error code on failure.
int lfs1_file_rewind(lfs1_t *lfs1, lfs1_file_t *file);

// Return the size of the file
//
// Similar to lfs1_file_seek(lfs1, file, 0, LFS1_SEEK_END)
// Returns the size of the file, or a negative error code on failure.
lfs1_soff_t lfs1_file_size(lfs1_t *lfs1, lfs1_file_t *file);


/// Directory operations ///

// Create a directory
//
// Returns a negative error code on failure.
int lfs1_mkdir(lfs1_t *lfs1, const char *path);

// Open a directory
//
// Once open a directory can be used with read to iterate over files.
// Returns a negative error code on failure.
int lfs1_dir_open(lfs1_t *lfs1, lfs1_dir_t *dir, const char *path);

// Close a directory
//
// Releases any allocated resources.
// Returns a negative error code on failure.
int lfs1_dir_close(lfs1_t *lfs1, lfs1_dir_t *dir);

// Read an entry in the directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a negative error code on failure.
int lfs1_dir_read(lfs1_t *lfs1, lfs1_dir_t *dir, struct lfs1_info *info);

// Change the position of the directory
//
// The new off must be a value previous returned from tell and specifies
// an absolute offset in the directory seek.
//
// Returns a negative error code on failure.
int lfs1_dir_seek(lfs1_t *lfs1, lfs1_dir_t *dir, lfs1_off_t off);

// Return the position of the directory
//
// The returned offset is only meant to be consumed by seek and may not make
// sense, but does indicate the current position in the directory iteration.
//
// Returns the position of the directory, or a negative error code on failure.
lfs1_soff_t lfs1_dir_tell(lfs1_t *lfs1, lfs1_dir_t *dir);

// Change the position of the directory to the beginning of the directory
//
// Returns a negative error code on failure.
int lfs1_dir_rewind(lfs1_t *lfs1, lfs1_dir_t *dir);


/// Miscellaneous littlefs specific operations ///

// Traverse through all blocks in use by the filesystem
//
// The provided callback will be called with each block address that is
// currently in use by the filesystem. This can be used to determine which
// blocks are in use or how much of the storage is available.
//
// Returns a negative error code on failure.
int lfs1_traverse(lfs1_t *lfs1, int (*cb)(void*, lfs1_block_t), void *data);

// Prunes any recoverable errors that may have occured in the filesystem
//
// Not needed to be called by user unless an operation is interrupted
// but the filesystem is still mounted. This is already called on first
// allocation.
//
// Returns a negative error code on failure.
int lfs1_deorphan(lfs1_t *lfs1);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
