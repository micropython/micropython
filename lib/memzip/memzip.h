#pragma pack(push, 1)

#define MEMZIP_FILE_HEADER_SIGNATURE 0x04034b50
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

} MEMZIP_FILE_HDR;

#define MEMZIP_CENTRAL_DIRECTORY_SIGNATURE 0x02014b50
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
    uint16_t    disk_num;
    uint16_t    internal_file_attributes;
    uint32_t    external_file_attributes;
    uint32_t    file_header_offset;

    /* char     filename[filename_len]  */
    /* uint8_t  extra[extra_len]        */

} MEMZIP_CENTRAL_DIRECTORY_HDR;

#define MEMZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE 0x06054b50
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

} MEMZIP_END_OF_CENTRAL_DIRECTORY;

#pragma pack(pop)

typedef enum {
    MZ_OK = 0,          /* (0) Succeeded */
    MZ_NO_FILE,         /* (1) Could not find the file. */
    MZ_FILE_COMPRESSED, /* (2) File is compressed (expecting uncompressed) */

} MEMZIP_RESULT;

typedef struct {
    uint32_t    file_size;
    uint16_t    last_mod_date;
    uint16_t    last_mod_time;
    uint8_t     is_dir;

} MEMZIP_FILE_INFO;

MEMZIP_RESULT memzip_locate(const char *filename, void **data, size_t *len);

MEMZIP_RESULT memzip_stat(const char *path, MEMZIP_FILE_INFO *info);
