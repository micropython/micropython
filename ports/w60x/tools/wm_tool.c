#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#define WM_TOOL_PATH_MAX                      256
#define WM_TOOL_ONCE_READ_LEN	              1024

#define WM_TOOL_RUN_IMG_HEADER_LEN            0x100

#define WM_TOOL_SECBOOT_IMG_ADDR             (0x2100)
#define WM_TOOL_SECBOOT_HEADER_LEN           (0x100)
#define WM_TOOL_SECBOOT_HEADER_POS           (WM_TOOL_SECBOOT_IMG_ADDR - WM_TOOL_SECBOOT_HEADER_LEN)
#define WM_TOOL_SECBOOT_IMG_AREA_TOTAL_LEN   (56 * 1024)

#define WM_TOOL_IMG_HEAD_MAGIC_NO            (0xA0FFFF9F)

#define WM_TOOL_DEFAULT_BAUD_RATE             115200

#define WM_TOOL_DOWNLOAD_TIMEOUT_SEC          (60 * 1)

#define WM_TOOL_USE_1K_XMODEM                  1  /* 1 for use 1k_xmodem 0 for xmodem */


/* Xmodem Frame form: <SOH><blk #><255-blk #><--128 data bytes--><CRC hi><CRC lo> */
#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_CRC_CHR 'C'
#define XMODEM_CRC_SIZE 2      /* Crc_High Byte + Crc_Low Byte */
#define XMODEM_FRAME_ID_SIZE 2 /* Frame_Id + 255-Frame_Id */
#define XMODEM_DATA_SIZE_SOH 128   /* for Xmodem protocol */
#define XMODEM_DATA_SIZE_STX 1024 /* for 1K xmodem protocol */

#if (WM_TOOL_USE_1K_XMODEM)
#define XMODEM_DATA_SIZE  XMODEM_DATA_SIZE_STX
#define XMODEM_HEAD XMODEM_STX
#else
#define XMODEM_DATA_SIZE XMODEM_DATA_SIZE_SOH
#define XMODEM_HEAD XMODEM_SOH
#endif

#ifdef WM_DEBUG
#define WM_TOOL_DBG_PRINT       wm_tool_printf
#else
#define WM_TOOL_DBG_PRINT(...)
#endif

typedef enum {
    WM_TOOL_DL_ACTION_NONE = 0,
    WM_TOOL_DL_ACTION_AT,
    WM_TOOL_DL_ACTION_RTS
} wm_tool_dl_action_e;

typedef enum {
    WM_TOOL_DL_ERASE_NONE = 0,
    WM_TOOL_DL_ERASE_SECBOOT,
    WM_TOOL_DL_ERASE_ALL
} wm_tool_dl_erase_e;

typedef enum {
    WM_TOOL_DL_TYPE_IMAGE = 0,
    WM_TOOL_DL_TYPE_FLS
} wm_tool_dl_type_e;

typedef enum {
    WM_TOOL_LAYOUT_TYPE_1M = 0,
    WM_TOOL_LAYOUT_TYPE_2M = 3
} wm_tool_layout_type_e;

typedef enum {
    WM_TOOL_ZIP_TYPE_UNCOMPRESS = 0,
    WM_TOOL_ZIP_TYPE_COMPRESS
} wm_tool_zip_type_e;

typedef enum {
    WM_TOOL_CRC32_REFLECT_OUTPUT = 1,
    WM_TOOL_CRC32_REFLECT_INPUT  = 2
} wm_tool_crc32_reflect_e;

typedef enum {
    WM_TOOL_SHOW_LOG_NONE = 0,
    WM_TOOL_SHOW_LOG_STR,
    WM_TOOL_SHOW_LOG_HEX
} wm_tool_show_log_e;

typedef struct {
    unsigned int   magic_no;
    unsigned short img_type;
    unsigned short zip_type;
    unsigned int   run_img_addr;
    unsigned int   run_img_len;
    unsigned int   run_org_checksum;
    unsigned int   upd_img_addr;
    unsigned int   upd_img_len;
    unsigned int   upd_checksum;
    unsigned int   upd_no;
    unsigned char  ver[16];
    unsigned int   hd_checksum;
} wm_tool_firmware_booter_t;

const static char *wm_tool_version = "1.0.4";

static int wm_tool_show_usage = 0;
static int wm_tool_list_com   = 0;
static int wm_tool_show_ver   = 0;

static char wm_tool_serial_path[WM_TOOL_PATH_MAX] = "/dev/ttyS0";
static unsigned int wm_tool_download_serial_rate  = WM_TOOL_DEFAULT_BAUD_RATE;
static unsigned int wm_tool_normal_serial_rate    = WM_TOOL_DEFAULT_BAUD_RATE;

static wm_tool_dl_action_e wm_tool_dl_action = WM_TOOL_DL_ACTION_NONE;
static wm_tool_dl_erase_e  wm_tool_dl_erase  = WM_TOOL_DL_ERASE_NONE;
static wm_tool_dl_type_e   wm_tool_dl_type   = WM_TOOL_DL_TYPE_IMAGE;
static char *wm_tool_download_image = NULL;

static char *wm_tool_input_binary = NULL;
static char *wm_tool_output_image = NULL;
static char *wm_tool_secboot_image = NULL;
static unsigned int wm_tool_src_binary_len = 0;
static unsigned int wm_tool_src_binary_crc = 0;
static int wm_tool_is_debug = 0;
static wm_tool_layout_type_e wm_tool_image_type = WM_TOOL_LAYOUT_TYPE_1M;
static wm_tool_zip_type_e wm_tool_zip_type = WM_TOOL_ZIP_TYPE_COMPRESS;
static unsigned int wm_tool_upd_addr = 0x90000;
static unsigned int wm_tool_run_addr = 0x10100;

static unsigned int wm_tool_file_crc = 0xFFFFFFFF;

static wm_tool_show_log_e wm_tool_show_log_type = WM_TOOL_SHOW_LOG_NONE;

#ifdef __MINGW32__

#ifndef CBR_2000000
#define CBR_2000000 2000000
#endif

#ifndef CBR_1000000
#define CBR_1000000 1000000
#endif

#ifndef CBR_921600
#define CBR_921600 921600
#endif

#ifndef CBR_460800
#define CBR_460800 460800
#endif

static DWORD wm_tool_uart_block = 0;

static HANDLE wm_tool_uart_handle = NULL;

const static int wm_tool_uart_speed_array[] = {CBR_2000000, CBR_1000000, CBR_921600, CBR_460800, CBR_115200, CBR_38400,
                                               CBR_19200,   CBR_9600,    CBR_4800,   CBR_2400,   CBR_1200};
#else /* __MINGW32__ */

#ifndef B2000000
#define B2000000 2000000
#endif

#ifndef B1000000
#define B1000000 1000000
#endif

#ifndef B921600
#define B921600 921600
#endif

#ifndef B460800
#define B460800 460800
#endif

static int wm_tool_uart_fd = -1;

static struct termios wm_tool_saved_serial_cfg;


const static int wm_tool_uart_speed_array[] = {B2000000, B1000000, B921600, B460800, B115200, B38400,
                                               B19200,   B9600,    B4800,   B2400,   B1200};

#endif /* __MINGW32__ */

const static int wm_tool_uart_name_array[] = {2000000, 1000000, 921600, 460800, 115200, 38400,
                                              19200,   9600,    4800,   2400,   1200};

const static unsigned char wm_tool_chip_cmd_b115200[]  = {0x21, 0x0a, 0x00, 0x97, 0x4b, 0x31, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00};
const static unsigned char wm_tool_chip_cmd_b460800[]  = {0x21, 0x0a, 0x00, 0x07, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x08, 0x07, 0x00};
const static unsigned char wm_tool_chip_cmd_b921600[]  = {0x21, 0x0a, 0x00, 0x5d, 0x50, 0x31, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00};
const static unsigned char wm_tool_chip_cmd_b1000000[] = {0x21, 0x0a, 0x00, 0x5e, 0x3d, 0x31, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0f, 0x00};
const static unsigned char wm_tool_chip_cmd_b2000000[] = {0x21, 0x0a, 0x00, 0xef, 0x2a, 0x31, 0x00, 0x00, 0x00, 0x80, 0x84, 0x1e, 0x00};

const static unsigned char wm_tool_chip_cmd_get_mac[]  = {0x21, 0x06, 0x00, 0xea, 0x2d, 0x38, 0x00, 0x00, 0x00};

static const unsigned int wm_tool_crc32_tab[] = {0x00000000L, 0x77073096L, 0xee0e612cL,
        0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
        0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL,
        0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L,
        0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L,
        0x83d385c7L, 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
        0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L,
        0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L,
        0xd20d85fdL, 0xa50ab56bL, 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L,
        0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
        0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L,
        0x56b3c423L, 0xcfba9599L, 0xb8bda50fL, 0x2802b89eL, 0x5f058808L,
        0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL,
        0xb6662d3dL, 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
        0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L,
        0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL,
        0x91646c97L, 0xe6635c01L, 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L,
        0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
        0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL,
        0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL,
        0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL,
        0xd3d6f4fbL, 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
        0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL,
        0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L,
        0xb966d409L, 0xce61e49fL, 0x5edef90eL, 0x29d9c998L, 0xb0d09822L,
        0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
        0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L,
        0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L,
        0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L,
        0x7d079eb1L, 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
        0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L, 0xfed41b76L,
        0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L,
        0x17b7be43L, 0x60b08ed5L, 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L,
        0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
        0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L,
        0xa867df55L, 0x316e8eefL, 0x4669be79L, 0xcb61b38cL, 0xbc66831aL,
        0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L,
        0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
        0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L,
        0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL,
        0x72076785L, 0x05005713L, 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL,
        0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
        0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL,
        0xf6b9265bL, 0x6fb077e1L, 0x18b74777L, 0x88085ae6L, 0xff0f6a70L,
        0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L,
        0x166ccf45L, 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
        0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL,
        0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L,
        0x47b2cf7fL, 0x30b5ffe9L, 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L,
        0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
        0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L,
        0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL};

static void wm_tool_stdin_to_uart(void);

/* ============================ zlib gzip ================================== */

#define ALLOC(size) zcalloc((voidp)0, 1, size)
#define TRYFREE(p) {if (p) zcfree((voidp)0, p);}

#define BINARY  0
#define ASCII   1
#define UNKNOWN 2

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2

#define REP_3_6      16
#define REPZ_3_10    17
#define REPZ_11_138  18

#define MAX_BL_BITS 7

#define BASE 65521 /* largest prime smaller than 65536 */
#define NMAX 5552

#define DO1(buf)  {s1 += *buf++; s2 += s1;}
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
#define DO16(buf) DO8(buf); DO8(buf);

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

#define DEFLATED   8

#ifndef WBITS
# define WBITS   15 /* 32K window */
#endif

#ifndef MEM_LEVEL
# define MEM_LEVEL  8
#endif

#define Z_BUFSIZE 4096

#define GZ_MAGIC_1 0x1f
#define GZ_MAGIC_2 0x8b

#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif

#ifndef TOO_FAR
#  define TOO_FAR 4096
#endif

#define Z_NO_FLUSH      0
#define Z_FINISH        4

#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

#define LENGTH_CODES 29/* number of length codes, not counting the special END_BLOCK code */
#define LITERALS  256/* number of literal bytes 0..255 */
#define L_CODES (LITERALS+1+LENGTH_CODES)/* number of Literal or Length codes, including the END_BLOCK code */
#define D_CODES   30/* number of distance codes */
#define BL_CODES  19/* number of codes used to transfer the bit lengths */
#define HEAP_SIZE (2*L_CODES+1)/* maximum heap size */

#define END_BLOCK 256

#define MAX_BITS 15

#define MAX_MEM_LEVEL 9

#define MIN_MATCH  3
#define MAX_MATCH  258

#define Z_UNKNOWN  2

#define INIT_STATE    42
#define BUSY_STATE   113
#define FINISH_STATE 666

#define NIL 0

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0

#define SMALLEST 1

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)

#define UPDATE_HASH(s,h,c) (h = (((h)<<s->hash_shift) ^ (c)) & s->hash_mask)

#define ERR_RETURN(strm,err) return (strm->msg=z_errmsg[1-err], err)

#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}

#define put_short(s, w) { \
    put_byte(s, (uch)((w) & 0xff)); \
    put_byte(s, (uch)((ush)(w) >> 8)); \
}

#define INSERT_STRING(s, str, match_head) \
   (UPDATE_HASH(s, s->ins_h, s->window[(str) + MIN_MATCH-1]), \
    s->prev[(str) & s->w_mask] = match_head = s->head[s->ins_h], \
    s->head[s->ins_h] = (str))

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)

#define check_match(s, start, match, length)

#define d_code(dist) \
   ((dist) < 256 ? dist_code[dist] : dist_code[256+((dist)>>7)])

#define FLUSH_BLOCK_ONLY(s, eof) { \
   ct_flush_block(s, (s->block_start >= 0L ? \
	       (char*)&s->window[(unsigned)s->block_start] : \
               (char*)Z_NULL), (long)s->strstart - s->block_start, (eof)); \
   s->block_start = s->strstart; \
   flush_pending(s->strm); \
}

#define FLUSH_BLOCK(s, eof) { \
   FLUSH_BLOCK_ONLY(s, eof); \
   if (s->strm->avail_out == 0) return 1; \
}

#define send_code(s, c, tree) send_bits(s, tree[c].Code, tree[c].Len)

#define MAX(a,b) (a >= b ? a : b)

#define Buf_size (8 * 2*sizeof(char))

#define smaller(tree, n, m, depth) \
   (tree[n].Freq < tree[m].Freq || \
   (tree[n].Freq == tree[m].Freq && depth[n] <= depth[m]))

#define pqremove(s, tree, top) \
{\
    top = s->heap[SMALLEST]; \
    s->heap[SMALLEST] = s->heap[s->heap_len--]; \
    pqdownheap(s, tree, SMALLEST); \
}

#ifndef local
#  define local static
#endif

#ifndef __P
#define __P(args)  args
#endif

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidp)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#define Assert(cond,msg)

#ifndef Byte
  typedef unsigned char  Byte;  /* 8 bits */
#endif
#ifndef uInt
  typedef unsigned int   uInt;  /* may be 16 or 32 bits */
#endif
#ifndef uLong
  typedef unsigned long  uLong; /* 32 bits or more */
#endif
typedef Byte *voidp;
typedef voidp gzFile;
typedef unsigned char  uch;
typedef unsigned long  ulg;
typedef unsigned short ush;
typedef ush Pos;
typedef unsigned IPos;

typedef voidp (*alloc_func) __P((voidp opaque, uInt items, uInt size));
typedef void  (*free_func)  __P((voidp opaque, voidp address));

typedef uLong (*check_func) __P((uLong check, Byte *buf, uInt len));

typedef struct z_stream_s {
    Byte     *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Byte     *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidp      opaque;  /* private data object passed to zalloc and zfree */

    Byte     data_type; /* best guess about the data type: ascii or binary */

} z_stream;

typedef struct gz_stream {
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
    FILE     *file;   /* .gz file */
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
    char     *path;   /* path name for debugging only */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
} gz_stream;

/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
} ct_data;

struct static_tree_desc_s {
    ct_data *static_tree;        /* static tree or NULL */
    int     *extra_bits;         /* extra bits for each code or NULL */
    int     extra_base;          /* base index for extra_bits */
    int     elems;               /* max number of elements in the tree */
    int     max_length;          /* max bit length for the codes */
};

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
} tree_desc;

typedef struct inflate_huft_s inflate_huft;
struct inflate_huft_s {
  union {
    struct {
      char Exop;        /* number of extra bits or operation */
      char Bits;        /* number of bits in this code or subcode */
    } what;
    Byte *pad;          /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit machines) */
  union {
    uInt Base;          /* literal, length base, or distance base */
    inflate_huft *Next; /* pointer to next level of table */
  } more;
};

/* inflate codes private state */
struct inflate_codes_state {

  /* mode */
  enum {	/* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,	/* x: set up for LEN */
      LEN,	/* i: get length/literal/eob next */
      LENEXT,	/* i: getting length extra (have base) */
      DIST,	/* i: get distance next */
      DISTEXT,	/* i: getting distance extra */
      COPY,	/* o: copying bytes in window, waiting for space */
      LIT,	/* o: got literal, waiting for output space */
      WASH,	/* o: got eob, possibly still output waiting */
      END,	/* x: got eob and all data flushed */
      BAD}	/* x: got error */
    mode;		/* current inflate_codes mode */

  /* mode dependent information */
  uInt len;
  union {
    struct {
      inflate_huft *tree;	/* pointer into tree */
      uInt need;		/* bits needed */
    } code;		/* if LEN or DIST, where in tree */
    uInt lit;		/* if LIT, literal */
    struct {
      uInt get;			/* bits to get for extra */
      uInt dist;		/* distance back to copy from */
    } copy;		/* if EXT or COPY, where and how much */
  } sub;		/* submode */

  /* mode independent information */
  Byte lbits;		/* ltree bits decoded per branch */
  Byte dbits;		/* dtree bits decoder per branch */
  inflate_huft *ltree;		/* literal/length/eob tree */
  inflate_huft *dtree;		/* distance tree */

};


/* inflate blocks semi-private state */
struct inflate_blocks_state {

  /* mode */
  enum {
      TYPE,	/* get type bits (3, including end bit) */
      LENS,	/* get lengths for stored */
      STORED,	/* processing stored block */
      TABLE,	/* get table lengths */
      BTREE,	/* get bit lengths tree for a dynamic block */
      DTREE,	/* get length, distance trees for a dynamic block */
      CODES,	/* processing fixed or dynamic block */
      DRY,	/* output remaining window bytes */
      DONE,	/* finished last block, done */
      INF_ERROR}/* got a data error--stuck here */
    mode;		/* current inflate_block mode */

  /* mode dependent information */
  union {
    uInt left;		/* if STORED, bytes left to copy */
    struct {
      uInt table;		/* table lengths (14 bits) */
      uInt index;		/* index into blens (or border) */
      uInt *blens;		/* bit lengths of codes */
      uInt bb;			/* bit length tree depth */
      inflate_huft *tb;		/* bit length decoding tree */
    } trees;		/* if DTREE, decoding info for trees */
    struct inflate_codes_state
      *codes;		/* if CODES, current state */
  } sub;		/* submode */
  uInt last;		/* true if this block is the last block */

  /* mode independent information */
  uInt bitk;		/* bits in bit buffer */
  uLong bitb;		/* bit buffer */
  Byte *window;		/* sliding window */
  Byte *end;		/* one byte after sliding window */
  Byte *read;		/* window read pointer */
  Byte *write;		/* window write pointer */
  check_func checkfn;   /* check function */
  uLong check;		/* check on output */

};

typedef struct internal_state {
    z_stream *strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    Byte *pending_buf;   /* output still pending */
    Byte *pending_out;   /* next pending byte to output to the stream */
    int   pending;       /* nb of bytes in the pending buffer */
    uLong adler;         /* adler32 of uncompressed data */
    int   noheader;      /* suppress zlib header and adler32 */
    Byte  data_type;     /* UNKNOWN, BINARY or ASCII */
    Byte  method;        /* STORED (for zip only) or DEFLATED */

    		/* used by deflate.c: */

    uInt  w_size;        /* LZ77 window size (32K by default) */
    uInt  w_bits;        /* log2(w_size)  (8..16) */
    uInt  w_mask;        /* w_size - 1 */

    Byte *window;
    /* Sliding window. Input bytes are read into the second half of the window,
     * and move to the first half later to keep a dictionary of at least wSize
     * bytes. With this organization, matches are limited to a distance of
     * wSize-MAX_MATCH bytes, but this ensures that IO is always
     * performed with a length multiple of the block size. Also, it limits
     * the window size to 64K, which is quite useful on MSDOS.
     * To do: use the user input buffer as sliding window.
     */

    ulg window_size;
    /* Actual size of window: 2*wSize, except when the user input buffer
     * is directly used as sliding window.
     */

    Pos *prev;
    /* Link to older string with same hash index. To limit the size of this
     * array to 64K, this link is maintained only for the last 32K strings.
     * An index in this array is thus a window index modulo 32K.
     */

    Pos *head; /* Heads of the hash chains or NIL. */

    uInt  ins_h;          /* hash index of string to be inserted */
    uInt  hash_size;      /* number of elements in hash table */
    uInt  hash_bits;      /* log2(hash_size) */
    uInt  hash_mask;      /* hash_size-1 */

    uInt  hash_shift;
    /* Number of bits by which ins_h must be shifted at each input
     * step. It must be such that after MIN_MATCH steps, the oldest
     * byte no longer takes part in the hash key, that is:
     *   hash_shift * MIN_MATCH >= hash_bits
     */

    long block_start;
    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */

    uInt match_length;           /* length of best match */
    IPos prev_match;             /* previous match */
    int match_available;         /* set if previous match exists */
    uInt strstart;               /* start of string to insert */
    uInt match_start;            /* start of matching string */
    uInt lookahead;              /* number of valid bytes ahead in window */

    uInt prev_length;
    /* Length of the best match at previous step. Matches not greater than this
     * are discarded. This is used in the lazy match evaluation.
     */

    uInt max_chain_length;
    /* To speed up deflation, hash chains are never searched beyond this
     * length.  A higher limit improves compression ratio but degrades the
     * speed.
     */

    uInt max_lazy_match;
    /* Attempt to find a better match only when the current match is strictly
     * smaller than this value. This mechanism is used only for compression
     * levels >= 4.
     */
#   define max_insert_length  max_lazy_match
    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * max_insert_length is used only for compression levels <= 3.
     */

    int level;    /* compression level (1..9) */
    int strategy; /* favor or force Huffman coding*/

    uInt good_match;
    /* Use a faster search when the previous match is longer than this */

     int nice_match; /* Stop searching when current match exceeds this */

    		/* used by trees.c: */

    ct_data dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    ct_data dyn_dtree[2*D_CODES+1]; /* distance tree */
    ct_data bl_tree[2*BL_CODES+1];  /* Huffman tree for the bit lengths */

    tree_desc l_desc;               /* descriptor for literal tree */
    tree_desc d_desc;               /* descriptor for distance tree */
    tree_desc bl_desc;              /* descriptor for bit length tree */

    ush bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    int heap_len;               /* number of elements in the heap */
    int heap_max;               /* element of largest frequency */
    /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
     * The same heap array is used to build all trees.
     */

    uch depth[2*L_CODES+1];
    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */

    uch *l_buf;           /* buffer for literals or lengths */

    uInt  lit_bufsize;
    /* Size of match buffer for literals/lengths.  There are 4 reasons for
     * limiting lit_bufsize to 64K:
     *   - frequencies can be kept in 16 bit counters
     *   - if compression is not successful for the first block, all input
     *     data is still in the window so we can still emit a stored block even
     *     when input comes from standard input.  (This can also be done for
     *     all blocks if lit_bufsize is not greater than 32K.)
     *   - if compression is not successful for a file smaller than 64K, we can
     *     even emit a stored file instead of a stored block (saving 5 bytes).
     *     This is applicable only for zip (not gzip or zlib).
     *   - creating new Huffman trees less frequently may not provide fast
     *     adaptation to changes in the input data statistics. (Take for
     *     example a binary file with poorly compressible code followed by
     *     a highly compressible string table.) Smaller buffer sizes give
     *     fast adaptation but have of course the overhead of transmitting
     *     trees more frequently.
     *   - I can't count above 4
     */

    uInt last_lit;      /* running index in l_buf */

    ush *d_buf;
    /* Buffer for distances. To simplify the code, d_buf and l_buf have
     * the same number of elements. To use different lengths, an extra flag
     * array would be necessary.
     */

    ulg opt_len;        /* bit length of current block with optimal trees */
    ulg static_len;     /* bit length of current block with static trees */
    ulg compressed_len; /* total bit length of compressed file */
    uInt matches;       /* number of string matches in current block */

#ifdef DEBUG
    ulg bits_sent;      /* bit length of the compressed data */
#endif

    ush bi_buf;
    /* Output buffer. bits are inserted starting at the bottom (least
     * significant bits).
     */
    int bi_valid;
    /* Number of valid bits in bi_buf.  All bits above the last valid bit
     * are always zero.
     */

    /* mode */
  enum {
      METHOD,	/* waiting for method byte */
      FLAG,	/* waiting for flag byte */
      //START,	/* make new blocks state */
      BLOCKS,	/* decompressing blocks */
      CHECK4,	/* four check bytes to go */
      CHECK3,	/* three check bytes to go */
      CHECK2,	/* two check bytes to go */
      CHECK1,	/* one check byte to go */
      //DONE,	/* finished check, done */
      //INF_ERROR/* got an error--stay here */
      }
    mode;		/* current inflate mode */

  /* mode dependent information */
  union {
    uInt method;	/* if FLAGS, method byte */
    struct inflate_blocks_state
      *blocks;		/* if BLOCKS, current state */
    struct {
      uLong was;		/* computed check value */
      uLong need;		/* stream check value */
    } check;		/* if CHECK, check values to compare */
  } sub;	/* submode */

  /* mode independent information */
  int  nowrap;		/* flag for no wrapper */
  uInt wbits;  		/* log2(window size)  (8..15, defaults to 15) */
} deflate_state;

typedef struct config_s {
   ush good_length; /* reduce lazy search above this match length */
   ush max_lazy;    /* do not perform lazy search above this match length */
   ush nice_length; /* quit search above this match length */
   ush max_chain;
} config;

char *z_errmsg[] = {
"stream end",          /* Z_STREAM_END    1 */
"",                    /* Z_OK            0 */
"file error",          /* Z_ERRNO        (-1) */
"stream error",        /* Z_STREAM_ERROR (-2) */
"data error",          /* Z_DATA_ERROR   (-3) */
"insufficient memory", /* Z_MEM_ERROR    (-4) */
"buffer error",        /* Z_BUF_ERROR    (-5) */
""};

local int base_length[LENGTH_CODES];
local int extra_lbits[LENGTH_CODES] /* extra bits for each length code */
   = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
local uch length_code[MAX_MATCH-MIN_MATCH+1];
local int base_dist[D_CODES];
local int extra_dbits[D_CODES] /* extra bits for each distance code */
   = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
local ct_data static_ltree[L_CODES+2];
local uch dist_code[512];
local int base_dist[D_CODES];
local ct_data static_dtree[D_CODES];

local static_tree_desc  static_l_desc =
{static_ltree, extra_lbits, LITERALS+1, L_CODES, MAX_BITS};

local static_tree_desc  static_d_desc =
{static_dtree, extra_dbits, 0,          D_CODES, MAX_BITS};

local int extra_blbits[BL_CODES]/* extra bits for each bit length code */
   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

local static_tree_desc  static_bl_desc =
{(ct_data *)0, extra_blbits, 0,      BL_CODES, MAX_BL_BITS};

local uch bl_order[BL_CODES]
   = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};

local config configuration_table[10] = {
/*      good lazy nice chain */
/* 0 */ {0,    0,  0,    0},  /* store only */
/* 1 */ {4,    4,  8,    4},  /* maximum speed, no lazy matches */
/* 2 */ {4,    5, 16,    8},
/* 3 */ {4,    6, 32,   32},

/* 4 */ {4,    4, 16,   16},  /* lazy matches */
/* 5 */ {8,   16, 32,   32},
/* 6 */ {8,   16, 128, 128},
/* 7 */ {8,   32, 128, 256},
/* 8 */ {32, 128, 258, 1024},
/* 9 */ {32, 258, 258, 4096}}; /* maximum compression */

#define FIXEDH 530	/* number of hufts used by fixed tables */
local inflate_huft fixed_mem[FIXEDH];

#define next more.Next

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

local voidp zcalloc (opaque, items, size)
    voidp opaque;
    unsigned items;
    unsigned size;
{
    return calloc(items, size);
}

local void  zcfree (opaque, ptr)
    voidp opaque;
    voidp ptr;
{
    free(ptr);
}

local uLong crc32(crc, buf, len)
    uLong crc;
    Byte *buf;
    uInt len;
{
    if (buf == Z_NULL) return 0L;
    crc = crc ^ 0xffffffffL;
    if (len) do {
	crc = wm_tool_crc32_tab[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
    } while (--len);
    return crc ^ 0xffffffffL;
}

local void zmemcpy(dest, source, len)
    Byte* dest;
    Byte* source;
    uInt  len;
{
    if (len == 0) return;
    do {
	*dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

local void zmemzero(dest, len)
    Byte* dest;
    uInt  len;
{
    if (len == 0) return;
    do {
	*dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}

local uLong adler32(adler, buf, len)
    uLong adler;
    Byte *buf;
    uInt len;
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
	k = len < NMAX ? len : NMAX;
	len -= k;
	while (k >= 16) {
	    DO16(buf);
	    k -= 16;
	}
	if (k != 0) do {
	    DO1(buf);
	} while (--k);
	s1 %= BASE;
	s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

local int read_buf(strm, buf, size)
    z_stream *strm;
    char *buf;
    unsigned size;
{
    unsigned len = strm->avail_in;

    if (len > size) len = size;
    if (len == 0) return 0;

    strm->avail_in  -= len;

    if (!strm->state->noheader) {
	strm->state->adler = adler32(strm->state->adler, strm->next_in, len);
    }
    zmemcpy((Byte *)buf, strm->next_in, len);
    strm->next_in  += len;
    strm->total_in += len;

    return (int)len;
}

local int inflate_trees_free(t, z)
inflate_huft *t;	/* table to free */
z_stream *z;		/* for zfree function */
/* Free the malloc'ed tables built by huft_build(), which makes a linked
   list of the tables it made, with the links in a dummy first entry of
   each table. */
{
  register inflate_huft *p, *q;

  /* Don't free fixed trees */
  if (t >= fixed_mem && t <= fixed_mem + FIXEDH)
    return Z_OK;

  /* Go through linked list, freeing from the malloced (t[-1]) address. */
  p = t;
  while (p != Z_NULL)
  {
    q = (--p)->next;
    ZFREE(z,p);
    p = q;
  }
  return Z_OK;
}

local void inflate_codes_free(c, z)
struct inflate_codes_state *c;
z_stream *z;
{
  inflate_trees_free(c->dtree, z);
  inflate_trees_free(c->ltree, z);
  ZFREE(z, c);
}

local int inflate_blocks_free(s, z, c)
struct inflate_blocks_state *s;
z_stream *z;
uLong *c;
{
  if (s->checkfn != Z_NULL)
    *c = s->check;
  if (s->mode == BTREE || s->mode == DTREE)
    ZFREE(z, s->sub.trees.blens);
  if (s->mode == CODES)
    inflate_codes_free(s->sub.codes, z);
  ZFREE(z, s->window);
  ZFREE(z, s);
  return Z_OK;
}

local int deflateEnd (strm)
    z_stream *strm;
{
    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;

    TRY_FREE(strm, strm->state->window);
    TRY_FREE(strm, strm->state->prev);
    TRY_FREE(strm, strm->state->head);
    TRY_FREE(strm, strm->state->pending_buf);

    ZFREE(strm, strm->state);
    strm->state = Z_NULL;

    return Z_OK;
}

local int inflateEnd(z)
z_stream *z;
{
  uLong c;

  if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->mode == BLOCKS)
    inflate_blocks_free(z->state->sub.blocks, z, &c);
  ZFREE(z, z->state);
  z->state = Z_NULL;
  return Z_OK;
}

local int destroy (s)
    gz_stream *s;
{
    int err = Z_OK;

    if (!s) return Z_STREAM_ERROR;

    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
    TRYFREE((voidp)s->path);
    TRYFREE((voidp)s->msg);

    if (s->stream.state != NULL) {
       if (s->mode == 'w') {
	   err = deflateEnd(&(s->stream));
       } else if (s->mode == 'r') {
	   err = inflateEnd(&(s->stream));
       }
    }
    if (s->file != NULL && fclose(s->file)) {
	err = Z_ERRNO;
    }
    if (s->z_err < 0) err = s->z_err;
    zcfree((voidp)0, (voidp)s);
    return err;
}

local void putLong (file, x)
    FILE *file;
    uLong x;
{
    int n;
    for (n = 0; n < 4; n++) {
	fputc((int)(x & 0xff), file);
	x >>= 8;
    }
}

local uLong getLong (buf)
    Byte *buf;
{
    uLong x = 0;
    Byte *p = buf+4;

    do {
	x <<= 8;
	x |= *--p;
    } while (p != buf);
    return x;
}

local unsigned bi_reverse(code, len)
    unsigned code; /* the value to invert */
    int len;       /* its bit length */
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}

local void gen_codes (tree, max_code, bl_count)
    ct_data *tree;             /* the tree to decorate */
    int max_code;              /* largest code with non zero frequency */
    ush bl_count[];            /* number of codes at each bit length */
{
    ush next_code[MAX_BITS+1]; /* next code value for each bit length */
    ush code = 0;              /* running code value */
    int bits;                  /* bit index */
    int n;                     /* code index */

    /* The distribution counts are first used to generate the code values
     * without bit reversal.
     */
    for (bits = 1; bits <= MAX_BITS; bits++) {
        next_code[bits] = code = (code + bl_count[bits-1]) << 1;
    }
    /* Check that the bit counts in bl_count are consistent. The last code
     * must be all ones.
     */
    Assert (code + bl_count[MAX_BITS]-1 == (1<<MAX_BITS)-1,
            "inconsistent bit counts");

    for (n = 0;  n <= max_code; n++) {
        int len = tree[n].Len;
        if (len == 0) continue;
        /* Now reverse the bits */
        tree[n].Code = bi_reverse(next_code[len]++, len);
    }
}

local void ct_static_init()
{
    int n;        /* iterates over tree elements */
    int bits;     /* bit counter */
    int length;   /* length value */
    int code;     /* code value */
    int dist;     /* distance index */
    ush bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    /* Initialize the mapping length (0..255) -> length code (0..28) */
    length = 0;
    for (code = 0; code < LENGTH_CODES-1; code++) {
        base_length[code] = length;
        for (n = 0; n < (1<<extra_lbits[code]); n++) {
            length_code[length++] = (uch)code;
        }
    }
    Assert (length == 256, "ct_static_init: length != 256");
    /* Note that the length 255 (match length 258) can be represented
     * in two different ways: code 284 + 5 bits or code 285, so we
     * overwrite length_code[255] to use the best encoding:
     */
    length_code[length-1] = (uch)code;

    /* Initialize the mapping dist (0..32K) -> dist code (0..29) */
    dist = 0;
    for (code = 0 ; code < 16; code++) {
        base_dist[code] = dist;
        for (n = 0; n < (1<<extra_dbits[code]); n++) {
            dist_code[dist++] = (uch)code;
        }
    }
    Assert (dist == 256, "ct_static_init: dist != 256");
    dist >>= 7; /* from now on, all distances are divided by 128 */
    for ( ; code < D_CODES; code++) {
        base_dist[code] = dist << 7;
        for (n = 0; n < (1<<(extra_dbits[code]-7)); n++) {
            dist_code[256 + dist++] = (uch)code;
        }
    }
    Assert (dist == 256, "ct_static_init: 256+dist != 512");

    /* Construct the codes of the static literal tree */
    for (bits = 0; bits <= MAX_BITS; bits++) bl_count[bits] = 0;
    n = 0;
    while (n <= 143) static_ltree[n++].Len = 8, bl_count[8]++;
    while (n <= 255) static_ltree[n++].Len = 9, bl_count[9]++;
    while (n <= 279) static_ltree[n++].Len = 7, bl_count[7]++;
    while (n <= 287) static_ltree[n++].Len = 8, bl_count[8]++;
    /* Codes 286 and 287 do not exist, but we must include them in the
     * tree construction to get a canonical Huffman tree (longest code
     * all ones)
     */
    gen_codes((ct_data *)static_ltree, L_CODES+1, bl_count);

    /* The static distance tree is trivial: */
    for (n = 0; n < D_CODES; n++) {
        static_dtree[n].Len = 5;
        static_dtree[n].Code = bi_reverse(n, 5);
    }
}

local void init_block(s)
    deflate_state *s;
{
    int n; /* iterates over tree elements */

    /* Initialize the trees. */
    for (n = 0; n < L_CODES;  n++) s->dyn_ltree[n].Freq = 0;
    for (n = 0; n < D_CODES;  n++) s->dyn_dtree[n].Freq = 0;
    for (n = 0; n < BL_CODES; n++) s->bl_tree[n].Freq = 0;

    s->dyn_ltree[END_BLOCK].Freq = 1;
    s->opt_len = s->static_len = 0L;
    s->last_lit = s->matches = 0;
}

local void ct_init(s)
    deflate_state *s;
{
    if (static_dtree[0].Len == 0) {
	ct_static_init();              /* To do: at compile time */
    }

    s->compressed_len = 0L;

    s->l_desc.dyn_tree = s->dyn_ltree;
    s->l_desc.stat_desc = &static_l_desc;

    s->d_desc.dyn_tree = s->dyn_dtree;
    s->d_desc.stat_desc = &static_d_desc;

    s->bl_desc.dyn_tree = s->bl_tree;
    s->bl_desc.stat_desc = &static_bl_desc;

    s->bi_buf = 0;
    s->bi_valid = 0;
#ifdef DEBUG
    s->bits_sent = 0L;
#endif

    /* Initialize the first block of the first file: */
    init_block(s);
}

local void lm_init (s)
    deflate_state *s;
{
    register unsigned j;

    s->window_size = (ulg)2L*s->w_size;


    /* Initialize the hash table (avoiding 64K overflow for 16 bit systems).
     * prev[] will be initialized on the fly.
     */
    s->head[s->hash_size-1] = NIL;
    zmemzero((unsigned char*)s->head, (unsigned int)(s->hash_size-1)*sizeof(*s->head));

    /* Set the default configuration parameters:
     */
    s->max_lazy_match   = configuration_table[s->level].max_lazy;
    s->good_match       = configuration_table[s->level].good_length;
    s->nice_match       = configuration_table[s->level].nice_length;
    s->max_chain_length = configuration_table[s->level].max_chain;

    s->strstart = 0;
    s->block_start = 0L;
    s->lookahead = 0;
    s->match_length = MIN_MATCH-1;
    s->match_available = 0;
#ifdef ASMV
    match_init(); /* initialize the asm code */
#endif

    s->ins_h = 0;
    for (j=0; j<MIN_MATCH-1; j++) UPDATE_HASH(s, s->ins_h, s->window[j]);
    /* If lookahead < MIN_MATCH, ins_h is garbage, but this is
     * not important since only literal bytes will be emitted.
     */
}

local int deflateReset (strm)
    z_stream *strm;
{
    deflate_state *s;

    if (strm == Z_NULL || strm->state == Z_NULL ||
	strm->zalloc == Z_NULL || strm->zfree == Z_NULL) return Z_STREAM_ERROR;

    strm->total_in = strm->total_out = 0;
    strm->msg = Z_NULL; /* use zfree if we ever allocate msg dynamically */
    strm->data_type = Z_UNKNOWN;

    s = (deflate_state *)strm->state;
    s->pending = 0;
    s->pending_out = s->pending_buf;

    s->status = s->noheader ? BUSY_STATE : INIT_STATE;
    s->adler = 1;

    ct_init(s);
    lm_init(s);

    return Z_OK;
}

local int deflateInit2 (strm, level, method, windowBits, memLevel, strategy)
    z_stream *strm;
    int  level;
    int  method;
    int  windowBits;
    int  memLevel;
    int  strategy;
{
    deflate_state *s;
    int noheader = 0;

    if (strm == Z_NULL) return Z_STREAM_ERROR;

    strm->msg = Z_NULL;
    if (strm->zalloc == Z_NULL) strm->zalloc = zcalloc;
    if (strm->zfree == Z_NULL) strm->zfree = zcfree;

    if (level == Z_DEFAULT_COMPRESSION) level = 6;

    if (windowBits < 0) { /* undocumented feature: suppress zlib header */
	noheader = 1;
	windowBits = -windowBits;
    }
    if (memLevel < 1 || memLevel > MAX_MEM_LEVEL || method != DEFLATED ||
	windowBits < 8 || windowBits > 15 || level < 1 || level > 9) {
	return Z_STREAM_ERROR;
    }
    s = (deflate_state *) ZALLOC(strm, 1, sizeof(deflate_state));
    if (s == Z_NULL) return Z_MEM_ERROR;
    strm->state = (struct internal_state *)s;
    s->strm = strm;

    s->noheader = noheader;
    s->w_bits = windowBits;
    s->w_size = 1 << s->w_bits;
    s->w_mask = s->w_size - 1;

    s->hash_bits = memLevel + 7;
    s->hash_size = 1 << s->hash_bits;
    s->hash_mask = s->hash_size - 1;
    s->hash_shift =  ((s->hash_bits+MIN_MATCH-1)/MIN_MATCH);

    s->window = (Byte*) ZALLOC(strm, s->w_size, 2*sizeof(Byte));
    s->prev   = (Pos*)  ZALLOC(strm, s->w_size, sizeof(Pos));
    s->head   = (Pos*)  ZALLOC(strm, s->hash_size, sizeof(Pos));

    s->lit_bufsize = 1 << (memLevel + 6); /* 16K elements by default */

    s->pending_buf = (uch*) ZALLOC(strm, s->lit_bufsize, 2*sizeof(ush));

    if (s->window == Z_NULL || s->prev == Z_NULL || s->head == Z_NULL ||
	s->pending_buf == Z_NULL) {
	strm->msg = z_errmsg[1-Z_MEM_ERROR];
	deflateEnd (strm);
	return Z_MEM_ERROR;
    }
    s->d_buf = (ush*) &(s->pending_buf[s->lit_bufsize]);
    s->l_buf = (uch*) &(s->pending_buf[3*s->lit_bufsize]);
    /* We overlay pending_buf and d_buf+l_buf. This works since the average
     * output size for (length,distance) codes is <= 32 bits (worst case
     * is 15+15+13=33).
     */

    s->level = level;
    s->strategy = strategy;
    s->method = (Byte)method;

    return deflateReset(strm);
}

local int inflateInit2(z, w)
z_stream *z;
int w;
{
  /* initialize state */
  if (z == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->zalloc == Z_NULL) z->zalloc = zcalloc;
  if (z->zfree == Z_NULL) z->zfree = zcfree;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  if ((z->state = (struct internal_state *)
       ZALLOC(z,1,sizeof(struct internal_state))) == Z_NULL)
    return Z_MEM_ERROR;
  z->state->mode = METHOD;

  /* handle undocumented nowrap option (no zlib header or check) */
  z->state->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    z->state->nowrap = 1;
    z->state->mode = START;
  }

  /* set window size */
  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return Z_STREAM_ERROR;
  }
  z->state->wbits = w;
  return Z_OK;
}

local void putShortMSB (s, b)
    deflate_state *s;
    uInt b;
{
    put_byte(s, (Byte)(b >> 8));
    put_byte(s, (Byte)(b & 0xff));
}

local void flush_pending(strm)
    z_stream *strm;
{
    unsigned len = strm->state->pending;

    if (len > strm->avail_out) len = strm->avail_out;
    if (len == 0) return;

    zmemcpy(strm->next_out, strm->state->pending_out, len);
    strm->next_out  += len;
    strm->state->pending_out  += len;
    strm->total_out += len;
    strm->avail_out  -= len;
    strm->state->pending -= len;
    if (strm->state->pending == 0) {
	strm->state->pending_out = strm->state->pending_buf;
    }
}

local void fill_window(s)
    deflate_state *s;
{
    register unsigned n, m;
    unsigned more;    /* Amount of free space at the end of the window. */

    do {
	more = (unsigned)(s->window_size -(ulg)s->lookahead -(ulg)s->strstart);

	/* Deal with !@#$% 64K limit: */
	if (more == 0 && s->strstart == 0 && s->lookahead == 0) {
	    more = s->w_size;
        } else if (more == (unsigned)(-1)) {
            /* Very unlikely, but possible on 16 bit machine if strstart == 0
             * and lookahead == 1 (input done one byte at time)
             */
            more--;

        /* If the window is almost full and there is insufficient lookahead,
         * move the upper half to the lower one to make room in the upper half.
         */
        } else if (s->strstart >= s->w_size+MAX_DIST(s)) {

            /* By the IN assertion, the window is not empty so we can't confuse
             * more == 0 with more == 64K on a 16 bit machine.
             */
            memcpy((char*)s->window, (char*)s->window+s->w_size,
		   (unsigned)s->w_size);
            s->match_start -= s->w_size;
            s->strstart    -= s->w_size; /* we now have strstart >= MAX_DIST */

            s->block_start -= (long) s->w_size;

            for (n = 0; n < s->hash_size; n++) {
                m = s->head[n];
                s->head[n] = (Pos)(m >= s->w_size ? m-s->w_size : NIL);
            }
            for (n = 0; n < s->w_size; n++) {
                m = s->prev[n];
                s->prev[n] = (Pos)(m >= s->w_size ? m-s->w_size : NIL);
                /* If n is not on any hash chain, prev[n] is garbage but
                 * its value will never be used.
                 */
            }
            more += s->w_size;
        }
        if (s->strm->avail_in == 0) return;

        /* If there was no sliding:
         *    strstart <= WSIZE+MAX_DIST-1 && lookahead <= MIN_LOOKAHEAD - 1 &&
         *    more == window_size - lookahead - strstart
         * => more >= window_size - (MIN_LOOKAHEAD-1 + WSIZE + MAX_DIST-1)
         * => more >= window_size - 2*WSIZE + 2
         * In the BIG_MEM or MMAP case (not yet supported),
         *   window_size == input_size + MIN_LOOKAHEAD  &&
         *   strstart + s->lookahead <= input_size => more >= MIN_LOOKAHEAD.
         * Otherwise, window_size == 2*WSIZE so more >= 2.
         * If there was sliding, more >= WSIZE. So in all cases, more >= 2.
         */
        Assert(more >= 2, "more < 2");

        n = read_buf(s->strm, (char*)s->window + s->strstart + s->lookahead,
		     more);
	s->lookahead += n;

    } while (s->lookahead < MIN_LOOKAHEAD && s->strm->avail_in != 0);
}

/* For 80x86 and 680x0, an optimized version will be provided in match.asm or
 * match.S. The code will be functionally equivalent.
 */
local int longest_match(s, cur_match)
    deflate_state *s;
    IPos cur_match;                             /* current match */
{
    unsigned chain_length = s->max_chain_length;/* max hash chain length */
    register Byte *scan = s->window + s->strstart; /* current string */
    register Byte *match;                       /* matched string */
    register int len;                           /* length of current match */
    int best_len = s->prev_length;              /* best match length so far */
    IPos limit = s->strstart > (IPos)MAX_DIST(s) ?
	s->strstart - (IPos)MAX_DIST(s) : NIL;
    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */

#ifdef UNALIGNED_OK
    /* Compare two bytes at a time. Note: this is not always beneficial.
     * Try with and without -DUNALIGNED_OK to check.
     */
    register Byte *strend = s->window + s->strstart + MAX_MATCH - 1;
    register ush scan_start = *(ush*)scan;
    register ush scan_end   = *(ush*)(scan+best_len-1);
#else
    register Byte *strend = s->window + s->strstart + MAX_MATCH;
    register Byte scan_end1  = scan[best_len-1];
    register Byte scan_end   = scan[best_len];
#endif

    /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
     * It is easy to get rid of this optimization if necessary.
     */
    Assert(s->hash_bits >= 8 && MAX_MATCH == 258, "Code too clever");

    /* Do not waste too much time if we already have a good match: */
    if (s->prev_length >= s->good_match) {
        chain_length >>= 2;
    }
    Assert(s->strstart <= s->window_size-MIN_LOOKAHEAD, "need lookahead");

    do {
        Assert(cur_match < s->strstart, "no future");
        match = s->window + cur_match;

        /* Skip to next match if the match length cannot increase
         * or if the match length is less than 2:
         */
#if (defined(UNALIGNED_OK) && MAX_MATCH == 258)
        /* This code assumes sizeof(unsigned short) == 2. Do not use
         * UNALIGNED_OK if your compiler uses a different size.
         */
        if (*(ush*)(match+best_len-1) != scan_end ||
            *(ush*)match != scan_start) continue;

        /* It is not necessary to compare scan[2] and match[2] since they are
         * always equal when the other bytes match, given that the hash keys
         * are equal and that HASH_BITS >= 8. Compare 2 bytes at a time at
         * strstart+3, +5, ... up to strstart+257. We check for insufficient
         * lookahead only every 4th comparison; the 128th check will be made
         * at strstart+257. If MAX_MATCH-2 is not a multiple of 8, it is
         * necessary to put more guard bytes at the end of the window, or
         * to check more often for insufficient lookahead.
         */
        scan++, match++;
        do {
        } while (*(ush*)(scan+=2) == *(ush*)(match+=2) &&
                 *(ush*)(scan+=2) == *(ush*)(match+=2) &&
                 *(ush*)(scan+=2) == *(ush*)(match+=2) &&
                 *(ush*)(scan+=2) == *(ush*)(match+=2) &&
                 scan < strend);
        /* The funny "do {}" generates better code on most compilers */

        /* Here, scan <= window+strstart+257 */
        Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");
        if (*scan == *match) scan++;

        len = (MAX_MATCH - 1) - (int)(strend-scan);
        scan = strend - (MAX_MATCH-1);

#else /* UNALIGNED_OK */

        if (match[best_len]   != scan_end  ||
            match[best_len-1] != scan_end1 ||
            *match            != *scan     ||
            *++match          != scan[1])      continue;

        /* The check at best_len-1 can be removed because it will be made
         * again later. (This heuristic is not always a win.)
         * It is not necessary to compare scan[2] and match[2] since they
         * are always equal when the other bytes match, given that
         * the hash keys are equal and that HASH_BITS >= 8.
         */
        scan += 2, match++;

        /* We check for insufficient lookahead only every 8th comparison;
         * the 256th check will be made at strstart+258.
         */
        do {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);

        Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");

        len = MAX_MATCH - (int)(strend - scan);
        scan = strend - MAX_MATCH;

#endif /* UNALIGNED_OK */

        if (len > best_len) {
            s->match_start = cur_match;
            best_len = len;
            if (len >= s->nice_match) break;
#ifdef UNALIGNED_OK
            scan_end = *(ush*)(scan+best_len-1);
#else
            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
#endif
        }
    } while ((cur_match = s->prev[cur_match & s->w_mask]) > limit
             && --chain_length != 0);

    return best_len;
}

local int ct_tally (s, dist, lc)
    deflate_state *s;
    int dist;  /* distance of matched string */
    int lc;    /* match length-MIN_MATCH or unmatched char (if dist==0) */
{
    s->d_buf[s->last_lit] = (ush)dist;
    s->l_buf[s->last_lit++] = (uch)lc;
    if (dist == 0) {
        /* lc is the unmatched char */
        s->dyn_ltree[lc].Freq++;
    } else {
	s->matches++;
        /* Here, lc is the match length - MIN_MATCH */
        dist--;             /* dist = match distance - 1 */
        Assert((ush)dist < (ush)MAX_DIST(s) &&
               (ush)lc <= (ush)(MAX_MATCH-MIN_MATCH) &&
               (ush)d_code(dist) < (ush)D_CODES,  "ct_tally: bad match");

        s->dyn_ltree[length_code[lc]+LITERALS+1].Freq++;
        s->dyn_dtree[d_code(dist)].Freq++;
    }

    /* Try to guess if it is profitable to stop the current block here */
    if (s->level > 2 && (s->last_lit & 0xfff) == 0) {
        /* Compute an upper bound for the compressed length */
        ulg out_length = (ulg)s->last_lit*8L;
        ulg in_length = (ulg)s->strstart - s->block_start;
        int dcode;
        for (dcode = 0; dcode < D_CODES; dcode++) {
            out_length += (ulg)s->dyn_dtree[dcode].Freq *
		(5L+extra_dbits[dcode]);
        }
        out_length >>= 3;

        if (s->matches < s->last_lit/2 && out_length < in_length/2) return 1;
    }
    return (s->last_lit == s->lit_bufsize-1);
    /* We avoid equality with lit_bufsize because of wraparound at 64K
     * on 16 bit machines and because stored blocks are restricted to
     * 64K-1 bytes.
     */
}

local void set_data_type(s)
    deflate_state *s;
{
    int n = 0;
    unsigned ascii_freq = 0;
    unsigned bin_freq = 0;
    while (n < 7)        bin_freq += s->dyn_ltree[n++].Freq;
    while (n < 128)    ascii_freq += s->dyn_ltree[n++].Freq;
    while (n < LITERALS) bin_freq += s->dyn_ltree[n++].Freq;
    s->data_type = (Byte)(bin_freq > (ascii_freq >> 2) ? BINARY : ASCII);
}

local void pqdownheap(s, tree, k)
    deflate_state *s;
    ct_data *tree;  /* the tree to restore */
    int k;               /* node to move down */
{
    int v = s->heap[k];
    int j = k << 1;  /* left son of k */
    while (j <= s->heap_len) {
        /* Set j to the smallest of the two sons: */
        if (j < s->heap_len &&
	    smaller(tree, s->heap[j+1], s->heap[j], s->depth)) {
	    j++;
	}
        /* Exit if v is smaller than both sons */
        if (smaller(tree, v, s->heap[j], s->depth)) break;

        /* Exchange v with the smallest son */
        s->heap[k] = s->heap[j];  k = j;

        /* And continue down the tree, setting j to the left son of k */
        j <<= 1;
    }
    s->heap[k] = v;
}

local void gen_bitlen(s, desc)
    deflate_state *s;
    tree_desc *desc;    /* the tree descriptor */
{
    ct_data *tree  = desc->dyn_tree;
    int max_code   = desc->max_code;
    ct_data *stree = desc->stat_desc->static_tree;
    int *extra     = desc->stat_desc->extra_bits;
    int base       = desc->stat_desc->extra_base;
    int max_length = desc->stat_desc->max_length;
    int h;              /* heap index */
    int n, m;           /* iterate over the tree elements */
    int bits;           /* bit length */
    int xbits;          /* extra bits */
    ush f;              /* frequency */
    int overflow = 0;   /* number of elements with bit length too large */

    for (bits = 0; bits <= MAX_BITS; bits++) s->bl_count[bits] = 0;

    /* In a first pass, compute the optimal bit lengths (which may
     * overflow in the case of the bit length tree).
     */
    tree[s->heap[s->heap_max]].Len = 0; /* root of the heap */

    for (h = s->heap_max+1; h < HEAP_SIZE; h++) {
        n = s->heap[h];
        bits = tree[tree[n].Dad].Len + 1;
        if (bits > max_length) bits = max_length, overflow++;
        tree[n].Len = (ush)bits;
        /* We overwrite tree[n].Dad which is no longer needed */

        if (n > max_code) continue; /* not a leaf node */

        s->bl_count[bits]++;
        xbits = 0;
        if (n >= base) xbits = extra[n-base];
        f = tree[n].Freq;
        s->opt_len += (ulg)f * (bits + xbits);
        if (stree) s->static_len += (ulg)f * (stree[n].Len + xbits);
    }
    if (overflow == 0) return;

    /* This happens for example on obj2 and pic of the Calgary corpus */

    /* Find the first bit length which could increase: */
    do {
        bits = max_length-1;
        while (s->bl_count[bits] == 0) bits--;
        s->bl_count[bits]--;      /* move one leaf down the tree */
        s->bl_count[bits+1] += 2; /* move one overflow item as its brother */
        s->bl_count[max_length]--;
        /* The brother of the overflow item also moves one step up,
         * but this does not affect bl_count[max_length]
         */
        overflow -= 2;
    } while (overflow > 0);

    /* Now recompute all bit lengths, scanning in increasing frequency.
     * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
     * lengths instead of fixing only the wrong ones. This idea is taken
     * from 'ar' written by Haruhiko Okumura.)
     */
    for (bits = max_length; bits != 0; bits--) {
        n = s->bl_count[bits];
        while (n != 0) {
            m = s->heap[--h];
            if (m > max_code) continue;
            if (tree[m].Len != (unsigned) bits) {

                s->opt_len += ((long)bits - (long)tree[m].Len)
		              *(long)tree[m].Freq;
                tree[m].Len = (ush)bits;
            }
            n--;
        }
    }
}

local void build_tree(s, desc)
    deflate_state *s;
    tree_desc *desc; /* the tree descriptor */
{
    ct_data *tree   = desc->dyn_tree;
    ct_data *stree  = desc->stat_desc->static_tree;
    int elems       = desc->stat_desc->elems;
    int n, m;          /* iterate over heap elements */
    int max_code = -1; /* largest code with non zero frequency */
    int node = elems;  /* next internal node of the tree */
    int new;           /* new node being created */

    /* Construct the initial heap, with least frequent element in
     * heap[SMALLEST]. The sons of heap[n] are heap[2*n] and heap[2*n+1].
     * heap[0] is not used.
     */
    s->heap_len = 0, s->heap_max = HEAP_SIZE;

    for (n = 0; n < elems; n++) {
        if (tree[n].Freq != 0) {
            s->heap[++(s->heap_len)] = max_code = n;
            s->depth[n] = 0;
        } else {
            tree[n].Len = 0;
        }
    }

    /* The pkzip format requires that at least one distance code exists,
     * and that at least one bit should be sent even if there is only one
     * possible code. So to avoid special checks later on we force at least
     * two codes of non zero frequency.
     */
    while (s->heap_len < 2) {
        new = s->heap[++(s->heap_len)] = (max_code < 2 ? ++max_code : 0);
        tree[new].Freq = 1;
        s->depth[new] = 0;
        s->opt_len--; if (stree) s->static_len -= stree[new].Len;
        /* new is 0 or 1 so it does not have extra bits */
    }
    desc->max_code = max_code;

    /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
     * establish sub-heaps of increasing lengths:
     */
    for (n = s->heap_len/2; n >= 1; n--) pqdownheap(s, tree, n);

    /* Construct the Huffman tree by repeatedly combining the least two
     * frequent nodes.
     */
    do {
        pqremove(s, tree, n);  /* n = node of least frequency */
        m = s->heap[SMALLEST]; /* m = node of next least frequency */

        s->heap[--(s->heap_max)] = n; /* keep the nodes sorted by frequency */
        s->heap[--(s->heap_max)] = m;

        /* Create a new node father of n and m */
        tree[node].Freq = tree[n].Freq + tree[m].Freq;
        s->depth[node] = (uch) (MAX(s->depth[n], s->depth[m]) + 1);
        tree[n].Dad = tree[m].Dad = (ush)node;
#ifdef DUMP_BL_TREE
        if (tree == s->bl_tree) {
            fprintf(stderr,"\nnode %d(%d), sons %d(%d) %d(%d)",
                    node, tree[node].Freq, n, tree[n].Freq, m, tree[m].Freq);
        }
#endif
        /* and insert the new node in the heap */
        s->heap[SMALLEST] = node++;
        pqdownheap(s, tree, SMALLEST);

    } while (s->heap_len >= 2);

    s->heap[--(s->heap_max)] = s->heap[SMALLEST];

    /* At this point, the fields freq and dad are set. We can now
     * generate the bit lengths.
     */
    gen_bitlen(s, (tree_desc *)desc);

    /* The field len is now set, we can generate the bit codes */
    gen_codes ((ct_data *)tree, max_code, s->bl_count);
}

local void scan_tree (s, tree, max_code)
    deflate_state *s;
    ct_data *tree;   /* the tree to be scanned */
    int max_code;    /* and its largest code of non zero frequency */
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    if (nextlen == 0) max_count = 138, min_count = 3;
    tree[max_code+1].Len = (ush)0xffff; /* guard */

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            s->bl_tree[curlen].Freq += count;
        } else if (curlen != 0) {
            if (curlen != prevlen) s->bl_tree[curlen].Freq++;
            s->bl_tree[REP_3_6].Freq++;
        } else if (count <= 10) {
            s->bl_tree[REPZ_3_10].Freq++;
        } else {
            s->bl_tree[REPZ_11_138].Freq++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

local int build_bl_tree(s)
    deflate_state *s;
{
    int max_blindex;  /* index of last bit length code of non zero freq */

    /* Determine the bit length frequencies for literal and distance trees */
    scan_tree(s, (ct_data *)s->dyn_ltree, s->l_desc.max_code);
    scan_tree(s, (ct_data *)s->dyn_dtree, s->d_desc.max_code);

    /* Build the bit length tree: */
    build_tree(s, (tree_desc *)(&(s->bl_desc)));
    /* opt_len now includes the length of the tree representations, except
     * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
     */

    /* Determine the number of bit length codes to send. The pkzip format
     * requires that at least 4 bit length codes be sent. (appnote.txt says
     * 3 but the actual value used is 4.)
     */
    for (max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex--) {
        if (s->bl_tree[bl_order[max_blindex]].Len != 0) break;
    }
    /* Update opt_len to include the bit length tree and counts */
    s->opt_len += 3*(max_blindex+1) + 5+5+4;

    return max_blindex;
}

local void send_bits(s, value, length)
    deflate_state *s;
    int value;  /* value to send */
    int length; /* number of bits */
{
#ifdef DEBUG
    Assert(length > 0 && length <= 15, "invalid length");
    s->bits_sent += (ulg)length;
#endif
    /* If not enough room in bi_buf, use (valid) bits from bi_buf and
     * (16 - bi_valid) bits from value, leaving (width - (16-bi_valid))
     * unused bits in value.
     */
    if (s->bi_valid > (int)Buf_size - length) {
        s->bi_buf |= (value << s->bi_valid);
        put_short(s, s->bi_buf);
        s->bi_buf = (ush)value >> (Buf_size - s->bi_valid);
        s->bi_valid += length - Buf_size;
    } else {
        s->bi_buf |= value << s->bi_valid;
        s->bi_valid += length;
    }
}

local void bi_windup(s)
    deflate_state *s;
{
    if (s->bi_valid > 8) {
        put_short(s, s->bi_buf);
    } else if (s->bi_valid > 0) {
        put_byte(s, (Byte)s->bi_buf);
    }
    s->bi_buf = 0;
    s->bi_valid = 0;
#ifdef DEBUG
    s->bits_sent = (s->bits_sent+7) & ~7;
#endif
}

local void copy_block(s, buf, len, header)
    deflate_state *s;
    char     *buf;    /* the input data */
    unsigned len;     /* its length */
    int      header;  /* true if block header must be written */
{
    bi_windup(s);              /* align on byte boundary */

    if (header) {
        put_short(s, (ush)len);
        put_short(s, (ush)~len);
#ifdef DEBUG
        s->bits_sent += 2*16;
#endif
    }
#ifdef DEBUG
    s->bits_sent += (ulg)len<<3;
#endif
    while (len--) {
	put_byte(s, *buf++);
    }
}

local void compress_block(s, ltree, dtree)
    deflate_state *s;
    ct_data *ltree; /* literal tree */
    ct_data *dtree; /* distance tree */
{
    unsigned dist;      /* distance of matched string */
    int lc;             /* match length or unmatched char (if dist == 0) */
    unsigned lx = 0;    /* running index in l_buf */
    unsigned code;      /* the code to send */
    int extra;          /* number of extra bits to send */

    if (s->last_lit != 0) do {
	dist = s->d_buf[lx];
        lc = s->l_buf[lx++];
        if (dist == 0) {
            send_code(s, lc, ltree); /* send a literal byte */
        } else {
            /* Here, lc is the match length - MIN_MATCH */
            code = length_code[lc];
            send_code(s, code+LITERALS+1, ltree); /* send the length code */
            extra = extra_lbits[code];
            if (extra != 0) {
                lc -= base_length[code];
                send_bits(s, lc, extra);       /* send the extra length bits */
            }
	    dist--; /* dist is now the match distance - 1 */
            code = d_code(dist);
            Assert (code < D_CODES, "bad d_code");

            send_code(s, code, dtree);       /* send the distance code */
            extra = extra_dbits[code];
            if (extra != 0) {
                dist -= base_dist[code];
                send_bits(s, dist, extra);   /* send the extra distance bits */
            }
        } /* literal or match pair ? */

	/* Check that the overlay between pending_buf and d_buf+l_buf is ok: */
	Assert(s->pending < s->lit_bufsize + 2*lx, "pendingBuf overflow");

    } while (lx < s->last_lit);

    send_code(s, END_BLOCK, ltree);
}

local void send_tree (s, tree, max_code)
    deflate_state *s;
    ct_data *tree; /* the tree to be scanned */
    int max_code;       /* and its largest code of non zero frequency */
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    /* tree[max_code+1].Len = -1; */  /* guard already set */
    if (nextlen == 0) max_count = 138, min_count = 3;

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do { send_code(s, curlen, s->bl_tree); } while (--count != 0);

        } else if (curlen != 0) {
            if (curlen != prevlen) {
                send_code(s, curlen, s->bl_tree); count--;
            }
            Assert(count >= 3 && count <= 6, " 3_6?");
            send_code(s, REP_3_6, s->bl_tree); send_bits(s, count-3, 2);

        } else if (count <= 10) {
            send_code(s, REPZ_3_10, s->bl_tree); send_bits(s, count-3, 3);

        } else {
            send_code(s, REPZ_11_138, s->bl_tree); send_bits(s, count-11, 7);
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

local void send_all_trees(s, lcodes, dcodes, blcodes)
    deflate_state *s;
    int lcodes, dcodes, blcodes; /* number of codes for each tree */
{
    int rank;                    /* index in bl_order */

    Assert (lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
    Assert (lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
            "too many codes");
    send_bits(s, lcodes-257, 5); /* not +255 as stated in appnote.txt */
    send_bits(s, dcodes-1,   5);
    send_bits(s, blcodes-4,  4); /* not -3 as stated in appnote.txt */
    for (rank = 0; rank < blcodes; rank++) {
        send_bits(s, s->bl_tree[bl_order[rank]].Len, 3);
    }

    send_tree(s, (ct_data *)s->dyn_ltree, lcodes-1); /* literal tree */

    send_tree(s, (ct_data *)s->dyn_dtree, dcodes-1); /* distance tree */
}

ulg ct_flush_block(s, buf, stored_len, eof)
    deflate_state *s;
    char *buf;        /* input block, or NULL if too old */
    ulg stored_len;   /* length of input block */
    int eof;          /* true if this is the last block for a file */
{
    ulg opt_lenb, static_lenb; /* opt_len and static_len in bytes */
    int max_blindex;  /* index of last bit length code of non zero freq */

     /* Check if the file is ascii or binary */
    if (s->data_type == UNKNOWN) set_data_type(s);

    /* Construct the literal and distance trees */
    build_tree(s, (tree_desc *)(&(s->l_desc)));


    build_tree(s, (tree_desc *)(&(s->d_desc)));

    /* At this point, opt_len and static_len are the total bit lengths of
     * the compressed block data, excluding the tree representations.
     */

    /* Build the bit length tree for the above two trees, and get the index
     * in bl_order of the last bit length code to send.
     */
    max_blindex = build_bl_tree(s);

    /* Determine the best encoding. Compute first the block length in bytes */
    opt_lenb = (s->opt_len+3+7)>>3;
    static_lenb = (s->static_len+3+7)>>3;

    if (static_lenb <= opt_lenb) opt_lenb = static_lenb;

    /* If compression failed and this is the first and last block,
     * and if the .zip file can be seeked (to rewrite the local header),
     * the whole file is transformed into a stored file:
     */

    if (stored_len+4 <= opt_lenb && buf != (char*)0) {
        /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
         * Otherwise we can't have processed more than WSIZE input bytes since
         * the last block flush, because compression would have been
         * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
         * transform a block into a stored block.
         */
        send_bits(s, (STORED_BLOCK<<1)+eof, 3);  /* send block type */
        s->compressed_len = (s->compressed_len + 3 + 7) & ~7L;
        s->compressed_len += (stored_len + 4) << 3;

        copy_block(s, buf, (unsigned)stored_len, 1); /* with header */

    } else if (static_lenb == opt_lenb) {
        send_bits(s, (STATIC_TREES<<1)+eof, 3);
        compress_block(s, (ct_data *)static_ltree, (ct_data *)static_dtree);
        s->compressed_len += 3 + s->static_len;
    } else {
        send_bits(s, (DYN_TREES<<1)+eof, 3);
        send_all_trees(s, s->l_desc.max_code+1, s->d_desc.max_code+1,
		       max_blindex+1);
        compress_block(s, (ct_data *)s->dyn_ltree, (ct_data *)s->dyn_dtree);
        s->compressed_len += 3 + s->opt_len;
    }
    Assert (s->compressed_len == s->bits_sent, "bad compressed size");
    init_block(s);

    if (eof) {
        bi_windup(s);
        s->compressed_len += 7;  /* align on byte boundary */
    }

    return s->compressed_len >> 3;
}

local int deflate_fast(s, flush)
    deflate_state *s;
    int flush;
{
    IPos hash_head; /* head of the hash chain */
    int bflush;     /* set if current block must be flushed */

    s->prev_length = MIN_MATCH-1;

    for (;;) {
        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need MAX_MATCH bytes
         * for the next match, plus MIN_MATCH bytes to insert the
         * string following the next match.
         */
        if (s->lookahead < MIN_LOOKAHEAD) {
	    fill_window(s);
	    if (s->lookahead < MIN_LOOKAHEAD && flush == Z_NO_FLUSH) return 1;

	    if (s->lookahead == 0) break; /* flush the current block */
	}

        /* Insert the string window[strstart .. strstart+2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */
        INSERT_STRING(s, s->strstart, hash_head);

        /* Find the longest match, discarding those <= prev_length.
         * At this point we have always match_length < MIN_MATCH
         */
        if (hash_head != NIL && s->strstart - hash_head <= MAX_DIST(s)) {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */
	    if (s->strategy != Z_HUFFMAN_ONLY) {
		s->match_length = longest_match (s, hash_head);
	    }
            /* longest_match() sets match_start */

            if (s->match_length > s->lookahead) s->match_length = s->lookahead;
        }
        if (s->match_length >= MIN_MATCH) {
            check_match(s, s->strstart, s->match_start, s->match_length);

            bflush = ct_tally(s, s->strstart - s->match_start,
		 	      s->match_length - MIN_MATCH);

            s->lookahead -= s->match_length;

            /* Insert new strings in the hash table only if the match length
             * is not too large. This saves time but degrades compression.
             */
            if (s->match_length <= s->max_insert_length) {
                s->match_length--; /* string at strstart already in hash table */
                do {
                    s->strstart++;
                    INSERT_STRING(s, s->strstart, hash_head);
                    /* strstart never exceeds WSIZE-MAX_MATCH, so there are
                     * always MIN_MATCH bytes ahead. If lookahead < MIN_MATCH
                     * these bytes are garbage, but it does not matter since
                     * the next lookahead bytes will be emitted as literals.
                     */
                } while (--s->match_length != 0);
                s->strstart++;
            } else {
                s->strstart += s->match_length;
                s->match_length = 0;
                s->ins_h = s->window[s->strstart];
                UPDATE_HASH(s, s->ins_h, s->window[s->strstart+1]);
#if MIN_MATCH != 3
                Call UPDATE_HASH() MIN_MATCH-3 more times
#endif
            }
        } else {
            /* No match, output a literal byte */
            bflush = ct_tally (s, 0, s->window[s->strstart]);
            s->lookahead--;
            s->strstart++;
        }
        if (bflush) FLUSH_BLOCK(s, 0);
    }
    FLUSH_BLOCK(s, flush == Z_FINISH);
    return 0; /* normal exit */
}

local int deflate_slow(s, flush)
    deflate_state *s;
    int flush;
{
    IPos hash_head;          /* head of hash chain */
    int bflush;              /* set if current block must be flushed */

    /* Process the input block. */
    for (;;) {
        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need MAX_MATCH bytes
         * for the next match, plus MIN_MATCH bytes to insert the
         * string following the next match.
         */
        if (s->lookahead < MIN_LOOKAHEAD) {
	    fill_window(s);
	    if (s->lookahead < MIN_LOOKAHEAD && flush == Z_NO_FLUSH) return 1;

	    if (s->lookahead == 0) break; /* flush the current block */
	}

        /* Insert the string window[strstart .. strstart+2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */
        INSERT_STRING(s, s->strstart, hash_head);

        /* Find the longest match, discarding those <= prev_length.
         */
        s->prev_length = s->match_length, s->prev_match = s->match_start;
        s->match_length = MIN_MATCH-1;

        if (hash_head != NIL && s->prev_length < s->max_lazy_match &&
            s->strstart - hash_head <= MAX_DIST(s)) {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */
	    if (s->strategy != Z_HUFFMAN_ONLY) {
		s->match_length = longest_match (s, hash_head);
	    }
            /* longest_match() sets match_start */
            if (s->match_length > s->lookahead) s->match_length = s->lookahead;

            if (s->match_length <= 5 && (s->strategy == Z_FILTERED ||
	         (s->match_length == MIN_MATCH &&
		  s->strstart - s->match_start > TOO_FAR))) {

                /* If prev_match is also MIN_MATCH, match_start is garbage
                 * but we will ignore the current match anyway.
                 */
                s->match_length = MIN_MATCH-1;
            }
        }
        /* If there was a match at the previous step and the current
         * match is not better, output the previous match:
         */
        if (s->prev_length >= MIN_MATCH && s->match_length <= s->prev_length) {

            check_match(s, s->strstart-1, s->prev_match, s->prev_length);

            bflush = ct_tally(s, s->strstart -1 - s->prev_match,
			      s->prev_length - MIN_MATCH);

            /* Insert in hash table all strings up to the end of the match.
             * strstart-1 and strstart are already inserted.
             */
            s->lookahead -= s->prev_length-1;
            s->prev_length -= 2;
            do {
                s->strstart++;
                INSERT_STRING(s, s->strstart, hash_head);
                /* strstart never exceeds WSIZE-MAX_MATCH, so there are
                 * always MIN_MATCH bytes ahead. If lookahead < MIN_MATCH
                 * these bytes are garbage, but it does not matter since the
                 * next lookahead bytes will always be emitted as literals.
                 */
            } while (--s->prev_length != 0);
            s->match_available = 0;
            s->match_length = MIN_MATCH-1;
            s->strstart++;

            if (bflush) FLUSH_BLOCK(s, 0);

        } else if (s->match_available) {
            /* If there was no match at the previous position, output a
             * single literal. If there was a match but the current match
             * is longer, truncate the previous match to a single literal.
             */
            if (ct_tally (s, 0, s->window[s->strstart-1])) {
	        FLUSH_BLOCK_ONLY(s, 0);
            }
            s->strstart++;
            s->lookahead--;
	    if (s->strm->avail_out == 0) return 1;
        } else {
            /* There is no previous match to compare with, wait for
             * the next step to decide.
             */
            s->match_available = 1;
            s->strstart++;
            s->lookahead--;
        }
    }
    if (s->match_available) ct_tally (s, 0, s->window[s->strstart-1]);

    FLUSH_BLOCK(s, flush == Z_FINISH);
    return 0;
}

local int deflate (strm, flush)
    z_stream *strm;
    int flush;
{
    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;

    if (strm->next_out == Z_NULL || strm->next_in == Z_NULL) {
	ERR_RETURN(strm, Z_STREAM_ERROR);
    }
    if (strm->avail_out == 0) ERR_RETURN(strm, Z_BUF_ERROR);

    strm->state->strm = strm; /* just in case */

    /* Write the zlib header */
    if (strm->state->status == INIT_STATE) {

	uInt header = (DEFLATED + ((strm->state->w_bits-8)<<4)) << 8;
        uInt level_flags = (strm->state->level-1) >> 1;

	if (level_flags > 3) level_flags = 3;
	header |= (level_flags << 6);
	header += 31 - (header % 31);

	strm->state->status = BUSY_STATE;
	putShortMSB(strm->state, header);
    }

    /* Flush as much pending output as possible */
    if (strm->state->pending != 0) {
	flush_pending(strm);
	if (strm->avail_out == 0) return Z_OK;
    }

    /* User must not provide more input after the first FINISH: */
    if (strm->state->status == FINISH_STATE && strm->avail_in != 0) {
	ERR_RETURN(strm, Z_BUF_ERROR);
    }

    /* Start a new block or continue the current one.
     */
    if (strm->avail_in != 0 ||
	(flush == Z_FINISH && strm->state->status != FINISH_STATE)) {

	if (flush == Z_FINISH) {
	    strm->state->status = FINISH_STATE;
	}
        if (strm->state->level <= 3) {
	    if (deflate_fast(strm->state, flush)) return Z_OK;
	} else {
	    if (deflate_slow(strm->state, flush)) return Z_OK;
	}
    }
    Assert(strm->avail_out > 0, "bug2");

    if (flush != Z_FINISH) return Z_OK;
    if (strm->state->noheader) return Z_STREAM_END;

    /* Write the zlib trailer (adler32) */
    putShortMSB(strm->state, (uInt)(strm->state->adler >> 16));
    putShortMSB(strm->state, (uInt)(strm->state->adler & 0xffff));
    flush_pending(strm);
    /* If avail_out is zero, the application will call deflate again
     * to flush the rest.
     */
    strm->state->noheader = 1; /* write the trailer only once! */
    return strm->state->pending != 0 ? Z_OK : Z_STREAM_END;
}

local int gzflush (file, flush)
    gzFile file;
    int flush;
{
    uInt len;
    int done = 0;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;) {
	len = Z_BUFSIZE - s->stream.avail_out;

	if (len != 0) {
	    if (fwrite(s->outbuf, 1, len, s->file) != len) {
		s->z_err = Z_ERRNO;
		return Z_ERRNO;
	    }
	    s->stream.next_out = s->outbuf;
	    s->stream.avail_out = Z_BUFSIZE;
	}
	if (done) break;
	s->z_err = deflate(&(s->stream), flush);

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer:
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);

	if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}

local gzFile gz_open (path, mode, fd)
    char *path;
    char *mode;
    int  fd;
{
    int err;
    char *p = mode;
    gz_stream *s = (gz_stream *)ALLOC(sizeof(gz_stream));

    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)0;
    s->stream.zfree = (free_func)0;
    s->stream.next_in = s->inbuf = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->file = NULL;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

    s->path = (char*)ALLOC(strlen(path)+1);
    if (s->path == NULL) {
	return destroy(s), (gzFile)Z_NULL;
    }
    strcpy(s->path, path); /* do this early for debugging */

    s->mode = '\0';
    do {
	if (*p == 'r') s->mode = 'r';
	if (*p == 'w') s->mode = 'w';
    } while (*p++);
    if (s->mode == '\0') return destroy(s), (gzFile)Z_NULL;

    if (s->mode == 'w') {
	err = deflateInit2(&(s->stream), Z_BEST_COMPRESSION,
			   DEFLATED, -WBITS, MEM_LEVEL, 0);
	/* windowBits is passed < 0 to suppress zlib header */

	s->stream.next_out = s->outbuf = ALLOC(Z_BUFSIZE);

	if (err != Z_OK || s->outbuf == Z_NULL) {
	    return destroy(s), (gzFile)Z_NULL;
	}
    } else {
	err = inflateInit2(&(s->stream), -WBITS);
	s->stream.next_in  = s->inbuf = ALLOC(Z_BUFSIZE);

	if (err != Z_OK || s->inbuf == Z_NULL) {
	    return destroy(s), (gzFile)Z_NULL;
	}
    }
    s->stream.avail_out = Z_BUFSIZE;

    errno = 0;
    s->file = fd < 0 ? fopen(path, mode) : fdopen(fd, mode);

    if (s->file == NULL) {
	return destroy(s), (gzFile)Z_NULL;
    }
    if (s->mode == 'w') {
	/* Write a very simple .gz header:
	 */
	fprintf(s->file, "%c%c%c%c%c%c%c%c%c%c", GZ_MAGIC_1, GZ_MAGIC_2,
	       DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
    } else {
	/* Check and skip the header:
         */
	Byte c1 = 0, c2 = 0;
	Byte method = 0;
	Byte flags = 0;
	Byte xflags = 0;
	Byte time[4];
	Byte osCode;
	int c;

	s->stream.avail_in = fread(s->inbuf, 1, 2, s->file);
	if (s->stream.avail_in != 2 || s->inbuf[0] != GZ_MAGIC_1
	    || s->inbuf[1] != GZ_MAGIC_2) {
	    s->transparent = 1;
	    return (gzFile)s;
	}
	s->stream.avail_in = 0;
	err = fscanf(s->file,"%c%c%4c%c%c", &method, &flags, time, &xflags, &osCode);

	if (method != DEFLATED || feof(s->file) || (flags & RESERVED) != 0) {
	    s->z_err = Z_DATA_ERROR;
	    return (gzFile)s;
	}
	if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
	    long len;
	    err = fscanf(s->file, "%c%c", &c1, &c2);
	    len = c1 + ((long)c2<<8);
	    fseek(s->file, len, SEEK_CUR);
	}
	if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
	    while ((c = getc(s->file)) != 0 && c != EOF) ;
	}
	if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
	    while ((c = getc(s->file)) != 0 && c != EOF) ;
	}
	if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
	    err = fscanf(s->file, "%c%c", &c1, &c2);
	}
	if (feof(s->file)) {
	    s->z_err = Z_DATA_ERROR;
	}
    }
    return (gzFile)s;
}

local gzFile gzopen (path, mode)
    char *path;
    char *mode;
{
    return gz_open (path, mode, -1);
}

local int gzwrite (file, buf, len)
    gzFile file;
    voidp buf;
    unsigned len;
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.next_in = buf;
    s->stream.avail_in = len;

    while (s->stream.avail_in != 0) {

	if (s->stream.avail_out == 0) {

	    s->stream.next_out = s->outbuf;
	    if (fwrite(s->outbuf, 1, Z_BUFSIZE, s->file) != Z_BUFSIZE) {
		s->z_err = Z_ERRNO;
		break;
	    }
	    s->stream.avail_out = Z_BUFSIZE;
	}
	s->z_err = deflate(&(s->stream), Z_NO_FLUSH);

	if (s->z_err != Z_OK) break;
    }
    s->crc = crc32(s->crc, buf, len);

    return len - s->stream.avail_in;
}

local int gzclose (file)
    gzFile file;
{
    uInt n;
    int err;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL) return Z_STREAM_ERROR;

    if (s->mode == 'w') {
	err = gzflush (file, Z_FINISH);
	if (err != Z_OK) return destroy((gz_stream *)file);

	putLong (s->file, s->crc);
	putLong (s->file, s->stream.total_in);

    } else if (s->mode == 'r' && s->z_err == Z_STREAM_END) {

	/* slide CRC and original size if they are at the end of inbuf */
	if ((n = s->stream.avail_in) < 8  && !s->z_eof) {
	    Byte *p = s->inbuf;
	    Byte *q = s->stream.next_in;
	    while (n--) { *p++ = *q++; };

	    n = s->stream.avail_in;
	    n += fread(p, 1, 8, s->file);
	    s->stream.next_in = s->inbuf;
	}
	/* check CRC and original size */
	if (n < 8 ||
	    getLong(s->stream.next_in) != s->crc ||
	    getLong(s->stream.next_in + 4) != s->stream.total_out) {

	    s->z_err = Z_DATA_ERROR;
	}
    }
    return destroy((gz_stream *)file);
}

/* ========================================================================= */

static int wm_tool_printf(const char *format, ...)
{
    int ret;
    va_list ap;

	va_start(ap, format);
	ret = vprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	return ret;
}

static unsigned long long wm_tool_crc32_reflect(unsigned long long ref, unsigned char ch)
{
	int i;
	unsigned long long value = 0;

	for ( i = 1; i < ( ch + 1 ); i++ )
	{
		if( ref & 1 )
			value |= 1 << ( ch - i );
		ref >>= 1;
	}

	return value;
}

static unsigned int wm_tool_crc32(unsigned int crc, unsigned char *buffer, int size, wm_tool_crc32_reflect_e mode)
{
	int i;
	unsigned char temp;

	for (i = 0; i < size; i++)
	{
		if (mode & WM_TOOL_CRC32_REFLECT_INPUT)
		{
			temp = wm_tool_crc32_reflect(buffer[i], 8);
		}
		else
		{
			temp = buffer[i];
		}
		crc = wm_tool_crc32_tab[(crc ^ temp) & 0xff] ^ (crc >> 8);
	}

	return crc ;
}

static unsigned int wm_tool_get_crc32(unsigned char *buffer, int size, wm_tool_crc32_reflect_e mode)
{
	wm_tool_file_crc = wm_tool_crc32(wm_tool_file_crc, buffer, size, mode);
	if (mode & WM_TOOL_CRC32_REFLECT_OUTPUT)
	{
		wm_tool_file_crc = wm_tool_crc32_reflect(wm_tool_file_crc, 32);
	}
	return wm_tool_file_crc;
}

static unsigned short wm_tool_get_crc16(unsigned char *ptr, unsigned short count)
{
	unsigned short crc, i;

	crc = 0;

	while (count--)
	{
		crc = crc ^ (int) *ptr++ << 8;

		for (i = 0; i < 8; i++)
		{
			if (crc & 0x8000)
			    crc = crc << 1 ^ 0x1021;
			else
			    crc = crc << 1;
		}
	}

	return (crc & 0xFFFF);
}

static int wm_tool_char_to_hex(char ch)
{
	int hex;

	hex = -1;

	if ((ch >= '0') && (ch <= '9'))
	{
	    hex = ch - '0';
	}
	else if ((ch >= 'a') && (ch <= 'f'))
	{
	    hex = ch - 'a' + 0xa;
	}
	else if ((ch >= 'A') && (ch <= 'F'))
	{
	    hex = ch - 'A' + 0xa;
	}

	return hex;
}

static int wm_tool_str_to_hex_array(char *str, int cnt, unsigned char array[])
{
	int hex;
	unsigned char tmp;
	unsigned char *des;

	des = array;

	while (cnt-- > 0)
	{
		hex = wm_tool_char_to_hex(*str++);

		if (hex < 0)
		{
		    return -1;
		}
		else
		{
		    tmp = (hex << 4) & 0xf0;
		}

		hex = wm_tool_char_to_hex(*str++);

		if (hex < 0)
		{
		    return -1;
		}
		else
		{
		    tmp = tmp | (hex & 0x0f);
		}

		*des++ = (unsigned char) tmp;
	}

	return ((*str == 0) ? 0 : -1);
}

static char *wm_tool_strcasestr(const char *str1, const char *str2)
{
  char *cp = (char *) str1;
  char *s1, *s2;

  if (!*str2) return (char *) str1;

  while (*cp) {
    s1 = cp;
    s2 = (char *) str2;

    while (*s1 && *s2 && !(tolower((int)*s1) - tolower((int)*s2))) s1++, s2++;
    if (!*s2) return cp;
    cp++;
  }

  return NULL;
}

static int wm_tool_get_file_size(const char* filename)
{
    FILE *fp = fopen(filename, "r");
    if(!fp) return -1;
    fseek(fp,0L,SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    return size;
}

static char *wm_tool_get_name(const char *name)
{
    static char sz_name[WM_TOOL_PATH_MAX] = {0};
    char *p = (char *)name;
    char *q = (char *)name;

    do
    {
#ifdef __MINGW32__
        p = strchr(p, '\\');
#else
        p = strchr(p, '/');
#endif
        if (p)
        {
            p++;
            q = p;
        }
    } while (p);

    strncpy(sz_name, q, WM_TOOL_PATH_MAX);

#ifdef __MINGW32__
    p = wm_tool_strcasestr(sz_name, ".exe");
    if (p)
        *p = '\0';
#endif

    return sz_name;
}

static void wm_tool_print_usage(const char *name)
{
    wm_tool_printf("Usage: %s [-h] [-v] [-b] [-o] [-sb] [-ct] [-it] [-ua] [-ra] [-df]"
                   " [-l] [-c] [-ws] [-ds] [-rs] [-eo] [-dl] [-sl]"
                   "\r\n"
                   "\r\n"
                   "WinnerMicro firmware packaging "
                   "and programming "
                   "tool\r\n\r\n"
                   "options:\r\n\r\n"
                   "  -h                   , show usage\r\n"
                   "  -v                   , show version\r\n"
                   "\r\n"
                   "  -b  binary           , original binary file\r\n"
                   "  -o  output_name      , output firmware file\r\n"
                   "                         the default is the same as the original binary file name\r\n"
                   "  -sb second_boot      , second boot file, used to generate fls file\r\n"
                   "  -fc compress_type    , whether the firmware is compressed, default is compressed\r\n"
                   "                         <0 | 1> or <uncompress | compress>\r\n"
                   "  -it image_type       , firmware image layout type, default is 1M layout\r\n"
                   "                         <0 | 3> or <1M | 2M>\r\n"
                   "  -ua update_address   , upgrade storage location (hexadecimal)\r\n"
                   "                         the default is 90000\r\n"
                   "  -ra run_address      , runtime position (hexadecimal)\r\n"
                   "                         the default is 10100\r\n"
                   "  -df                  , generate debug firmware for openocd\r\n"
                   "\r\n"
                   "  -l                   , list the local serial port\r\n"
                   "  -c  serial_name      , connect a serial port\r\n"
#if defined(__APPLE__) && defined(__MACH__)
                   "                         e.g: tty.usbserial0 tty.usbserial3 tty.usbserial7\r\n"
#elif defined(__MINGW32__) || defined(__CYGWIN__)
                   "                         e.g: COM0 COM3 COM7\r\n"
#elif defined(__linux__)
                   "                         e.g: ttyUSB0 ttyUSB3 ttyUSB7\r\n"
#endif
                   "  -ws baud_rate        , set the serial port speed during normal work, default is 115200\r\n"
                   "                         <1200 - 2000000> or <1M | 2M>\r\n"
                   "  -ds baud_rate        , set the serial port speed when downloading, default is 115200\r\n"
                   "                         <115200 | 460800 | 921600 | 1000000 | 2000000> or <1M | 2M>\r\n"
                   "  -rs reset_action     , set device reset method, default is manual control\r\n"
                   "                         <none | at | rts>\r\n"
                   "                          none - manual control device reset\r\n"
                   "                          at   - use the at command to control the device reset\r\n"
                   "                          rts  - use the serial port rts pin to control the device reset\r\n"
                   "  -eo erase_option     , firmware area erase option\r\n"
                   "                         <secboot | all>\r\n"
                   "                          secboot - erase the second boot area\r\n"
                   "                          all     - erase all areas\r\n"
                   "  -dl download_firmware, firmware file to be downloaded, default download compressed image\r\n"
                   "  -sl display_format   , display the log information output from the serial port\r\n"
                   "                         <0 | 1> or <str | hex>\r\n"
                   "                          str - string mode display\r\n"
                   "                          hex - hexadecimal format\r\n"
                   , wm_tool_get_name(name));

    return;
}

static void wm_tool_print_version(const char *name)
{
    wm_tool_printf("%s %s\r\nCopyright (C) 2013 - 2019 WinnerMicro, Inc.\r\n", wm_tool_get_name(name), wm_tool_version);

    return;
}

static int wm_tool_parse_arv(int argc, char *argv[])
{
    int opt;
    int option_index = 0;
    char *opt_string = "hvlc:b:o:";
    int cnt = 0;

    opterr = 1;/* show err info */

    struct option long_options[] = {
       {"dl", required_argument, NULL, 'd'},
       {"ws", required_argument, NULL, 'w'},
       {"ds", required_argument, NULL, 's'},
       {"sb", required_argument, NULL, 'S'},
       {"it", required_argument, NULL, 'i'},
       {"fc", required_argument, NULL, 'C'},
       {"ua", required_argument, NULL, 'u'},
       {"ra", required_argument, NULL, 'r'},
       {"df", no_argument,       NULL, 'D'},
       {"rs", required_argument, NULL, 'a'},
       {"eo", required_argument, NULL, 'e'},
       {"sl", required_argument, NULL, 'g'},
       {0,    0,                 NULL, 0}
    };

    while ( (opt = getopt_long_only(argc, argv, opt_string, long_options, &option_index)) != -1)
    {
        WM_TOOL_DBG_PRINT("%c-%s\r\n", opt, optarg);

        switch (opt)
        {
            case '?':
            case 'h':
            {
                wm_tool_show_usage = 1;
                break;
            }
            case 'v':
            {
                wm_tool_show_ver = 1;
                break;
            }
            case 'l':
            {
                wm_tool_list_com = 1;
                break;
            }
            case 'c':
            {
#if defined(__MINGW32__)
                strcpy(wm_tool_serial_path, optarg);
#elif defined(__CYGWIN__)
                sprintf(wm_tool_serial_path, "/dev/ttyS%d", atoi(optarg + strlen("COM")) - 1);
#else
                sprintf(wm_tool_serial_path, "/dev/%s", optarg);
#endif
                break;
            }
            case 'w':
            {
                if ('M' == optarg[1])
                    wm_tool_normal_serial_rate = (optarg[0] - 0x30) * 1000000;
                else
                    wm_tool_normal_serial_rate = strtol(optarg, NULL, 10);
                break;
            }
            case 's':
            {
                if ('M' == optarg[1])
                    wm_tool_download_serial_rate = (optarg[0] - 0x30) * 1000000;
                else
                    wm_tool_download_serial_rate = strtol(optarg, NULL, 10);
                break;
            }
            case 'a':
            {
                if (0 == strncmp(optarg, "none", strlen("none")))
                    wm_tool_dl_action = WM_TOOL_DL_ACTION_NONE;
                else if (0 == strncmp(optarg, "at", strlen("at")))
                    wm_tool_dl_action = WM_TOOL_DL_ACTION_AT;
                else if (0 == strncmp(optarg, "rts", strlen("rts")))
                    wm_tool_dl_action = WM_TOOL_DL_ACTION_RTS;
                else
                    wm_tool_show_usage = 1;
                break;
            }
            case 'e':
            {
                if (0 == strncmp(optarg, "secboot", strlen("secboot")))
                    wm_tool_dl_erase = WM_TOOL_DL_ERASE_SECBOOT;
                else if (0 == strncmp(optarg, "all", strlen("all")))
                    wm_tool_dl_erase = WM_TOOL_DL_ERASE_ALL;
                else
                    wm_tool_show_usage = 1;
                break;
            }
            case 'd':
            {
                wm_tool_download_image = strdup(optarg);
                if (wm_tool_strcasestr(wm_tool_download_image, ".fls"))
                    wm_tool_dl_type = WM_TOOL_DL_TYPE_FLS;
                break;
            }
            case 'o':
            {
                wm_tool_output_image = strdup(optarg);
                break;
            }
            case 'b':
            {
                wm_tool_input_binary = strdup(optarg);
                break;
            }
            case 'S':
            {
                wm_tool_secboot_image = strdup(optarg);
                break;
            }
            case 'i':
            {
                if ('0' == optarg[0])
                    wm_tool_image_type = WM_TOOL_LAYOUT_TYPE_1M;
                else if ('3' == optarg[0])
                    wm_tool_image_type = WM_TOOL_LAYOUT_TYPE_2M;
                else if (0 == strncmp(optarg, "1M", strlen("1M")))
                    wm_tool_image_type = WM_TOOL_LAYOUT_TYPE_1M;
                else if (0 == strncmp(optarg, "2M", strlen("2M")))
                    wm_tool_image_type = WM_TOOL_LAYOUT_TYPE_2M;
                else
                {
                    if (isdigit((int)optarg[0]))
                        wm_tool_image_type = optarg[0] - 0x30;
                    else
                        wm_tool_show_usage = 1;
                }
                break;
            }
            case 'C':
            {
                if ('0' == optarg[0])
                    wm_tool_zip_type = WM_TOOL_ZIP_TYPE_UNCOMPRESS;
                else if ('1' == optarg[0])
                    wm_tool_zip_type = WM_TOOL_ZIP_TYPE_COMPRESS;
                else if (0 == strncmp(optarg, "compress", strlen("compress")))
                    wm_tool_zip_type = WM_TOOL_ZIP_TYPE_COMPRESS;
                else if (0 == strncmp(optarg, "uncompress", strlen("uncompress")))
                    wm_tool_zip_type = WM_TOOL_ZIP_TYPE_UNCOMPRESS;
                else
                    wm_tool_show_usage = 1;
                break;
            }
            case 'u':
            {
                wm_tool_upd_addr = strtol(optarg, NULL, 16);
                break;
            }
            case 'r':
            {
                wm_tool_run_addr = strtol(optarg, NULL, 16);
                break;
            }
            case 'D':
            {
                wm_tool_is_debug = 1;
                break;
            }
            case 'g':
            {
                if ('0' == optarg[0])
                    wm_tool_show_log_type = WM_TOOL_SHOW_LOG_STR;
                else if ('1' == optarg[0])
                    wm_tool_show_log_type = WM_TOOL_SHOW_LOG_HEX;
                else if (0 == strncmp(optarg, "str", strlen("str")))
                    wm_tool_show_log_type = WM_TOOL_SHOW_LOG_STR;
                else if (0 == strncmp(optarg, "hex", strlen("hex")))
                    wm_tool_show_log_type = WM_TOOL_SHOW_LOG_HEX;
                else
                    wm_tool_show_usage = 1;
                break;
                break;
            }
            default:
            {
                wm_tool_show_usage = 1;
                break;
            }
        }

        cnt++;
    }

    return cnt;
}

static int wm_tool_pack_image(const char *outfile)
{
    FILE *fpbin = NULL;
	FILE *fpimg = NULL;
	char *name = "2.0.0";
	int readlen = 0;
	int filelen = 0;
	int patch = 0;
	wm_tool_firmware_booter_t fbooter;
	unsigned char buf[WM_TOOL_ONCE_READ_LEN + 1];

    fpbin = fopen(wm_tool_input_binary, "rb");
	if (NULL == fpbin)
	{
		wm_tool_printf("can not open input file [%s].\r\n", wm_tool_input_binary);
		return -2;
	}

	fpimg = fopen(outfile, "wb+");
	if (NULL == fpimg)
	{
		wm_tool_printf("open img file error: [%s].\r\n", outfile);
		fclose(fpbin);
		return -3;
	}

    /* --------deal with upgrade image's CRC begin---- */
    wm_tool_file_crc = 0xFFFFFFFF;
	while (!feof(fpbin))
	{
		memset(buf, 0, sizeof(buf));
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpbin);
		if(readlen % 4 != 0)
		{
			patch = 4 - readlen%4;
			readlen += patch;
		}
		filelen += readlen;
		wm_tool_get_crc32((unsigned char*)buf, readlen, 0);
	}
	/* --------deal with upgrade image's CRC end---- */

	wm_tool_src_binary_len = filelen;
	wm_tool_src_binary_crc = wm_tool_file_crc;

    memset(&fbooter, 0, sizeof(wm_tool_firmware_booter_t));
    strcpy((char *)fbooter.ver, name);
	fbooter.magic_no = WM_TOOL_IMG_HEAD_MAGIC_NO;

    fbooter.run_org_checksum = wm_tool_file_crc;
	fbooter.img_type         = wm_tool_image_type;
	fbooter.run_img_len      = filelen;
	fbooter.run_img_addr     = wm_tool_run_addr;
	fbooter.zip_type         = WM_TOOL_ZIP_TYPE_UNCOMPRESS;
	fbooter.upd_img_len      = filelen;
	fbooter.upd_img_addr     = wm_tool_upd_addr;
	fbooter.upd_checksum     = wm_tool_file_crc;

    /* calculate image's header's CRC */
	wm_tool_file_crc = 0xFFFFFFFF;
	wm_tool_get_crc32((unsigned char *)&fbooter, sizeof(wm_tool_firmware_booter_t) - 4, 0);
    fbooter.hd_checksum = wm_tool_file_crc;

    /* write image's header to output file */
	fwrite(&fbooter, 1, sizeof(wm_tool_firmware_booter_t), fpimg);

	/* write image to output file */
	fseek(fpbin, 0, SEEK_SET);
	while (!feof(fpbin))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpbin);
		fwrite(buf, 1, readlen, fpimg);
	}

	/* write dummy data to pad 4byte-aligned */
	if (patch > 0)
	{
		memset(buf, 0, patch);
		fwrite(buf, 1, patch, fpimg);
	}

	if (fpbin)
	{
		fclose(fpbin);
	}

	if (fpimg)
	{
		fclose(fpimg);
	}

    wm_tool_printf("generate normal image completed.\r\n");

    return 0;
}

static int wm_tool_pack_gz_image(const char *gzbin, const char *outfile)
{
    FILE *fpbin = NULL;
	FILE *fpimg = NULL;
	char *name = "2.0.0";
	int readlen = 0;
	int filelen = 0;
	int patch = 0;
	wm_tool_firmware_booter_t fbooter;
	unsigned char buf[WM_TOOL_ONCE_READ_LEN + 1];

    fpbin = fopen(gzbin, "rb");
	if (NULL == fpbin)
	{
		wm_tool_printf("can not open input file [%s].\r\n", gzbin);
		return -2;
	}

	fpimg = fopen(outfile, "wb+");
	if (NULL == fpimg)
	{
		wm_tool_printf("open img file error: [%s].\r\n", outfile);
		fclose(fpbin);
		return -3;
	}

    /* --------deal with upgrade image's CRC begin---- */
    wm_tool_file_crc = 0xFFFFFFFF;
	while (!feof(fpbin))
	{
		memset(buf, 0, sizeof(buf));
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpbin);
		if(readlen % 4 != 0)
		{
			patch = 4 - readlen%4;
			readlen += patch;
		}
		filelen += readlen;
		wm_tool_get_crc32((unsigned char*)buf, readlen, 0);
	}
	/* --------deal with upgrade image's CRC end---- */

    memset(&fbooter, 0, sizeof(wm_tool_firmware_booter_t));
    strcpy((char *)fbooter.ver, name);
	fbooter.magic_no = WM_TOOL_IMG_HEAD_MAGIC_NO;

    fbooter.run_org_checksum = wm_tool_src_binary_crc;
	fbooter.img_type         = wm_tool_image_type;
	fbooter.run_img_len      = wm_tool_src_binary_len;
	fbooter.run_img_addr     = wm_tool_run_addr;
	fbooter.zip_type         = wm_tool_zip_type;
	fbooter.upd_img_len      = filelen;
	fbooter.upd_img_addr     = wm_tool_upd_addr;
	fbooter.upd_checksum     = wm_tool_file_crc;

    /* calculate image's header's CRC */
	wm_tool_file_crc = 0xFFFFFFFF;
	wm_tool_get_crc32((unsigned char *)&fbooter, sizeof(wm_tool_firmware_booter_t) - 4, 0);
    fbooter.hd_checksum = wm_tool_file_crc;

    /* write image's header to output file */
	fwrite(&fbooter, 1, sizeof(wm_tool_firmware_booter_t), fpimg);

	/* write image to output file */
	fseek(fpbin, 0, SEEK_SET);
	while (!feof(fpbin))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpbin);
		fwrite(buf, 1, readlen, fpimg);
	}

	/* write dummy data to pad 4byte-aligned */
	if (patch > 0)
	{
		memset(buf, 0, patch);
		fwrite(buf, 1, patch, fpimg);
	}

	if (fpbin)
	{
		fclose(fpbin);
	}

	if (fpimg)
	{
		fclose(fpimg);
	}

    wm_tool_printf("generate compressed image completed.\r\n");

    return 0;
}

static int wm_tool_pack_dbg_image(const char *image, const char *outfile)
{
    FILE *fpimg = NULL;
	FILE *fout = NULL;
	unsigned char buf[WM_TOOL_ONCE_READ_LEN + 1];
	int readlen = 0;
	int i;
	wm_tool_firmware_booter_t fbooter;
	int appimg_len = 0;
	int final_len = 0;
	int magic_word = 0;

	fpimg = fopen(image, "rb");
	if (NULL == fpimg)
	{
		wm_tool_printf("open img file error: [%s].\r\n", image);
		return -4;
	}

	magic_word = 0;
	readlen = fread(&magic_word, 1, 4, fpimg);
	if (magic_word != WM_TOOL_IMG_HEAD_MAGIC_NO)
	{
		wm_tool_printf("input [%s] file magic error.\n", image);
		fclose(fpimg);
		return -4;
	}

	fout = fopen(outfile, "wb+");

	appimg_len = wm_tool_get_file_size(image);

	/* write 0xFF to output file */
	final_len = WM_TOOL_RUN_IMG_HEADER_LEN + (appimg_len - sizeof(wm_tool_firmware_booter_t));
	for (i = 0; i < (final_len /WM_TOOL_ONCE_READ_LEN); i++)
	{
		memset(buf, 0xff, WM_TOOL_ONCE_READ_LEN);
		fwrite(buf, 1, WM_TOOL_ONCE_READ_LEN, fout);
	}
	memset(buf, 0xff, final_len % WM_TOOL_ONCE_READ_LEN);
	fwrite(buf, 1, final_len % WM_TOOL_ONCE_READ_LEN, fout);

	memset(&fbooter, 0, sizeof(wm_tool_firmware_booter_t));

	/* write sec img to output file */
	fseek(fpimg, 0, SEEK_SET);
	readlen = fread((unsigned char *)&fbooter, 1, sizeof(wm_tool_firmware_booter_t), fpimg);

	fseek(fout, 0, SEEK_SET);
	fwrite(&fbooter, 1, sizeof(wm_tool_firmware_booter_t), fout);
	fseek(fout, WM_TOOL_RUN_IMG_HEADER_LEN, SEEK_SET);

	while (!feof(fpimg))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpimg);
		fwrite(buf, 1, readlen, fout);
	}

	if (fpimg)
	{
		fclose(fpimg);
	}

	if (fout)
	{
		fclose(fout);
	}

    wm_tool_printf("generate debug image completed.\r\n");

    return 0;
}

static int wm_tool_pack_fls(const char *image, const char *outfile)
{
	FILE *fpbin = NULL;
	FILE *fpimg = NULL;
	FILE *fout = NULL;
	unsigned char buf[WM_TOOL_ONCE_READ_LEN + 1];
	int readlen = 0;
	int filelen = 0;
	int i;
	wm_tool_firmware_booter_t fbooter;
	int appimg_len = 0;
	int final_len = 0;
	int magic_word = 0;

	if (!wm_tool_secboot_image)
        return 0;/* maybe return 1 */

	fpbin = fopen(wm_tool_secboot_image,"rb");
	if (NULL == fpbin)
	{
		wm_tool_printf("can not open input file [%s].\r\n", wm_tool_secboot_image);
		return -2;
	}

	magic_word = 0;
	readlen = fread(&magic_word, 1, 4, fpbin);
	if (magic_word != WM_TOOL_IMG_HEAD_MAGIC_NO)
	{
		wm_tool_printf("input [%s] file magic error.\r\n", wm_tool_secboot_image);
		return -3;
	}

	fpimg = fopen(image, "rb");
	if (NULL == fpimg)
	{
		wm_tool_printf("open img file error [%s].\r\n", image);
		fclose(fpbin);
		return -4;
	}

	magic_word = 0;
	readlen = fread(&magic_word, 1, 4, fpimg);
	if (magic_word != WM_TOOL_IMG_HEAD_MAGIC_NO)
	{
		wm_tool_printf("input [%s] file magic error.\r\n", image);
		return -4;
	}

	fout = fopen(outfile, "wb+");

	appimg_len = wm_tool_get_file_size(image);

	/* write 0xFF to output file */
	final_len = WM_TOOL_SECBOOT_IMG_AREA_TOTAL_LEN + appimg_len;
	for (i = 0; i < (final_len /WM_TOOL_ONCE_READ_LEN); i++)
	{
		memset(buf, 0xff, WM_TOOL_ONCE_READ_LEN);
		fwrite(buf, 1, WM_TOOL_ONCE_READ_LEN, fout);
	}
	memset(buf, 0xff, final_len % WM_TOOL_ONCE_READ_LEN);
	fwrite(buf, 1, final_len % WM_TOOL_ONCE_READ_LEN, fout);

	fseek(fout, sizeof(wm_tool_firmware_booter_t), SEEK_SET);
	fseek(fpbin, 0, SEEK_SET);
	/* write SECBOOT header to output file */
	readlen = fread(buf, 1, sizeof(wm_tool_firmware_booter_t), fpbin);
	fwrite(buf, 1, sizeof(wm_tool_firmware_booter_t), fout);

	/* write SECBOOT image to output file */
	fseek(fout, sizeof(wm_tool_firmware_booter_t) + 256, SEEK_SET);
	while (!feof(fpbin))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpbin);
		fwrite(buf, 1, readlen, fout);
	}

	/* write run img header to output file */
	fseek(fout, sizeof(wm_tool_firmware_booter_t) + WM_TOOL_SECBOOT_IMG_AREA_TOTAL_LEN, SEEK_SET);
	fseek(fpimg, 0, SEEK_SET);
	readlen = fread(buf, 1, sizeof(wm_tool_firmware_booter_t), fpimg);
	fwrite(buf, 1, readlen, fout);

	fseek(fout, sizeof(wm_tool_firmware_booter_t) + WM_TOOL_SECBOOT_IMG_AREA_TOTAL_LEN + WM_TOOL_RUN_IMG_HEADER_LEN, SEEK_SET);
	/* write run img to output file */
	fseek(fpimg, sizeof(wm_tool_firmware_booter_t), SEEK_SET);
	while (!feof(fpimg))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fpimg);
		fwrite(buf, 1, readlen, fout);
	}

	/* calculate CRC from secboot header */
	wm_tool_file_crc = 0xFFFFFFFF;
	fseek(fout, sizeof(wm_tool_firmware_booter_t), SEEK_SET);
	filelen = 0;
	while (!feof(fout))
	{
		readlen = fread(buf, 1, WM_TOOL_ONCE_READ_LEN, fout);
		filelen += readlen;
		wm_tool_get_crc32((unsigned char*)buf, readlen, 0);
	}

	memset(&fbooter, 0, sizeof(wm_tool_firmware_booter_t));

	fbooter.magic_no = WM_TOOL_IMG_HEAD_MAGIC_NO;
	fbooter.run_org_checksum = wm_tool_file_crc;
	fbooter.img_type = 0;
	fbooter.run_img_len = filelen;
	fbooter.run_img_addr = WM_TOOL_SECBOOT_HEADER_POS;
	fbooter.zip_type = 0;
	fbooter.upd_img_len = 0;
	fbooter.upd_img_addr = 0;
	fbooter.upd_checksum = 0;

	/* write fls's header to output file */
	wm_tool_file_crc = 0xFFFFFFFF;
	wm_tool_get_crc32((unsigned char *)&fbooter, sizeof(wm_tool_firmware_booter_t) - 4, 0);
  	fbooter.hd_checksum = wm_tool_file_crc;
	fseek(fout, 0, SEEK_SET);
	fwrite(&fbooter, 1, sizeof(wm_tool_firmware_booter_t), fout);

	if (fpbin)
	{
		fclose(fpbin);
	}

	if (fpimg)
	{
		fclose(fpimg);
	}

	if (fout)
	{
		fclose(fout);
	}

    wm_tool_printf("generate flash file completed.\r\n");

	return 0;
}

static int wm_tool_gzip_bin(const char *binary, const char *gzbin)
{
    FILE *bfp;
    gzFile gzfp;
    int ret;
    char buf[1024];

    bfp = fopen(binary, "rb");
    gzfp = gzopen((char *)gzbin, "wb+");

    if (!bfp || !gzfp)
    {
        wm_tool_printf("can not gzip binary.\r\n");
        return -1;
    }

    do {
        ret = fread(buf, 1, sizeof(buf), bfp);
        if (ret > 0)
        {
            ret = gzwrite(gzfp, (voidp)buf, ret);
            if (ret <= 0)
            {
                wm_tool_printf("can not write gzip binary.\r\n");
                return -2;
            }
        }
    } while (ret > 0);

    gzclose(gzfp);
    fclose(bfp);

    wm_tool_printf("compress binary completed.\r\n");

    return 0;
}

static int wm_tool_pack_firmware(void)
{
    int ret;
    char *path;
    char *name;
    char *image;
    char *gzbin;
    char *gzimg;
    char *dbgimg;
    char *fls;

    if (!wm_tool_input_binary)
        return 1;

    if (!wm_tool_output_image)
    {
        char *r = wm_tool_input_binary;
        char *t = wm_tool_input_binary;
        do
        {
            r = strchr(r, '.');
            if (r)
            {
                t = r;
                r++;
            }
        } while (r);
        wm_tool_output_image = malloc(t - wm_tool_input_binary + 1);
        if (!wm_tool_output_image)
            return -1;
        memcpy(wm_tool_output_image, wm_tool_input_binary, t - wm_tool_input_binary);
        wm_tool_output_image[t - wm_tool_input_binary] = '\0';
    }

    char *p = wm_tool_output_image;
    char *q = wm_tool_output_image;

    do
    {
        p = strchr(p, '/');
        if (p)
        {
            p++;
            q = p;
        }
    } while (p);

    /* output file */
    name = strdup(q);
    *q = '\0';
    path = strdup(wm_tool_output_image);

    image = malloc(strlen(path) + strlen(name) + strlen(".img") + 1);
    if (!image)
        return -1;
    sprintf(image, "%s%s.img", path, name);

    ret = wm_tool_pack_image(image);
    if (ret)
        return ret;

    if (WM_TOOL_ZIP_TYPE_COMPRESS == wm_tool_zip_type)
    {
        gzbin = malloc(strlen(path) + strlen(name) + strlen("_gz.bin") + 1);
        gzimg = malloc(strlen(path) + strlen(name) + strlen("_gz.img") + 1);
        if (!gzbin || !gzimg)
            return -1;
        sprintf(gzbin, "%s%s.bin.gz", path, name);
        sprintf(gzimg, "%s%s_gz.img", path, name);

        ret = wm_tool_gzip_bin(wm_tool_input_binary, gzbin);
        if (ret)
            return ret;

        ret = wm_tool_pack_gz_image(gzbin, gzimg);
        free(gzbin);
        free(gzimg);
        if (ret)
            return ret;
    }

    if (wm_tool_is_debug)
    {
        dbgimg = malloc(strlen(path) + strlen(name) + strlen("_dbg.img") + 1);
        if (!dbgimg)
            return -1;
        sprintf(dbgimg, "%s%s_dbg.img", path, name);

        ret = wm_tool_pack_dbg_image(image, dbgimg);
        free(dbgimg);
        if (ret)
            return ret;
    }

    fls = malloc(strlen(path) + strlen(name) + strlen(".fls") + 1);
    if (!fls)
        return -1;
    sprintf(fls, "%s%s.fls", path, name);

    ret = wm_tool_pack_fls(image, fls);

    free(image);
    free(fls);

    return ret;
}

#ifdef __MINGW32__
static void wm_tool_delay_ms(int ms)
{
    Sleep(ms);

    return;
}

static int wm_tool_uart_set_rts(int boolflag)
{
    return EscapeCommFunction(wm_tool_uart_handle,((boolflag) ? SETRTS : CLRRTS)) ? 0 : -1;
}

static int wm_tool_uart_set_dtr(int boolflag)
{
    return EscapeCommFunction(wm_tool_uart_handle,((boolflag) ? SETDTR : CLRDTR)) ? 0 : -1;
}

static int wm_tool_uart_set_timeout(void)
{
    BOOL ret;
    COMMTIMEOUTS timeout;

	timeout.ReadIntervalTimeout         = MAXDWORD;
	timeout.ReadTotalTimeoutConstant    = 0;
	timeout.ReadTotalTimeoutMultiplier  = 0;
	timeout.WriteTotalTimeoutConstant   = 0;
	timeout.WriteTotalTimeoutMultiplier = 0;

	ret = SetCommTimeouts(wm_tool_uart_handle, &timeout);
	if (ret)
	    ret = SetCommMask(wm_tool_uart_handle, EV_TXEMPTY);

    return ret ? 0 : -1;
}

static void wm_tool_uart_set_block(int bolck)
{
    if (bolck)
        wm_tool_uart_block = INFINITE;
    else
        wm_tool_uart_block = 0;

    return;
}

static int wm_tool_uart_set_speed(int speed)
{
    int i;
    int size;
	DCB cfg;

	size = sizeof(wm_tool_uart_speed_array) / sizeof(int);

    for (i= 0; i < size; i++)
    {
        if (speed == wm_tool_uart_name_array[i])
        {
            if (GetCommState(wm_tool_uart_handle, &cfg))
            {
            	cfg.DCBlength		= sizeof(DCB);
            	cfg.BaudRate		= wm_tool_uart_speed_array[i];
            	cfg.fBinary		    = TRUE;
            	cfg.fParity         = FALSE;
            	cfg.fDtrControl	    = DTR_CONTROL_DISABLE;
            	cfg.fDsrSensitivity = FALSE;
            	cfg.fRtsControl	    = RTS_CONTROL_DISABLE;
            	cfg.ByteSize		= 8;
            	cfg.StopBits 		= ONESTOPBIT;
            	cfg.fAbortOnError	= FALSE;
            	cfg.fOutX			= FALSE;
            	cfg.fInX			= FALSE;
            	cfg.fErrorChar      = FALSE;
            	cfg.fNull           = FALSE;
            	cfg.fOutxCtsFlow    = FALSE;
	            cfg.fOutxDsrFlow    = FALSE;
	            cfg.Parity          = NOPARITY;
	            cfg.fTXContinueOnXoff = FALSE;

            	return SetCommState(wm_tool_uart_handle, &cfg) ? 0 : -1;
        	}
        	else
        	{
                return -3;
        	}
        }
    }

	return -2;
}

static void wm_tool_uart_clear(void)
{
    PurgeComm(wm_tool_uart_handle, PURGE_RXCLEAR);

    return;
}

static int wm_tool_uart_open(const char *device)
{
    BOOL ret;
    char name[40];

	sprintf(name,"\\\\.\\%s", device);

	wm_tool_uart_handle = CreateFile(name, GENERIC_WRITE | GENERIC_READ,
                                     0, NULL, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                     NULL);

	if (wm_tool_uart_handle == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

    ret = SetupComm(wm_tool_uart_handle, 4096, 4096);
    if (ret)
    {
        ret  = wm_tool_uart_set_speed(WM_TOOL_DEFAULT_BAUD_RATE);
        ret |= wm_tool_uart_set_timeout();
    }
    else
    {
        ret = -1;
    }

    return ret;
}

static int wm_tool_uart_read(void *data, unsigned int size)
{
    BOOL ret;
    DWORD wait;
	unsigned long len = 0;
	COMSTAT state;
	DWORD error;
    OVERLAPPED event;

    do
    {
    	memset(&event, 0, sizeof(OVERLAPPED));

    	event.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    	ClearCommError(wm_tool_uart_handle, &error, &state);

        ret = ReadFile(wm_tool_uart_handle, data, size, &len, &event);
        if (!ret)
        {
            if (ERROR_IO_PENDING == GetLastError())
    		{
    			wait = WaitForSingleObject(event.hEvent, wm_tool_uart_block);
    			if (WAIT_OBJECT_0 == wait)
    			    ret = TRUE;
    		}
        }
    } while (wm_tool_uart_block && !len);

	if (ret)
	{
        if (len > 0)
            return (int)len;
	}

	return -1;
}

static int wm_tool_uart_write(const void *data, unsigned int size)
{
    BOOL ret;
    DWORD wait;
    OVERLAPPED event;
    COMSTAT state;
	DWORD error;
    unsigned int snd = 0;
	unsigned long len = 0;

    memset(&event, 0, sizeof(OVERLAPPED));

    event.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    do
    {
        ClearCommError(wm_tool_uart_handle, &error, &state);

        ret = WriteFile(wm_tool_uart_handle, data + snd, size - snd, &len, &event);
        if (!ret)
        {
            if (ERROR_IO_PENDING == GetLastError())
    		{
    			wait = WaitForSingleObject(event.hEvent, INFINITE);
    			if (WAIT_OBJECT_0 == wait)
			        ret = TRUE;
    		}
        }

        if (ret)
    	{
            if ((len > 0) && (snd != size))
            {
                snd += len;
            }
            else
            {
    	        return size;
    	    }
    	}
    	else
    	{
            break;
    	}
    } while (1);

	return -1;
}

static void wm_tool_uart_close(void)
{
	FlushFileBuffers(wm_tool_uart_handle);
	CloseHandle(wm_tool_uart_handle);
	wm_tool_uart_handle = NULL;

	return;
}

static DWORD WINAPI wm_tool_uart_tx_thread(LPVOID arg)
{
    wm_tool_stdin_to_uart();

    return 0;
}

static int wm_tool_create_thread(void)
{
    HANDLE thread_handle;
    thread_handle = CreateThread(0, 0, wm_tool_uart_tx_thread, NULL, 0, NULL);
    CloseHandle(thread_handle);
    return 0;
}

#else

static void wm_tool_delay_ms(int ms)
{
    usleep(ms * 1000);

    return;
}

static int wm_tool_uart_set_rts(int boolflag)
{
    int ret;
    int controlbits;

    ret = ioctl(wm_tool_uart_fd, TIOCMGET, &controlbits);
    if (ret < 0)
    {
        WM_TOOL_DBG_PRINT("TIOCMGET failed!\r\n");
        return ret;
    }

    if (controlbits & TIOCM_RTS)
    {
        WM_TOOL_DBG_PRINT("TIOCM_RTS!, %d!\r\n", boolflag);
    }
    else
    {
        WM_TOOL_DBG_PRINT("~TIOCM_RTS!, %d!\r\n", boolflag);
    }

    if (boolflag)
        controlbits |= TIOCM_RTS;
    else
        controlbits &= ~TIOCM_RTS;

    ret = ioctl(wm_tool_uart_fd, TIOCMSET, &controlbits);
    if (ret < 0)
    {
        WM_TOOL_DBG_PRINT("TIOCMSET failed!\r\n");
    }

    if (ret >= 0)
        ret = 0;

    return ret;
}

static int wm_tool_uart_set_dtr(int boolflag)
{
    int ret;
    int controlbits;

    ret = ioctl(wm_tool_uart_fd, TIOCMGET, &controlbits);
    if (ret < 0)
    {
        WM_TOOL_DBG_PRINT("TIOCMGET failed!\r\n");
        return ret;
    }

    if (controlbits & TIOCM_DTR)
    {
        WM_TOOL_DBG_PRINT("TIOCM_DTR, %d!\r\n", boolflag);
    }
    else
    {
        WM_TOOL_DBG_PRINT("~TIOCM_DTR, %d!\r\n", boolflag);
    }

    if (boolflag)
        controlbits |= TIOCM_DTR;
    else
        controlbits &= ~TIOCM_DTR;

    ret = ioctl(wm_tool_uart_fd, TIOCMSET, &controlbits);
    if (ret < 0)
    {
        WM_TOOL_DBG_PRINT("TIOCMSET failed!\r\n");
    }

    if (ret >= 0)
        ret = 0;

    return ret;
}

static void wm_tool_uart_set_block(int bolck)
{
    int comopt;

    comopt = fcntl(wm_tool_uart_fd, F_GETFL, 0);

    if (bolck)
        fcntl(wm_tool_uart_fd, F_SETFL, comopt & ~O_NDELAY);
    else
        fcntl(wm_tool_uart_fd, F_SETFL, comopt | O_NDELAY);

    return;
}

static int wm_tool_uart_set_speed(int speed)
{

    int i;
    int size;
    int status;
    struct termios opt;

    tcgetattr(wm_tool_uart_fd, &opt);

    size = sizeof(wm_tool_uart_speed_array) / sizeof(int);

    for (i= 0; i < size; i++)
    {
        if (speed == wm_tool_uart_name_array[i])
        {
            cfsetispeed(&opt, wm_tool_uart_speed_array[i]);
            cfsetospeed(&opt, wm_tool_uart_speed_array[i]);

            status = tcsetattr(wm_tool_uart_fd, TCSANOW, &opt);

            if (status != 0)
            {
                return -1;
            }
        }
    }

    return 0;
}

static void wm_tool_uart_clear(void)
{
    tcflush(wm_tool_uart_fd, TCIFLUSH);

    return;
}

static int wm_tool_uart_open(const char *device)
{
    struct termios tty;

    wm_tool_uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == wm_tool_uart_fd)
    {
		return -1;
    }

    tcgetattr(wm_tool_uart_fd, &wm_tool_saved_serial_cfg);

    wm_tool_uart_set_dtr(0);

    tcgetattr(wm_tool_uart_fd, &tty);

    /* databit 8bit */
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

    /* set into raw, no echo mode */
    tty.c_iflag =  IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    /* have no software flow control */
    tty.c_iflag &= ~(IXON|IXOFF|IXANY);

    /* parity none */
    tty.c_cflag &= ~(PARENB | PARODD);

    /* stopbit 1bit */
    tty.c_cflag &= ~CSTOPB;

    tcsetattr(wm_tool_uart_fd, TCSANOW, &tty);

    return wm_tool_uart_set_speed(WM_TOOL_DEFAULT_BAUD_RATE);
}

static int wm_tool_uart_read(void *data, unsigned int size)
{
	return read(wm_tool_uart_fd, data, size);
}

static int wm_tool_uart_write(const void *data, unsigned int size)
{
    int snd = 0;
    int ret = 0;

    do
    {
        ret = write(wm_tool_uart_fd, data + snd, size - snd);
        if (ret > 0)
        {
            snd += ret;

            if (snd == size)
            {
                return size;
            }
        }
        else
        {
            break;
        }
    } while (1);

	return -1;
}

static void wm_tool_uart_close(void)
{
    tcsetattr(wm_tool_uart_fd, TCSANOW, &wm_tool_saved_serial_cfg);

    tcdrain(wm_tool_uart_fd);
    tcflush(wm_tool_uart_fd, TCIOFLUSH);

	close(wm_tool_uart_fd);
    wm_tool_uart_fd = -1;

	return;
}

static void *wm_tool_uart_tx_thread(void *arg)
{
    wm_tool_stdin_to_uart();

    return NULL;
}

static int wm_tool_create_thread(void)
{
    pthread_t thread_id;
    return pthread_create(&thread_id, NULL, wm_tool_uart_tx_thread, NULL);
}
#endif

static void wm_tool_stdin_to_uart(void)
{
    int ret;
    char buf[WM_TOOL_ONCE_READ_LEN];

    while (1)
    {
        if (fgets(buf, WM_TOOL_ONCE_READ_LEN, stdin))
        {
            ret = wm_tool_uart_write(buf, strlen(buf));
            if (ret <= 0)
            {
                WM_TOOL_DBG_PRINT("failed to write [%s].\r\n", buf);
            }
        }
    }

    return;
}

static int wm_tool_show_log_from_serial(void)
{
    int i;
    int ret;
    unsigned int j = 0;
    unsigned char buf[WM_TOOL_ONCE_READ_LEN + 1];

    ret = wm_tool_uart_open(wm_tool_serial_path);
    if (ret)
    {
        wm_tool_printf("can not open serial\r\n");
        return ret;
    }

    if (WM_TOOL_DEFAULT_BAUD_RATE != wm_tool_normal_serial_rate)
    {
        ret = wm_tool_uart_set_speed(wm_tool_normal_serial_rate);
        if (ret)
        {
            wm_tool_printf("can not set serial baud rate.\r\n");
            wm_tool_uart_close();
            return ret;
        }
    }

    ret = wm_tool_create_thread();
    if (ret)
    {
        wm_tool_printf("can not create thread.\r\n");
        wm_tool_uart_close();
        return ret;
    }

    wm_tool_uart_set_block(0);

    while (1)
    {
        ret = wm_tool_uart_read(buf, WM_TOOL_ONCE_READ_LEN);
        if (ret > 0)
        {
            if (WM_TOOL_SHOW_LOG_STR == wm_tool_show_log_type)
            {
                buf[ret] = '\0';
                wm_tool_printf("%s", (char *)buf);
            }
            else if (WM_TOOL_SHOW_LOG_HEX == wm_tool_show_log_type)
            {
                for (i = 0; i < ret; i++, j++)
                {
                    wm_tool_printf("%02X ", buf[i]);
                    if ((j + 1) % 16 == 0)
                    {
                        wm_tool_printf("\r\n");
                    }
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            wm_tool_delay_ms(3);
        }
    }

    wm_tool_uart_close();

    return ret;
}

static int wm_tool_set_wifi_chip_speed(int speed)
{
	int ret;

	if (2000000 == speed)
	{
		ret = wm_tool_uart_write(wm_tool_chip_cmd_b2000000, sizeof(wm_tool_chip_cmd_b2000000));
	}
	else if (1000000 == speed)
	{
		ret = wm_tool_uart_write(wm_tool_chip_cmd_b1000000, sizeof(wm_tool_chip_cmd_b1000000));
	}
	else if (921600 == speed)
	{
		ret = wm_tool_uart_write(wm_tool_chip_cmd_b921600, sizeof(wm_tool_chip_cmd_b921600));
	}
	else if (460800 == speed)
	{
		ret = wm_tool_uart_write(wm_tool_chip_cmd_b460800, sizeof(wm_tool_chip_cmd_b460800));
	}
	else
	{
		ret = wm_tool_uart_write(wm_tool_chip_cmd_b115200, sizeof(wm_tool_chip_cmd_b115200));
	}

	return ret;
}

static int wm_tool_send_esc2uart(int ms)
{
    int i;
	int err = 0;
    unsigned char esc_key = 27;

    for (i = 0; i < (ms / 10); i++)
	{
        err = wm_tool_uart_write(&esc_key, 1);
		wm_tool_delay_ms(10);/* 10-50ms */
	}

    return err;
}

static int wm_tool_erase_image(wm_tool_dl_erase_e type)
{
    int cnt = 0;
    int ret = -1;
    unsigned char ch;
    unsigned char sec_cmd[] = {0x21, 0x06, 0x00, 0xc7, 0x7c, 0x3f, 0x00, 0x00, 0x00};
    unsigned char rom_cmd[] = {0x21, 0x06, 0x00, 0x41, 0x45, 0x32, 0x00, 0x00, 0x00};

    WM_TOOL_DBG_PRINT("start erase.\r\n");

    if (WM_TOOL_DL_ERASE_SECBOOT == type)
    {
        wm_tool_printf("erase second boot area...\r\n");
        ret = wm_tool_uart_write(sec_cmd, sizeof(sec_cmd));
    }
    else if (WM_TOOL_DL_ERASE_ALL == type)
    {
        wm_tool_printf("erase image area...\r\n");
        ret = wm_tool_uart_write(rom_cmd, sizeof(rom_cmd));
    }
    if (ret <= 0)
        return -1;

    wm_tool_uart_clear();

    wm_tool_uart_set_block(1);

    do
	{
		ret = wm_tool_uart_read(&ch, 1);
		if (ret > 0)
	    {
		    if (('C' == ch) || ('P' == ch))
		        cnt++;
		    else
		        cnt = 0;
		}
		else
		{
		    wm_tool_printf("erase error, errno = %d.\r\n", errno);
            return -2;
		}
	} while (cnt < 3);

    wm_tool_uart_set_block(0);

    wm_tool_printf("erase finish.\r\n");

    return 0;
}

static int wm_tool_query_mac(void)
{
    int ret = -1;
    int err;
    int offset = 0;
    char macstr[32] = {0};
    int len = strlen("MAC:AABBCCDDEEFF\n");/* resp format, ROM "Mac:AABBCCDDEEFF\n", SECBOOT "MAC:AABBCCDDEEFF\n" */
    unsigned char macaddr[6] = {0};

    wm_tool_uart_clear();

    wm_tool_uart_set_block(1);

	err = wm_tool_uart_write(wm_tool_chip_cmd_get_mac, sizeof(wm_tool_chip_cmd_get_mac));
    if (err > 0)
    {
        do
        {
            err = wm_tool_uart_read((unsigned char *)(macstr + offset), sizeof(macstr) - 1 - offset);
            if (err > 0)
            {
                offset += err;
                if (offset >= len)
                {
                    macstr[len - 1] = '\0';/* \n -> 0 */
                    err = wm_tool_str_to_hex_array(macstr + strlen("MAC:"), 6, macaddr);
                    if (!err)
                    {
                        wm_tool_printf("mac %02X-%02X-%02X-%02X-%02X-%02X.\r\n", macaddr[0],
                                       macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

                        if (!strncmp(macstr, "Mac:", strlen("Mac:")) && 
                            (WM_TOOL_DL_TYPE_FLS != wm_tool_dl_type))
                        {
                            wm_tool_printf("please download the firmware in .fls format.\r\n");
                        }
                        else
                        {
                            ret = 0;
                        }
                    }
                    break;
                }
            }
        } while (err > 0);
    }

    wm_tool_uart_set_block(0);

    return ret;
}

static int wm_tool_xmodem_download(const char *image)
{
    FILE *imgfp;
	unsigned char packet_data[XMODEM_DATA_SIZE];
	unsigned char frame_data[XMODEM_DATA_SIZE + XMODEM_CRC_SIZE + XMODEM_FRAME_ID_SIZE + 1];
	int complete, retry_num, pack_counter, read_number, write_number = 0, i;
	unsigned short crc_value;
	unsigned char ack_id;
	int sndlen;
	int ret = -111;

	imgfp = fopen(image, "rb");
    if (!imgfp)
    {
        wm_tool_printf("can not open image to download.\r\n");
		return -1;
    }

	sndlen = 0;
	pack_counter = 0;
	complete = 0;
	retry_num = 0;

	wm_tool_printf("start download.\r\n");
	wm_tool_printf("0%% [");

    wm_tool_uart_clear();

    wm_tool_uart_set_block(1);

	ack_id = XMODEM_ACK;

	while (!complete)
	{
	    WM_TOOL_DBG_PRINT("switch ack_id = %x\r\n", ack_id);

        switch(ack_id)
        {
        	case XMODEM_ACK:
        	{
	            retry_num = 0;
	            pack_counter++;
	            read_number = fread(packet_data, sizeof(char), XMODEM_DATA_SIZE, imgfp);

	            WM_TOOL_DBG_PRINT("fread = %d\r\n", read_number);

	            if (read_number > 0)
	            {
					sndlen += read_number;

					if (read_number < XMODEM_DATA_SIZE)
	                {
	                    WM_TOOL_DBG_PRINT("start filling the last frame!\r\n");
	                    for ( ; read_number < XMODEM_DATA_SIZE; read_number++)
	                        packet_data[read_number] = 0x0;
	                }

	                frame_data[0] = XMODEM_HEAD;
	                frame_data[1] = (char)pack_counter;
	                frame_data[2] = (char)(255 - frame_data[1]);

	                for (i = 0; i < XMODEM_DATA_SIZE; i++)
	                    frame_data[i + 3] = packet_data[i];

	                crc_value = wm_tool_get_crc16(packet_data, XMODEM_DATA_SIZE);

					frame_data[XMODEM_DATA_SIZE + 3]=(unsigned char)(crc_value >> 8);
	                frame_data[XMODEM_DATA_SIZE + 4]=(unsigned char)(crc_value);

	                write_number = wm_tool_uart_write(frame_data, XMODEM_DATA_SIZE + 5);
	                if (write_number <= 0)
	                    wm_tool_printf("write serial error, errno = %d.\r\n", errno);

	                WM_TOOL_DBG_PRINT("waiting for ack, %d, %d ...\r\n", pack_counter, write_number);

	                while ((wm_tool_uart_read(&ack_id, 1)) <= 0);

	                if (ack_id == XMODEM_ACK)
	                {
	                	WM_TOOL_DBG_PRINT("Ok!\r\n");

	                	if (sndlen % 10240 == 0)
                        {
    						wm_tool_printf("#");
						}
	                }
					else
					{
                    	WM_TOOL_DBG_PRINT("error = %x!\r\n", ack_id);
					}
	            }
	            else
	            {
	                ack_id = XMODEM_EOT;
	                complete = 1;

	                WM_TOOL_DBG_PRINT("waiting for complete ack ...\r\n");

	                while (ack_id != XMODEM_ACK)
	                {
	                    ack_id = XMODEM_EOT;

	                    write_number = wm_tool_uart_write(&ack_id, 1);
	                    if (write_number <= 0)
	                        wm_tool_printf("write serial error, errno = %d.\r\n", errno);

	                    while ((wm_tool_uart_read(&ack_id, 1)) <= 0);
	                }

	                WM_TOOL_DBG_PRINT("ok\r\n");

	                wm_tool_printf("] 100%%\r\n");

	                wm_tool_printf("download completed.\r\n");

	                ret = 0;
	            }
	            break;
            }
            case XMODEM_NAK:
            {
                if ( retry_num++ > 100)
                {
                    WM_TOOL_DBG_PRINT("retry too many times, quit!\r\n");
                    wm_tool_printf("download firmware timeout.\r\n");

                    complete = 1;
                }
                else
                {
                    write_number = wm_tool_uart_write(frame_data, XMODEM_DATA_SIZE + 5);
                    if (write_number <= 0)
	                    wm_tool_printf("write serial error, errno = %d.\r\n", errno);

                    WM_TOOL_DBG_PRINT("retry for ack, %d, %d ...\r\n", pack_counter, write_number);

                    while ((wm_tool_uart_read(&ack_id, 1)) <= 0);

                    if (ack_id == XMODEM_ACK)
                    {
                    	WM_TOOL_DBG_PRINT("ok\r\n");
                    }
    				else
    				{
                    	WM_TOOL_DBG_PRINT("error!\r\n");
    				}
                }
                break;
            }
            default:
            {
                WM_TOOL_DBG_PRINT("fatal error!\r\n");
                WM_TOOL_DBG_PRINT("unknown xmodem protocal [%x].\r\n", ack_id);
                wm_tool_printf("\r\ndownload failed, please reset and try again.\r\n");

                complete = 1;
                break;
            }
        }
    }

    wm_tool_uart_set_block(0);

    fclose(imgfp);

    return ret;
}
static int wm_tool_download_firmware(void)
{
    int ret;
    int cnt = 0;
    int note = 1;
    int timeout = 0;
    float timeuse;
    time_t start, end;
    unsigned char ch;

    wm_tool_printf("connecting serial...\r\n");

    ret = wm_tool_uart_open(wm_tool_serial_path);
    if (ret)
    {
        wm_tool_printf("can not open serial\r\n");
        return ret;
    }

    /* In some cases, setting the serial port initialization setting requires a delay. */
    wm_tool_delay_ms(500);

    wm_tool_printf("serial connected.\r\n");

    if (WM_TOOL_DL_ACTION_AT == wm_tool_dl_action)
    {
        if (WM_TOOL_DEFAULT_BAUD_RATE != wm_tool_normal_serial_rate)
            wm_tool_uart_set_speed(wm_tool_normal_serial_rate);

#if 0 /* use erase option */
        if (WM_TOOL_DL_TYPE_FLS == wm_tool_dl_type)
        {
            ret = wm_tool_uart_write("AT+&FLSW=8002000,0\r\n", strlen("AT+&FLSW=8002000,0\r\n"));
            if (ret <= 0)
            {
                wm_tool_printf("destroy secboot failed.\r\n");
                wm_tool_uart_close();
                return -3;
            }
            wm_tool_delay_ms(300);
        }
#endif

        ret = wm_tool_uart_write("AT+Z\r\n", strlen("AT+Z\r\n"));
        if (ret <= 0)
        {
            wm_tool_printf("reset error.\r\n");
            wm_tool_uart_close();
            return -4;
        }

        if (WM_TOOL_DEFAULT_BAUD_RATE != wm_tool_normal_serial_rate)
            wm_tool_uart_set_speed(WM_TOOL_DEFAULT_BAUD_RATE);
    }
    else if (WM_TOOL_DL_ACTION_RTS == wm_tool_dl_action)
    {
        ret  = wm_tool_uart_set_dtr(0);
        ret |= wm_tool_uart_set_rts(1);
        wm_tool_delay_ms(50);
        ret |= wm_tool_uart_set_dtr(1);
        ret |= wm_tool_uart_set_rts(0);
        wm_tool_delay_ms(50);
		ret |= wm_tool_uart_set_dtr(0);
		if (ret < 0)
        {
            wm_tool_printf("set rts to reboot error.\r\n");
            wm_tool_uart_close();
            return -5;
        }
    }

    wm_tool_printf("wait serial sync...");

    wm_tool_send_esc2uart(500);/* used for delay */

    start = time(NULL);

    do
	{
		ret = wm_tool_uart_read(&ch, 1);

		WM_TOOL_DBG_PRINT("ret=%d, %x-%c\r\n", ret, ch, ch);

		if (ret > 0)
	    {
		    if (('C' == ch) || ('P' == ch))
		        cnt++;
		    else
		        cnt = 0;
		}
		else
		{
            wm_tool_send_esc2uart(30);
		}

		end = time(NULL);
		timeuse = difftime(end, start);
		if (timeuse >= 1)
		{
		    wm_tool_printf(".");

            timeout++;
            if ((timeout >= (WM_TOOL_DOWNLOAD_TIMEOUT_SEC / 10)) && note)
            {
                wm_tool_printf("\r\nplease manually reset the device.\r\n");
                note = 0;
            }
            else if (timeout > WM_TOOL_DOWNLOAD_TIMEOUT_SEC)
            {
                wm_tool_uart_close();
                wm_tool_printf("\r\nserial sync timeout.\r\n");
        		return -6;
            }

		    start = time(NULL);
		}
	} while (cnt < 3);

	wm_tool_printf("\r\nserial sync sucess.\r\n");

	ret = wm_tool_query_mac();
	if (ret)
    {
        wm_tool_uart_close();
        wm_tool_printf("download failed.\r\n");
        return ret;
    }

	if (WM_TOOL_DL_ERASE_SECBOOT == wm_tool_dl_erase)
	{
        ret = wm_tool_erase_image(WM_TOOL_DL_ERASE_SECBOOT);
        if (ret)
        {
            wm_tool_uart_close();
            wm_tool_printf("failed to erase.\r\n");
            return ret;
        }
	}
	else if (WM_TOOL_DL_ERASE_ALL == wm_tool_dl_erase)
	{
	    ret  = wm_tool_erase_image(WM_TOOL_DL_ERASE_SECBOOT);
        ret |= wm_tool_erase_image(WM_TOOL_DL_ERASE_ALL);
        if (ret)
        {
            wm_tool_uart_close();
            wm_tool_printf("failed to erase.\r\n");
            return ret;
        }
	}

    if (WM_TOOL_DEFAULT_BAUD_RATE != wm_tool_download_serial_rate)
    {
        ret = wm_tool_set_wifi_chip_speed(wm_tool_download_serial_rate);
    	if (ret > 0)
    	{
    		wm_tool_delay_ms(1 * 1000);
    		wm_tool_uart_set_speed(wm_tool_download_serial_rate);
    	}
	}

    ret = wm_tool_xmodem_download(wm_tool_download_image);
    if (ret)
    {
        if (WM_TOOL_DEFAULT_BAUD_RATE != wm_tool_download_serial_rate)
        {
            wm_tool_delay_ms(1 * 1000);
    		wm_tool_set_wifi_chip_speed(WM_TOOL_DEFAULT_BAUD_RATE);
    		wm_tool_delay_ms(1 * 1000);
        }
    }
    else
    {
        if (WM_TOOL_DL_TYPE_FLS == wm_tool_dl_type)
        {
            if (WM_TOOL_DL_ACTION_RTS == wm_tool_dl_action)/* auto reset */
            {
                wm_tool_uart_set_dtr(0);
                wm_tool_uart_set_rts(1);
                wm_tool_delay_ms(50);
                wm_tool_uart_set_dtr(1);
                wm_tool_uart_set_rts(0);
                wm_tool_delay_ms(50);
        		wm_tool_uart_set_dtr(0);
    		}
    		else
    		{
                wm_tool_printf("please manually reset the device.\r\n");
    		}
        }
    }

    wm_tool_uart_close();

    return ret;
}

static void wm_tool_show_local_com(void)
{
#if defined(__APPLE__) && defined(__MACH__)
    char *comstr = "tty.usbserial";
#elif defined(__CYGWIN__)
    int num;
    char *comstr = "ttyS";
#elif defined(__linux__)
    char *comstr = "ttyUSB";
#endif

#if defined(__APPLE__) || defined(__MACH__) || defined(__CYGWIN__) || defined(__linux__)

    DIR *dir;
	struct dirent *file;

	dir = opendir("/dev");

	if (dir)
	{
		while (NULL != (file = readdir(dir)))
		{

			if ((0 == strncmp(file->d_name, comstr, strlen(comstr))) && (DT_CHR == file->d_type))
			{
#if defined(__CYGWIN__)
                num = atoi(file->d_name + strlen(comstr));
	        	wm_tool_printf("COM%d ", num + 1);
#else
                wm_tool_printf("%s ", file->d_name);
#endif
			}
		}
		closedir(dir);
		wm_tool_printf("\r\n");
	}

#elif defined(__MINGW32__)

    LONG ret;
    HKEY key;
	DWORD i = 0;
	DWORD knlen;
    char kname[WM_TOOL_PATH_MAX] = {0};
	DWORD kvlen;
    char kvalue[WM_TOOL_PATH_MAX] = {0};
    REGSAM kmask = KEY_READ;

#if defined(KEY_WOW64_64KEY)
    BOOL is_64;

    if (IsWow64Process(GetCurrentProcess(), &is_64))
    {
        if (is_64)
            kmask |= KEY_WOW64_64KEY;
    }
#endif

    ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, NULL, 0, kmask, NULL, &key, NULL);
    if (ret != ERROR_SUCCESS)
    {
        return;
    }

	do{
	    knlen = sizeof(kname);
	    kvlen = sizeof(kvalue);
		ret = RegEnumValue(key, i, kname, &knlen, 0, NULL, (LPBYTE)kvalue, &kvlen);
		if (ret == ERROR_SUCCESS)
		{
            if (strcmp(kvalue, "") != 0)
            {
    			wm_tool_printf("%s ", kvalue);
		    }
		}

		i++;
	} while (ret != ERROR_NO_MORE_ITEMS);

    RegCloseKey(key);

    wm_tool_printf("\r\n");

#else

    wm_tool_printf("\r\nunsupported system.\r\n");

#endif

    return;
}
static void wm_tool_free_res(void)
{
    if (wm_tool_download_image)
        free(wm_tool_download_image);

    if (wm_tool_output_image)
        free(wm_tool_output_image);

    if (wm_tool_input_binary)
        free(wm_tool_input_binary);

    if (wm_tool_secboot_image)
        free(wm_tool_secboot_image);

    return;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    if (!wm_tool_parse_arv(argc, argv))
    {
        wm_tool_print_usage(argv[0]);
        wm_tool_free_res();
        return 0;
    }

    if (wm_tool_show_usage)
    {
        wm_tool_print_usage(argv[0]);
        wm_tool_free_res();
        return 0;
    }

    if (wm_tool_show_ver)
    {
        wm_tool_print_version(argv[0]);
        wm_tool_free_res();
        return 0;
    }

    if (wm_tool_list_com)
    {
        wm_tool_show_local_com();
        wm_tool_free_res();
        return 0;
    }

    if (wm_tool_input_binary)
    {
        ret = wm_tool_pack_firmware();
    }

    if (wm_tool_download_image)
    {
        ret = wm_tool_download_firmware();
    }

    if (wm_tool_show_log_type)
    {
        ret = wm_tool_show_log_from_serial();
    }

    if (ret > 0)
        wm_tool_print_usage(argv[0]);

    wm_tool_free_res();

    return ret;
}

