/*
 * The little filesystem
 *
 * Copyright (c) 2022, The littlefs authors.
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LFS2_H
#define LFS2_H

#include "lfs2_util.h"

#ifdef __cplusplus
extern "C"
{
#endif


/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS2_VERSION 0x0002000b
#define LFS2_VERSION_MAJOR (0xffff & (LFS2_VERSION >> 16))
#define LFS2_VERSION_MINOR (0xffff & (LFS2_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS2_DISK_VERSION 0x00020001
#define LFS2_DISK_VERSION_MAJOR (0xffff & (LFS2_DISK_VERSION >> 16))
#define LFS2_DISK_VERSION_MINOR (0xffff & (LFS2_DISK_VERSION >>  0))


/// Definitions ///

// Type definitions
typedef uint32_t lfs2_size_t;
typedef uint32_t lfs2_off_t;

typedef int32_t  lfs2_ssize_t;
typedef int32_t  lfs2_soff_t;

typedef uint32_t lfs2_block_t;

// Maximum name size in bytes, may be redefined to reduce the size of the
// info struct. Limited to <= 1022. Stored in superblock and must be
// respected by other littlefs drivers.
#ifndef LFS2_NAME_MAX
#define LFS2_NAME_MAX 255
#endif

// Maximum size of a file in bytes, may be redefined to limit to support other
// drivers. Limited on disk to <= 2147483647. Stored in superblock and must be
// respected by other littlefs drivers.
#ifndef LFS2_FILE_MAX
#define LFS2_FILE_MAX 2147483647
#endif

// Maximum size of custom attributes in bytes, may be redefined, but there is
// no real benefit to using a smaller LFS2_ATTR_MAX. Limited to <= 1022. Stored
// in superblock and must be respected by other littlefs drivers.
#ifndef LFS2_ATTR_MAX
#define LFS2_ATTR_MAX 1022
#endif

// Possible error codes, these are negative to allow
// valid positive return values
enum lfs2_error {
    LFS2_ERR_OK          = 0,    // No error
    LFS2_ERR_IO          = -5,   // Error during device operation
    LFS2_ERR_CORRUPT     = -84,  // Corrupted
    LFS2_ERR_NOENT       = -2,   // No directory entry
    LFS2_ERR_EXIST       = -17,  // Entry already exists
    LFS2_ERR_NOTDIR      = -20,  // Entry is not a dir
    LFS2_ERR_ISDIR       = -21,  // Entry is a dir
    LFS2_ERR_NOTEMPTY    = -39,  // Dir is not empty
    LFS2_ERR_BADF        = -9,   // Bad file number
    LFS2_ERR_FBIG        = -27,  // File too large
    LFS2_ERR_INVAL       = -22,  // Invalid parameter
    LFS2_ERR_NOSPC       = -28,  // No space left on device
    LFS2_ERR_NOMEM       = -12,  // No more memory available
    LFS2_ERR_NOATTR      = -61,  // No data/attr available
    LFS2_ERR_NAMETOOLONG = -36,  // File name too long
};

// File types
enum lfs2_type {
    // file types
    LFS2_TYPE_REG            = 0x001,
    LFS2_TYPE_DIR            = 0x002,

    // internally used types
    LFS2_TYPE_SPLICE         = 0x400,
    LFS2_TYPE_NAME           = 0x000,
    LFS2_TYPE_STRUCT         = 0x200,
    LFS2_TYPE_USERATTR       = 0x300,
    LFS2_TYPE_FROM           = 0x100,
    LFS2_TYPE_TAIL           = 0x600,
    LFS2_TYPE_GLOBALS        = 0x700,
    LFS2_TYPE_CRC            = 0x500,

    // internally used type specializations
    LFS2_TYPE_CREATE         = 0x401,
    LFS2_TYPE_DELETE         = 0x4ff,
    LFS2_TYPE_SUPERBLOCK     = 0x0ff,
    LFS2_TYPE_DIRSTRUCT      = 0x200,
    LFS2_TYPE_CTZSTRUCT      = 0x202,
    LFS2_TYPE_INLINESTRUCT   = 0x201,
    LFS2_TYPE_SOFTTAIL       = 0x600,
    LFS2_TYPE_HARDTAIL       = 0x601,
    LFS2_TYPE_MOVESTATE      = 0x7ff,
    LFS2_TYPE_CCRC           = 0x500,
    LFS2_TYPE_FCRC           = 0x5ff,

    // internal chip sources
    LFS2_FROM_NOOP           = 0x000,
    LFS2_FROM_MOVE           = 0x101,
    LFS2_FROM_USERATTRS      = 0x102,
};

// File open flags
enum lfs2_open_flags {
    // open flags
    LFS2_O_RDONLY = 1,         // Open a file as read only
#ifndef LFS2_READONLY
    LFS2_O_WRONLY = 2,         // Open a file as write only
    LFS2_O_RDWR   = 3,         // Open a file as read and write
    LFS2_O_CREAT  = 0x0100,    // Create a file if it does not exist
    LFS2_O_EXCL   = 0x0200,    // Fail if a file already exists
    LFS2_O_TRUNC  = 0x0400,    // Truncate the existing file to zero size
    LFS2_O_APPEND = 0x0800,    // Move to end of file on every write
#endif

    // internally used flags
#ifndef LFS2_READONLY
    LFS2_F_DIRTY   = 0x010000, // File does not match storage
    LFS2_F_WRITING = 0x020000, // File has been written since last flush
#endif
    LFS2_F_READING = 0x040000, // File has been read since last flush
#ifndef LFS2_READONLY
    LFS2_F_ERRED   = 0x080000, // An error occurred during write
#endif
    LFS2_F_INLINE  = 0x100000, // Currently inlined in directory entry
};

// File seek flags
enum lfs2_whence_flags {
    LFS2_SEEK_SET = 0,   // Seek relative to an absolute position
    LFS2_SEEK_CUR = 1,   // Seek relative to the current file position
    LFS2_SEEK_END = 2,   // Seek relative to the end of the file
};


// Configuration provided during initialization of the littlefs
struct lfs2_config {
    // Opaque user provided context that can be used to pass
    // information to the block device operations
    void *context;

    // Read a region in a block. Negative error codes are propagated
    // to the user.
    int (*read)(const struct lfs2_config *c, lfs2_block_t block,
            lfs2_off_t off, void *buffer, lfs2_size_t size);

    // Program a region in a block. The block must have previously
    // been erased. Negative error codes are propagated to the user.
    // May return LFS2_ERR_CORRUPT if the block should be considered bad.
    int (*prog)(const struct lfs2_config *c, lfs2_block_t block,
            lfs2_off_t off, const void *buffer, lfs2_size_t size);

    // Erase a block. A block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propagated to the user.
    // May return LFS2_ERR_CORRUPT if the block should be considered bad.
    int (*erase)(const struct lfs2_config *c, lfs2_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propagated to the user.
    int (*sync)(const struct lfs2_config *c);

#ifdef LFS2_THREADSAFE
    // Lock the underlying block device. Negative error codes
    // are propagated to the user.
    int (*lock)(const struct lfs2_config *c);

    // Unlock the underlying block device. Negative error codes
    // are propagated to the user.
    int (*unlock)(const struct lfs2_config *c);
#endif

    // Minimum size of a block read in bytes. All read operations will be a
    // multiple of this value.
    lfs2_size_t read_size;

    // Minimum size of a block program in bytes. All program operations will be
    // a multiple of this value.
    lfs2_size_t prog_size;

    // Size of an erasable block in bytes. This does not impact ram consumption
    // and may be larger than the physical erase size. However, non-inlined
    // files take up at minimum one block. Must be a multiple of the read and
    // program sizes.
    lfs2_size_t block_size;

    // Number of erasable blocks on the device. Defaults to block_count stored
    // on disk when zero.
    lfs2_size_t block_count;

    // Number of erase cycles before littlefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    int32_t block_cycles;

    // Size of block caches in bytes. Each cache buffers a portion of a block in
    // RAM. The littlefs needs a read cache, a program cache, and one additional
    // cache per file. Larger caches can improve performance by storing more
    // data and reducing the number of disk accesses. Must be a multiple of the
    // read and program sizes, and a factor of the block size.
    lfs2_size_t cache_size;

    // Size of the lookahead buffer in bytes. A larger lookahead buffer
    // increases the number of blocks found during an allocation pass. The
    // lookahead buffer is stored as a compact bitmap, so each byte of RAM
    // can track 8 blocks.
    lfs2_size_t lookahead_size;

    // Threshold for metadata compaction during lfs2_fs_gc in bytes. Metadata
    // pairs that exceed this threshold will be compacted during lfs2_fs_gc.
    // Defaults to ~88% block_size when zero, though the default may change
    // in the future.
    //
    // Note this only affects lfs2_fs_gc. Normal compactions still only occur
    // when full.
    //
    // Set to -1 to disable metadata compaction during lfs2_fs_gc.
    lfs2_size_t compact_thresh;

    // Optional statically allocated read buffer. Must be cache_size.
    // By default lfs2_malloc is used to allocate this buffer.
    void *read_buffer;

    // Optional statically allocated program buffer. Must be cache_size.
    // By default lfs2_malloc is used to allocate this buffer.
    void *prog_buffer;

    // Optional statically allocated lookahead buffer. Must be lookahead_size.
    // By default lfs2_malloc is used to allocate this buffer.
    void *lookahead_buffer;

    // Optional upper limit on length of file names in bytes. No downside for
    // larger names except the size of the info struct which is controlled by
    // the LFS2_NAME_MAX define. Defaults to LFS2_NAME_MAX or name_max stored on
    // disk when zero.
    lfs2_size_t name_max;

    // Optional upper limit on files in bytes. No downside for larger files
    // but must be <= LFS2_FILE_MAX. Defaults to LFS2_FILE_MAX or file_max stored
    // on disk when zero.
    lfs2_size_t file_max;

    // Optional upper limit on custom attributes in bytes. No downside for
    // larger attributes size but must be <= LFS2_ATTR_MAX. Defaults to
    // LFS2_ATTR_MAX or attr_max stored on disk when zero.
    lfs2_size_t attr_max;

    // Optional upper limit on total space given to metadata pairs in bytes. On
    // devices with large blocks (e.g. 128kB) setting this to a low size (2-8kB)
    // can help bound the metadata compaction time. Must be <= block_size.
    // Defaults to block_size when zero.
    lfs2_size_t metadata_max;

    // Optional upper limit on inlined files in bytes. Inlined files live in
    // metadata and decrease storage requirements, but may be limited to
    // improve metadata-related performance. Must be <= cache_size, <=
    // attr_max, and <= block_size/8. Defaults to the largest possible
    // inline_max when zero.
    //
    // Set to -1 to disable inlined files.
    lfs2_size_t inline_max;

#ifdef LFS2_MULTIVERSION
    // On-disk version to use when writing in the form of 16-bit major version
    // + 16-bit minor version. This limiting metadata to what is supported by
    // older minor versions. Note that some features will be lost. Defaults to 
    // to the most recent minor version when zero.
    uint32_t disk_version;
#endif
};

// File info structure
struct lfs2_info {
    // Type of the file, either LFS2_TYPE_REG or LFS2_TYPE_DIR
    uint8_t type;

    // Size of the file, only valid for REG files. Limited to 32-bits.
    lfs2_size_t size;

    // Name of the file stored as a null-terminated string. Limited to
    // LFS2_NAME_MAX+1, which can be changed by redefining LFS2_NAME_MAX to
    // reduce RAM. LFS2_NAME_MAX is stored in superblock and must be
    // respected by other littlefs drivers.
    char name[LFS2_NAME_MAX+1];
};

// Filesystem info structure
struct lfs2_fsinfo {
    // On-disk version.
    uint32_t disk_version;

    // Size of a logical block in bytes.
    lfs2_size_t block_size;

    // Number of logical blocks in filesystem.
    lfs2_size_t block_count;

    // Upper limit on the length of file names in bytes.
    lfs2_size_t name_max;

    // Upper limit on the size of files in bytes.
    lfs2_size_t file_max;

    // Upper limit on the size of custom attributes in bytes.
    lfs2_size_t attr_max;
};

// Custom attribute structure, used to describe custom attributes
// committed atomically during file writes.
struct lfs2_attr {
    // 8-bit type of attribute, provided by user and used to
    // identify the attribute
    uint8_t type;

    // Pointer to buffer containing the attribute
    void *buffer;

    // Size of attribute in bytes, limited to LFS2_ATTR_MAX
    lfs2_size_t size;
};

// Optional configuration provided during lfs2_file_opencfg
struct lfs2_file_config {
    // Optional statically allocated file buffer. Must be cache_size.
    // By default lfs2_malloc is used to allocate this buffer.
    void *buffer;

    // Optional list of custom attributes related to the file. If the file
    // is opened with read access, these attributes will be read from disk
    // during the open call. If the file is opened with write access, the
    // attributes will be written to disk every file sync or close. This
    // write occurs atomically with update to the file's contents.
    //
    // Custom attributes are uniquely identified by an 8-bit type and limited
    // to LFS2_ATTR_MAX bytes. When read, if the stored attribute is smaller
    // than the buffer, it will be padded with zeros. If the stored attribute
    // is larger, then it will be silently truncated. If the attribute is not
    // found, it will be created implicitly.
    struct lfs2_attr *attrs;

    // Number of custom attributes in the list
    lfs2_size_t attr_count;
};


/// internal littlefs data structures ///
typedef struct lfs2_cache {
    lfs2_block_t block;
    lfs2_off_t off;
    lfs2_size_t size;
    uint8_t *buffer;
} lfs2_cache_t;

typedef struct lfs2_mdir {
    lfs2_block_t pair[2];
    uint32_t rev;
    lfs2_off_t off;
    uint32_t etag;
    uint16_t count;
    bool erased;
    bool split;
    lfs2_block_t tail[2];
} lfs2_mdir_t;

// littlefs directory type
typedef struct lfs2_dir {
    struct lfs2_dir *next;
    uint16_t id;
    uint8_t type;
    lfs2_mdir_t m;

    lfs2_off_t pos;
    lfs2_block_t head[2];
} lfs2_dir_t;

// littlefs file type
typedef struct lfs2_file {
    struct lfs2_file *next;
    uint16_t id;
    uint8_t type;
    lfs2_mdir_t m;

    struct lfs2_ctz {
        lfs2_block_t head;
        lfs2_size_t size;
    } ctz;

    uint32_t flags;
    lfs2_off_t pos;
    lfs2_block_t block;
    lfs2_off_t off;
    lfs2_cache_t cache;

    const struct lfs2_file_config *cfg;
} lfs2_file_t;

typedef struct lfs2_superblock {
    uint32_t version;
    lfs2_size_t block_size;
    lfs2_size_t block_count;
    lfs2_size_t name_max;
    lfs2_size_t file_max;
    lfs2_size_t attr_max;
} lfs2_superblock_t;

typedef struct lfs2_gstate {
    uint32_t tag;
    lfs2_block_t pair[2];
} lfs2_gstate_t;

// The littlefs filesystem type
typedef struct lfs2 {
    lfs2_cache_t rcache;
    lfs2_cache_t pcache;

    lfs2_block_t root[2];
    struct lfs2_mlist {
        struct lfs2_mlist *next;
        uint16_t id;
        uint8_t type;
        lfs2_mdir_t m;
    } *mlist;
    uint32_t seed;

    lfs2_gstate_t gstate;
    lfs2_gstate_t gdisk;
    lfs2_gstate_t gdelta;

    struct lfs2_lookahead {
        lfs2_block_t start;
        lfs2_block_t size;
        lfs2_block_t next;
        lfs2_block_t ckpoint;
        uint8_t *buffer;
    } lookahead;

    const struct lfs2_config *cfg;
    lfs2_size_t block_count;
    lfs2_size_t name_max;
    lfs2_size_t file_max;
    lfs2_size_t attr_max;
    lfs2_size_t inline_max;

#ifdef LFS2_MIGRATE
    struct lfs21 *lfs21;
#endif
} lfs2_t;


/// Filesystem functions ///

#ifndef LFS2_READONLY
// Format a block device with the littlefs
//
// Requires a littlefs object and config struct. This clobbers the littlefs
// object, and does not leave the filesystem mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs2_format(lfs2_t *lfs2, const struct lfs2_config *config);
#endif

// Mounts a littlefs
//
// Requires a littlefs object and config struct. Multiple filesystems
// may be mounted simultaneously with multiple littlefs objects. Both
// lfs2 and config must be allocated while mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs2_mount(lfs2_t *lfs2, const struct lfs2_config *config);

// Unmounts a littlefs
//
// Does nothing besides releasing any allocated resources.
// Returns a negative error code on failure.
int lfs2_unmount(lfs2_t *lfs2);

/// General operations ///

#ifndef LFS2_READONLY
// Removes a file or directory
//
// If removing a directory, the directory must be empty.
// Returns a negative error code on failure.
int lfs2_remove(lfs2_t *lfs2, const char *path);
#endif

#ifndef LFS2_READONLY
// Rename or move a file or directory
//
// If the destination exists, it must match the source in type.
// If the destination is a directory, the directory must be empty.
//
// Returns a negative error code on failure.
int lfs2_rename(lfs2_t *lfs2, const char *oldpath, const char *newpath);
#endif

// Find info about a file or directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a negative error code on failure.
int lfs2_stat(lfs2_t *lfs2, const char *path, struct lfs2_info *info);

// Get a custom attribute
//
// Custom attributes are uniquely identified by an 8-bit type and limited
// to LFS2_ATTR_MAX bytes. When read, if the stored attribute is smaller than
// the buffer, it will be padded with zeros. If the stored attribute is larger,
// then it will be silently truncated. If no attribute is found, the error
// LFS2_ERR_NOATTR is returned and the buffer is filled with zeros.
//
// Returns the size of the attribute, or a negative error code on failure.
// Note, the returned size is the size of the attribute on disk, irrespective
// of the size of the buffer. This can be used to dynamically allocate a buffer
// or check for existence.
lfs2_ssize_t lfs2_getattr(lfs2_t *lfs2, const char *path,
        uint8_t type, void *buffer, lfs2_size_t size);

#ifndef LFS2_READONLY
// Set custom attributes
//
// Custom attributes are uniquely identified by an 8-bit type and limited
// to LFS2_ATTR_MAX bytes. If an attribute is not found, it will be
// implicitly created.
//
// Returns a negative error code on failure.
int lfs2_setattr(lfs2_t *lfs2, const char *path,
        uint8_t type, const void *buffer, lfs2_size_t size);
#endif

#ifndef LFS2_READONLY
// Removes a custom attribute
//
// If an attribute is not found, nothing happens.
//
// Returns a negative error code on failure.
int lfs2_removeattr(lfs2_t *lfs2, const char *path, uint8_t type);
#endif


/// File operations ///

#ifndef LFS2_NO_MALLOC
// Open a file
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum lfs2_open_flags that are bitwise-ored together.
//
// Returns a negative error code on failure.
int lfs2_file_open(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags);

// if LFS2_NO_MALLOC is defined, lfs2_file_open() will fail with LFS2_ERR_NOMEM
// thus use lfs2_file_opencfg() with config.buffer set.
#endif

// Open a file with extra configuration
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum lfs2_open_flags that are bitwise-ored together.
//
// The config struct provides additional config options per file as described
// above. The config struct must remain allocated while the file is open, and
// the config struct must be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs2_file_opencfg(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags,
        const struct lfs2_file_config *config);

// Close a file
//
// Any pending writes are written out to storage as though
// sync had been called and releases any allocated resources.
//
// Returns a negative error code on failure.
int lfs2_file_close(lfs2_t *lfs2, lfs2_file_t *file);

// Synchronize a file on storage
//
// Any pending writes are written out to storage.
// Returns a negative error code on failure.
int lfs2_file_sync(lfs2_t *lfs2, lfs2_file_t *file);

// Read data from file
//
// Takes a buffer and size indicating where to store the read data.
// Returns the number of bytes read, or a negative error code on failure.
lfs2_ssize_t lfs2_file_read(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size);

#ifndef LFS2_READONLY
// Write data to file
//
// Takes a buffer and size indicating the data to write. The file will not
// actually be updated on the storage until either sync or close is called.
//
// Returns the number of bytes written, or a negative error code on failure.
lfs2_ssize_t lfs2_file_write(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size);
#endif

// Change the position of the file
//
// The change in position is determined by the offset and whence flag.
// Returns the new position of the file, or a negative error code on failure.
lfs2_soff_t lfs2_file_seek(lfs2_t *lfs2, lfs2_file_t *file,
        lfs2_soff_t off, int whence);

#ifndef LFS2_READONLY
// Truncates the size of the file to the specified size
//
// Returns a negative error code on failure.
int lfs2_file_truncate(lfs2_t *lfs2, lfs2_file_t *file, lfs2_off_t size);
#endif

// Return the position of the file
//
// Equivalent to lfs2_file_seek(lfs2, file, 0, LFS2_SEEK_CUR)
// Returns the position of the file, or a negative error code on failure.
lfs2_soff_t lfs2_file_tell(lfs2_t *lfs2, lfs2_file_t *file);

// Change the position of the file to the beginning of the file
//
// Equivalent to lfs2_file_seek(lfs2, file, 0, LFS2_SEEK_SET)
// Returns a negative error code on failure.
int lfs2_file_rewind(lfs2_t *lfs2, lfs2_file_t *file);

// Return the size of the file
//
// Similar to lfs2_file_seek(lfs2, file, 0, LFS2_SEEK_END)
// Returns the size of the file, or a negative error code on failure.
lfs2_soff_t lfs2_file_size(lfs2_t *lfs2, lfs2_file_t *file);


/// Directory operations ///

#ifndef LFS2_READONLY
// Create a directory
//
// Returns a negative error code on failure.
int lfs2_mkdir(lfs2_t *lfs2, const char *path);
#endif

// Open a directory
//
// Once open a directory can be used with read to iterate over files.
// Returns a negative error code on failure.
int lfs2_dir_open(lfs2_t *lfs2, lfs2_dir_t *dir, const char *path);

// Close a directory
//
// Releases any allocated resources.
// Returns a negative error code on failure.
int lfs2_dir_close(lfs2_t *lfs2, lfs2_dir_t *dir);

// Read an entry in the directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a positive value on success, 0 at the end of directory,
// or a negative error code on failure.
int lfs2_dir_read(lfs2_t *lfs2, lfs2_dir_t *dir, struct lfs2_info *info);

// Change the position of the directory
//
// The new off must be a value previous returned from tell and specifies
// an absolute offset in the directory seek.
//
// Returns a negative error code on failure.
int lfs2_dir_seek(lfs2_t *lfs2, lfs2_dir_t *dir, lfs2_off_t off);

// Return the position of the directory
//
// The returned offset is only meant to be consumed by seek and may not make
// sense, but does indicate the current position in the directory iteration.
//
// Returns the position of the directory, or a negative error code on failure.
lfs2_soff_t lfs2_dir_tell(lfs2_t *lfs2, lfs2_dir_t *dir);

// Change the position of the directory to the beginning of the directory
//
// Returns a negative error code on failure.
int lfs2_dir_rewind(lfs2_t *lfs2, lfs2_dir_t *dir);


/// Filesystem-level filesystem operations

// Find on-disk info about the filesystem
//
// Fills out the fsinfo structure based on the filesystem found on-disk.
// Returns a negative error code on failure.
int lfs2_fs_stat(lfs2_t *lfs2, struct lfs2_fsinfo *fsinfo);

// Finds the current size of the filesystem
//
// Note: Result is best effort. If files share COW structures, the returned
// size may be larger than the filesystem actually is.
//
// Returns the number of allocated blocks, or a negative error code on failure.
lfs2_ssize_t lfs2_fs_size(lfs2_t *lfs2);

// Traverse through all blocks in use by the filesystem
//
// The provided callback will be called with each block address that is
// currently in use by the filesystem. This can be used to determine which
// blocks are in use or how much of the storage is available.
//
// Returns a negative error code on failure.
int lfs2_fs_traverse(lfs2_t *lfs2, int (*cb)(void*, lfs2_block_t), void *data);

#ifndef LFS2_READONLY
// Attempt to make the filesystem consistent and ready for writing
//
// Calling this function is not required, consistency will be implicitly
// enforced on the first operation that writes to the filesystem, but this
// function allows the work to be performed earlier and without other
// filesystem changes.
//
// Returns a negative error code on failure.
int lfs2_fs_mkconsistent(lfs2_t *lfs2);
#endif

#ifndef LFS2_READONLY
// Attempt any janitorial work
//
// This currently:
// 1. Calls mkconsistent if not already consistent
// 2. Compacts metadata > compact_thresh
// 3. Populates the block allocator
//
// Though additional janitorial work may be added in the future.
//
// Calling this function is not required, but may allow the offloading of
// expensive janitorial work to a less time-critical code path.
//
// Returns a negative error code on failure. Accomplishing nothing is not
// an error.
int lfs2_fs_gc(lfs2_t *lfs2);
#endif

#ifndef LFS2_READONLY
// Grows the filesystem to a new size, updating the superblock with the new
// block count.
//
// If LFS2_SHRINKNONRELOCATING is defined, this function will also accept
// block_counts smaller than the current configuration, after checking
// that none of the blocks that are being removed are in use.
// Note that littlefs's pseudorandom block allocation means that
// this is very unlikely to work in the general case.
//
// Returns a negative error code on failure.
int lfs2_fs_grow(lfs2_t *lfs2, lfs2_size_t block_count);
#endif

#ifndef LFS2_READONLY
#ifdef LFS2_MIGRATE
// Attempts to migrate a previous version of littlefs
//
// Behaves similarly to the lfs2_format function. Attempts to mount
// the previous version of littlefs and update the filesystem so it can be
// mounted with the current version of littlefs.
//
// Requires a littlefs object and config struct. This clobbers the littlefs
// object, and does not leave the filesystem mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int lfs2_migrate(lfs2_t *lfs2, const struct lfs2_config *cfg);
#endif
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
