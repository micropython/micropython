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

// ---- Flash budget and floating point ----
//
// The port splits internal flash into three regions (see ld/*.ld):
//
//   Sector 0        .vectors          16 KB
//   Sectors 1-3     littlefs          48 KB
//   Sectors 4-7     firmware image   448 KB
//
// Keeping the filesystem low and the code high (the same layout idea as
// ports/stm32 FLASH_START / FLASH_FS / FLASH_TEXT) gives the image 448 KB
// instead of the 384 KB a trailing filesystem allows.  Floating point costs
// about 32.7 KB, which did not fit in 384 KB but does fit in 448 KB.
//
// The GD32F4 is a Cortex-M4F and MCU_ARCH already builds with -mfloat-abi=hard
// -mfpu=fpv4-sp-d16, so the hardware is there; this only has to be turned on.
#ifndef MICROPY_FLOAT_IMPL
#define MICROPY_FLOAT_IMPL                (MICROPY_FLOAT_IMPL_FLOAT)
#endif

// ---- Interpreter performance ----
// Threaded dispatch and a small dictionary lookup cache, the same pair
// ports/stm32 enables for Cortex-M.  Costs about 1.1 KB.
#ifndef MICROPY_OPT_COMPUTED_GOTO
#define MICROPY_OPT_COMPUTED_GOTO         (1)
#endif
#ifndef MICROPY_OPT_MAP_LOOKUP_CACHE
#define MICROPY_OPT_MAP_LOOKUP_CACHE      (1)
#endif

// ---- Native code generation (stage 2) ----
// Enables @micropython.native and @micropython.viper, which compile Python to
// Thumb-2 machine code at runtime.  The GD32F4 is a Cortex-M4 so the ARMV7M
// instruction set is available (py/asmthumb.c picks it up from __ARM_ARCH_ISA_THUMB).
#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB                (1)
#endif
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB         (1)
#endif

// ---- Small feature switches (stage 1) ----
// SPI bit-order constants, so SPI(firstbit=SPI.LSB) works instead of the raw int.
#define MICROPY_PY_MACHINE_SPI_MSB        (0)
#define MICROPY_PY_MACHINE_SPI_LSB        (1)

// Allows raising exceptions from interrupt context at low cost.
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE   (0)   // dynamic allocation

// Lets the REPL print heap/stack usage, handy on a memory-tight port.
#define MICROPY_REPL_INFO                 (1)

#define MICROPY_LONGINT_IMPL              (MICROPY_LONGINT_IMPL_MPZ)
// newlib (bare-metal) does not provide SSIZE_MAX
#ifndef SSIZE_MAX
#define SSIZE_MAX (0x7FFFFFFF)
#endif
#define MICROPY_ENABLE_GC                 (1)
// The USB device stack schedules a task from its interrupt handler.
#define MICROPY_ENABLE_SCHEDULER          (1)
#define MICROPY_SCHEDULER_STATIC_NODES    (1)
#define MICROPY_ENABLE_COMPILER           (1)
// Frozen modules: bytecode generated at build time and stored in Flash.
// MICROPY_MODULE_FROZEN_MPY already comes from the configured ROM level.
#ifndef MICROPY_QSTR_EXTRA_POOL
#define MICROPY_QSTR_EXTRA_POOL           mp_qstr_frozen_const_pool
#endif
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
// FAT filesystem support, so an SD card formatted FAT16/FAT32 can be mounted
// alongside the internal littlefs2 volume.  extmod.mk compiles lib/oofatfs and
// passes -DMICROPY_VFS_FAT=1 when the make variable of the same name is set
// (see mpconfigport.mk), so guard this to avoid a redefinition.
#ifndef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT                   (1)
#endif
#define MICROPY_FATFS_ENABLE_LFN          (2)
#define MICROPY_FATFS_LFN_CODE_PAGE       437 /* token-pasted into TBL_CT437 */
#define MICROPY_FATFS_USE_LABEL           (1)
#define MICROPY_FATFS_RPATH               (2)
#define MICROPY_FATFS_MULTI_PARTITION     (1)
#define MICROPY_READER_VFS                (1)
#define MICROPY_PY_IO                     (1)
// os: listdir/stat/rename/remove/mkdir/mount over the built-in VFS
#define MICROPY_PY_OS                     (1)
#define MICROPY_PY_OS_SYNC                (1)
#define MICROPY_PY_OS_UNAME               (1)
#define MICROPY_PY_OS_URANDOM             (1)
// os.dupterm(): one extra stream can be attached to the REPL.
#define MICROPY_PY_OS_DUPTERM             (1)
#define MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM (0)
#define MICROPY_PY_UCTYPES                (1)
#define MICROPY_PY_UZLIB                  (0)
#define MICROPY_PY_URE                    (1)
#define MICROPY_PY_UJSON                  (1)
#define MICROPY_PY_FRAMEBUF               (1)
#define MICROPY_PY_UBINASCII              (1)
#define MICROPY_PY_HEAPQ                  (1)
#define MICROPY_PY_MACHINE                (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW   machine_pin_make_new
#define MICROPY_PY_MACHINE_INCLUDEFILE    "modmachine_port.c"

// trim modmachine to the essentials (Pin + freq + reset)
#define MICROPY_PY_MACHINE_PIN_BASE         (1)
#define MICROPY_PY_MACHINE_SIGNAL           (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
// DHT temperature/humidity sensors, built on machine.time_pulse_us().
#define MICROPY_PY_MACHINE_DHT_READINTO     (1)
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_MEMX           (1)
// machine.mem_backup(): the 20 RTC backup registers (80 bytes), retained across
// reset and deep sleep as long as VBAT/main supply is present.
#define MICROPY_PY_MACHINE_MEM_BACKUP     (1)
#define MICROPY_PY_MACHINE_MEM_BACKUP_INCLUDEFILE "machine_mem_backup_port.c"
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

// UART extras backed by machine_uart_port.c: break generation, single-char
// read/write, and an RX interrupt handler.
#define MICROPY_PY_MACHINE_UART_SENDBREAK       (1)
#define MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR (1)
#define MICROPY_PY_MACHINE_UART_IRQ             (1)
#define MICROPY_PY_MACHINE_SPI            (1)
#define MICROPY_PY_MACHINE_I2C            (1)
// Networking: lwIP over the GD32 ENET MAC
#ifndef MICROPY_PY_LWIP
#define MICROPY_PY_LWIP                    (1)
#endif
#define MICROPY_PY_LWIP_REPEAT_ARP         (1)
#define MICROPY_PY_NETWORK                 (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "gd32"

// Expose network.LAN in the network module (picked up by extmod/modnetwork.c).
// mpconfig.h is included very early, so forward-declare the type tag instead of
// pulling in py/obj.h here.
struct _mp_obj_type_t;
extern const struct _mp_obj_type_t network_lan_type;
#define MICROPY_PORT_NETWORK_INTERFACES \
    { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&network_lan_type) },
#define MICROPY_PY_SOCKET                  (1)
// OneWire (DS18B20 and friends) is a pure bit-bang protocol on top of the
// port's open-drain pin helpers, so it costs no extra hardware support.
#define MICROPY_PY_ONEWIRE                 (1)

// TLS via mbedTLS (see mbedtls/mbedtls_config_port.h for the trimmed profile).
// MICROPY_PY_SSL and MICROPY_SSL_MBEDTLS are supplied by the Makefile CFLAGS,
// so only the remaining options are set here.
#ifndef MICROPY_PY_SSL_FINALISER
#define MICROPY_PY_SSL_FINALISER           (1)
#endif
// mbedTLS allocates through MicroPython's tracked allocator on bare metal.
#ifndef MICROPY_TRACKED_ALLOC
#define MICROPY_TRACKED_ALLOC              (MICROPY_SSL_MBEDTLS)
#endif
#define MICROPY_PY_WEBSOCKET               (1)
#define MICROPY_PY_WEBREPL                 (1)

// USB device (TinyUSB DWC2 on USBFS)
#define MICROPY_HW_ENABLE_USBDEV          (1)
#define MICROPY_HW_USB_CDC                (1)
// GigaDevice is 0x28E9; use a private PID range for the MicroPython CDC
// interface so it does not clash with the vendor's own examples.
#define MICROPY_HW_USB_VID                (0x28E9)
#define MICROPY_HW_USB_PID                (0x0189)
#define MICROPY_HW_USB_MSC                (0)
// Expose machine.USBDevice so the device descriptors can be inspected and
// runtime device registration is available.
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE (1)
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
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)

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
