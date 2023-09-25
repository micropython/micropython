// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSoC62"    
#define MICROPY_HW_BOARD_NAME                   "CY8CPROTO-062-4343W"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-062-4343W"

#define MICROPY_GC_HEAP_SIZE                    (256 * 1024) // 256 KB

#define MICROPY_PY_HASHLIB                      (1)
#define MICROPY_PY_HASHLIB_MD5                  (1)
#define MICROPY_PY_HASHLIB_SHA1                 (1)
#define MICROPY_PY_HASHLIB_SHA256               (1)
