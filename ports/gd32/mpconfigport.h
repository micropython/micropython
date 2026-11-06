// mpconfigport.h - MicroPython configuration for GD32F4xx port
#ifndef MICROPY_INCLUDED_GD32_MPCONFIGPORT_H
#define MICROPY_INCLUDED_GD32_MPCONFIGPORT_H

#include <stdint.h>
#include <alloca.h>

// Board-specific definitions must come first so that they can override the
// defaults below (e.g. MICROPY_HW_MCU_FREQ).  py/mpconfig.h includes this file
// very early, before any board header would otherwise be seen.
#include "mpconfigboard.h"

// Use the "extra features" ROM level as a base and tune below.
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#define MICROPY_LONGINT_IMPL              (MICROPY_LONGINT_IMPL_MPZ)
// newlib (bare-metal) does not provide SSIZE_MAX
#ifndef SSIZE_MAX
#define SSIZE_MAX (0x7FFFFFFF)
#endif
#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_ENABLE_COMPILER           (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_PYSTACK            (1)
#define MICROPY_PYSTACK_SIZE              (2048)
#define MICROPY_ALLOC_PATH_MAX            (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

// type definitions for the specific machine
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PY_SYS_PLATFORM           "gd32"
#define MICROPY_PY_SYS_EXIT               (1)

// disable modules whose sources are not included in this port
#define MICROPY_PY_SYS_STDFILES           (0)

// filesystem: littlefs2 on internal flash
#define MICROPY_VFS                       (1)
#define MICROPY_VFS_LFS2                  (1)
#define MICROPY_VFS_FAT                   (0)
#define MICROPY_READER_VFS                (1)
#define MICROPY_PY_IO                     (1)
// os: listdir/stat/rename/remove/mkdir/mount over the built-in VFS
#define MICROPY_PY_OS                     (1)
#define MICROPY_PY_OS_SYNC                (1)
#define MICROPY_PY_OS_UNAME               (1)
#define MICROPY_PY_OS_URANDOM             (1)
#define MICROPY_PY_UCTYPES                (1)
#define MICROPY_PY_UZLIB                  (0)
#define MICROPY_PY_UHASHLIB               (0)
#define MICROPY_PY_URE                    (1)
#define MICROPY_PY_UJSON                  (1)
#define MICROPY_PY_FRAMEBUF               (0)
#define MICROPY_PY_UBINASCII              (1)
#define MICROPY_PY_HEAPQ                  (1)
#define MICROPY_PY_MACHINE                (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW   machine_pin_make_new
#define MICROPY_PY_MACHINE_INCLUDEFILE    "modmachine_port.c"

// trim modmachine to the essentials (Pin + freq + reset)
#define MICROPY_PY_MACHINE_PIN_BASE         (1)
#define MICROPY_PY_MACHINE_SIGNAL           (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_MEMX           (1)
#define MICROPY_PY_MACHINE_MEM_BACKUP     (0)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_RESET            (1)
#define MICROPY_PY_MACHINE_BOOTLOADER       (0)
#define MICROPY_PY_MACHINE_FREQ_NUM_ARGS_MAX (1)

// peripherals
#define MICROPY_PY_MACHINE_TIMER          (1)
// soft timer uses this 32-bit ms counter (provided by mphalport.c)
extern volatile uint32_t systick_ms;
#define MICROPY_SOFT_TIMER_TICKS_MS systick_ms
#define MICROPY_PY_MACHINE_WDT            (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE "machine_wdt_port.c"
#define MICROPY_PY_MACHINE_WDT_TIMEOUT_MS (1)
#define MICROPY_PY_MACHINE_UART           (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "machine_uart_port.c"
#define MICROPY_PY_MACHINE_SPI            (1)
#define MICROPY_PY_MACHINE_I2C            (1)
#define MICROPY_PY_MACHINE_I2S            (1)
#define MICROPY_PY_MACHINE_I2S_INCLUDEFILE "machine_i2s_port.c"
#define MICROPY_PY_MACHINE_I2S_RING_BUF   (1)
// Bus constants exposed as I2S.RX / I2S.TX
#define MICROPY_PY_MACHINE_I2S_CONSTANT_RX  (0)
#define MICROPY_PY_MACHINE_I2S_CONSTANT_TX  (1)
#define MICROPY_PY_MACHINE_I2C_TARGET     (1)
#define MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE "machine_i2c_target_port.c"
#define MICROPY_PY_MACHINE_I2C_TARGET_MAX (2)
#define MICROPY_HW_NUM_CAN                (2)

// GD32F4 bxCAN timing limits (tq counts, classic CAN only)
#define CAN_TSEG1_MIN                     (1)
#define CAN_TSEG1_MAX                     (16)
#define CAN_TSEG2_MIN                     (1)
#define CAN_TSEG2_MAX                     (8)
#define CAN_SJW_MIN                       (1)
#define CAN_SJW_MAX                       (4)
#define CAN_BRP_MIN                       (1)
#define CAN_BRP_MAX                       (1024)
#define CAN_TX_QUEUE_LEN                  (3)
#define CAN_HW_MAX_FILTER                 (14)
#define CAN_FILTERS_STD_EXT_SEPARATE      (0)
#define MICROPY_HW_CAN_IS_RESERVED(can_id) (false)
#define MICROPY_PY_MACHINE_SDCARD         (1)
#define MICROPY_PY_MACHINE_SOFTSPI        (1)
#define MICROPY_PY_MACHINE_SOFTI2C        (1)
#define MICROPY_PY_MACHINE_PWM            (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE "machine_pwm_port.c"
#define MICROPY_PY_MACHINE_PWM_DUTY_U16_NS (1)
#define MICROPY_PY_MACHINE_PWM_DUTY       (0)
#define MICROPY_PY_MACHINE_CAN            (1)
#define MICROPY_PY_MACHINE_CAN_INCLUDEFILE "machine_can_port.c"
#define MICROPY_PY_MACHINE_DAC            (1)
#define MICROPY_PY_MACHINE_ADC            (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE "machine_adc_port.c"
#define MICROPY_PY_MACHINE_ADC_INIT       (0)
#define MICROPY_PY_MACHINE_ADC_DEINIT     (0)
#define MICROPY_PY_MACHINE_ADC_BLOCK      (0)
#define MICROPY_PY_MACHINE_ADC_READ_UV    (0)
#define MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH (0)
#define MICROPY_PY_MACHINE_ADC_READ       (0)
#define MICROPY_PY_MACHINE_ADC_READ_TIMED (0)
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(1) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(2) },

// ---- stdlib modules that need no external dependencies ----
// time: backed by mp_hal_time_ns() (RTC when set, else uptime epoch)
#define MICROPY_PY_TIME                     (1)
#define MICROPY_PY_TIME_INCLUDEFILE         "modtime_port.c"
#define MICROPY_PY_TIME_TIME_TIME_NS        (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (0)

// random: seed from the GD32 hardware true random number generator
uint32_t mp_hal_get_hw_random_u32(void);
#define MICROPY_PY_RANDOM                   (1)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS       (1)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC    (mp_hal_get_hw_random_u32())


// UART REPL baud rate
#ifndef MICROPY_HW_UART_REPL_BAUD
#define MICROPY_HW_UART_REPL_BAUD   (115200)
#endif


#define MICROPY_GC_STACK_ENTRY_TYPE uint32_t

// CPU frequency default
#ifndef MICROPY_HW_MCU_FREQ
#define MICROPY_HW_MCU_FREQ         (168000000)
#endif

#endif // MICROPY_INCLUDED_GD32_MPCONFIGPORT_H
