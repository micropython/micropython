// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSoC63"
#define MICROPY_HW_BOARD_NAME                   "CY8CPROTO-063-BLE"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-063-BLE"

#define MICROPY_GC_HEAP_SIZE                    (64 * 1024) // 64 KB

#define MICROPY_PY_HASHLIB                      (0)
#define MICROPY_PY_HASHLIB_MD5                  (0)
#define MICROPY_PY_HASHLIB_SHA1                 (0)
#define MICROPY_PY_HASHLIB_SHA256               (0)

