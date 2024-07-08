// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSoC63"
#define MICROPY_HW_BOARD_NAME                   "CY8CPROTO-063-BLE"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-063-BLE"

#define MICROPY_GC_HEAP_SIZE                    (64 * 1024) // 64 KB

#define MAX_UART             10 // TODO: Change as per bsp?
