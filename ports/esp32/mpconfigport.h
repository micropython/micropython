// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// Board-specific definitions
#include "mpconfigboard.h"

#include <stdint.h>
#include <alloca.h>
#include "esp_random.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "esp_wifi_types.h"

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL            (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// object representation and NLR handling
#define MICROPY_OBJ_REPR                    (MICROPY_OBJ_REPR_A)
#if CONFIG_IDF_TARGET_ARCH_XTENSA
#define MICROPY_NLR_SETJMP                  (1)
#endif

// memory allocation policies
#define MICROPY_ALLOC_PATH_MAX              (128)

// Initial Python heap size.  This starts small but adds new heap areas on demand due to
// the settings MICROPY_GC_SPLIT_HEAP and MICROPY_GC_SPLIT_HEAP_AUTO.  The value is
// different for different MCUs and is chosen so they can grow the heap once (double it)
// and still have enough internal RAM to start WiFi and make a HTTPS request.
#ifndef MICROPY_GC_INITIAL_HEAP_SIZE
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_GC_INITIAL_HEAP_SIZE        (56 * 1024)
#elif CONFIG_IDF_TARGET_ESP32S2 && !CONFIG_SPIRAM
#define MICROPY_GC_INITIAL_HEAP_SIZE        (36 * 1024)
#else
#define MICROPY_GC_INITIAL_HEAP_SIZE        (64 * 1024)
#endif
#endif

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
#if CONFIG_IDF_TARGET_ARCH_RISCV
#if CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT
#define MICROPY_EMIT_RV32                   (0)
#else
#define MICROPY_EMIT_RV32                   (1)
#endif
#else
#define MICROPY_EMIT_XTENSAWIN              (1)
#endif

// optimisations
#ifndef MICROPY_OPT_COMPUTED_GOTO
#define MICROPY_OPT_COMPUTED_GOTO           (1)
#endif

// Python internal features
#define MICROPY_READER_VFS                  (1)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_STACK_CHECK_MARGIN          (1024)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_WARNINGS                    (1)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_STREAMS_POSIX_API           (1)
#define MICROPY_USE_INTERNAL_ERRNO          (0) // errno.h from xtensa-esp32-elf/sys-include/sys
#define MICROPY_USE_INTERNAL_PRINTF         (0) // ESP32 SDK requires its own printf
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_VFS                         (1)

// control over Python builtins
#define MICROPY_PY_STR_BYTES_CMP_WARN       (1)
#define MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       esp32_help_text
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS        (1)
#define MICROPY_PY_TIME_INCLUDEFILE         "ports/esp32/modtime.c"
#define MICROPY_PY_THREAD                   (1)
#define MICROPY_PY_THREAD_GIL               (1)
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR    (32)

#define MICROPY_GC_SPLIT_HEAP               (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO          (1)

// extended modules
#ifndef MICROPY_PY_ESPNOW
#define MICROPY_PY_ESPNOW                   (1)
#endif
#ifndef MICROPY_PY_BLUETOOTH
#define MICROPY_PY_BLUETOOTH                (1)
#define MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS (1)
#define MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS_WITH_INTERLOCK (1)
// Event stack size is the RTOS stack size minus an allowance for the stack used
// by the NimBLE functions that call into invoke_irq_handler().
// MICROPY_STACK_CHECK_MARGIN is further subtracted from this value to set the stack limit.
#define MICROPY_PY_BLUETOOTH_SYNC_EVENT_STACK_SIZE (CONFIG_BT_NIMBLE_TASK_STACK_SIZE - 1024)
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
#define MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING (1)
#define MICROPY_BLUETOOTH_NIMBLE            (1)
#define MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY (1)
#endif
#define MICROPY_PY_HASHLIB_MD5              (1)
#define MICROPY_PY_HASHLIB_SHA1             (1)
#define MICROPY_PY_HASHLIB_SHA256           (1)
#define MICROPY_PY_CRYPTOLIB                (1)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC    (esp_random())
#define MICROPY_PY_OS_INCLUDEFILE           "ports/esp32/modos.c"
#define MICROPY_PY_OS_DUPTERM               (1)
#define MICROPY_PY_OS_DUPTERM_NOTIFY        (1)
#define MICROPY_PY_OS_SYNC                  (1)
#define MICROPY_PY_OS_UNAME                 (1)
#define MICROPY_PY_OS_URANDOM               (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE      "ports/esp32/modmachine.c"
#define MICROPY_PY_MACHINE_RESET            (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_ADC              (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE  "ports/esp32/machine_adc.c"
#define MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH  (1)
#define MICROPY_PY_MACHINE_ADC_INIT         (1)
#define MICROPY_PY_MACHINE_ADC_READ         (1)
#define MICROPY_PY_MACHINE_ADC_READ_UV      (1)
#define MICROPY_PY_MACHINE_ADC_BLOCK        (1)
#define MICROPY_PY_MACHINE_ADC_BLOCK_INCLUDEFILE "ports/esp32/machine_adc_block.c"
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_DHT_READINTO     (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_PWM              (1)
#define MICROPY_PY_MACHINE_PWM_DUTY         (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/esp32/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1 (1)
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#ifndef MICROPY_PY_MACHINE_DAC
#define MICROPY_PY_MACHINE_DAC              (SOC_DAC_SUPPORTED)
#endif
#ifndef MICROPY_PY_MACHINE_I2S
#define MICROPY_PY_MACHINE_I2S              (SOC_I2S_SUPPORTED)
#endif
#define MICROPY_PY_MACHINE_I2S_INCLUDEFILE  "ports/esp32/machine_i2s.c"
#define MICROPY_PY_MACHINE_I2S_FINALISER    (1)
#define MICROPY_PY_MACHINE_I2S_CONSTANT_RX  (I2S_DIR_RX)
#define MICROPY_PY_MACHINE_I2S_CONSTANT_TX  (I2S_DIR_TX)
#define MICROPY_PY_MACHINE_UART             (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "ports/esp32/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_SENDBREAK   (1)
#define MICROPY_PY_MACHINE_UART_IRQ         (1)
#define MICROPY_PY_MACHINE_WDT              (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE  "ports/esp32/machine_wdt.c"
#define MICROPY_PY_NETWORK (1)
#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32"
#elif CONFIG_IDF_TARGET_ESP32S2
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32s2"
#elif CONFIG_IDF_TARGET_ESP32S3
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32s3"
#elif CONFIG_IDF_TARGET_ESP32C3
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32c3"
#elif CONFIG_IDF_TARGET_ESP32C6
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32c6"
#endif
#endif
#define MICROPY_PY_NETWORK_INCLUDEFILE      "ports/esp32/modnetwork.h"
#define MICROPY_PY_NETWORK_MODULE_GLOBALS_INCLUDEFILE "ports/esp32/modnetwork_globals.h"
#ifndef MICROPY_PY_NETWORK_WLAN
#define MICROPY_PY_NETWORK_WLAN             (1)
#endif
#ifndef MICROPY_HW_ENABLE_SDCARD
#define MICROPY_HW_ENABLE_SDCARD            (1)
#endif
#define MICROPY_HW_SOFTSPI_MIN_DELAY        (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE     (esp_rom_get_cpu_ticks_per_us() * 1000000 / 200) // roughly
#define MICROPY_PY_SSL                      (1)
#define MICROPY_SSL_MBEDTLS                 (1)
#define MICROPY_PY_WEBSOCKET                (1)
#define MICROPY_PY_WEBREPL                  (1)
#define MICROPY_PY_ONEWIRE                  (1)
#define MICROPY_PY_SOCKET_EVENTS            (MICROPY_PY_WEBREPL)
#define MICROPY_PY_BLUETOOTH_RANDOM_ADDR    (1)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN            (1)
#define MICROPY_FATFS_RPATH                 (2)
#define MICROPY_FATFS_MAX_SS                (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE         437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */

// task size
#ifndef MICROPY_TASK_STACK_SIZE
#define MICROPY_TASK_STACK_SIZE             (16 * 1024)
#endif

#define MP_STATE_PORT MP_STATE_VM

#ifndef MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_ENABLE_USBDEV            (SOC_USB_OTG_SUPPORTED)
#endif

#if MICROPY_HW_ENABLE_USBDEV
#define MICROPY_SCHEDULER_STATIC_NODES        (1)
#define MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER (1)

#ifndef MICROPY_HW_USB_VID
#define USB_ESPRESSIF_VID 0x303A
#if CONFIG_TINYUSB_DESC_USE_ESPRESSIF_VID
#define MICROPY_HW_USB_VID  (USB_ESPRESSIF_VID)
#else
#define MICROPY_HW_USB_VID  (CONFIG_TINYUSB_DESC_CUSTOM_VID)
#endif
#endif

#ifndef MICROPY_HW_USB_PID
#if CONFIG_TINYUSB_DESC_USE_DEFAULT_PID
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
// A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
// Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
// Auto ProductID layout's Bitmap:
//   [MSB]         HID | MSC | CDC          [LSB]
#define USB_TUSB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
    _PID_MAP(MIDI, 3))  // | _PID_MAP(AUDIO, 4) | _PID_MAP(VENDOR, 5) )
#define MICROPY_HW_USB_PID  (USB_TUSB_PID)
#else
#define MICROPY_HW_USB_PID  (CONFIG_TINYUSB_DESC_CUSTOM_PID)
#endif
#endif

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#ifdef CONFIG_TINYUSB_DESC_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING CONFIG_TINYUSB_DESC_MANUFACTURER_STRING
#else
#define MICROPY_HW_USB_MANUFACTURER_STRING "MicroPython"
#endif
#endif

#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#ifdef CONFIG_TINYUSB_DESC_PRODUCT_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING CONFIG_TINYUSB_DESC_PRODUCT_STRING
#else
#define MICROPY_HW_USB_PRODUCT_FS_STRING "Board in FS mode"
#endif
#endif

#endif // MICROPY_HW_ENABLE_USBDEV

// Enable stdio over native USB peripheral CDC via TinyUSB
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC                  (MICROPY_HW_ENABLE_USBDEV)
#endif

// Enable stdio over USB Serial/JTAG peripheral
#ifndef MICROPY_HW_ESP_USB_SERIAL_JTAG
#define MICROPY_HW_ESP_USB_SERIAL_JTAG      (SOC_USB_SERIAL_JTAG_SUPPORTED && !MICROPY_HW_USB_CDC)
#endif

#if MICROPY_HW_USB_CDC && MICROPY_HW_ESP_USB_SERIAL_JTAG
#error "Invalid build config: Can't enable both native USB and USB Serial/JTAG peripheral"
#endif

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p)))
#if SOC_CPU_IDRAM_SPLIT_USING_PMP && !CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT
// On targets with this configuration all RAM is executable so no need for a custom commit function.
#else
void *esp_native_code_commit(void *, size_t, void *);
#define MP_PLAT_COMMIT_EXEC(buf, len, reloc) esp_native_code_commit(buf, len, reloc)
#endif
#define MP_SSIZE_MAX (0x7fffffff)

#if MICROPY_PY_SOCKET_EVENTS
#define MICROPY_PY_SOCKET_EVENTS_HANDLER extern void socket_events_handler(void); socket_events_handler();
#else
#define MICROPY_PY_SOCKET_EVENTS_HANDLER
#endif

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_SOCKET_EVENTS_HANDLER \
        MP_THREAD_GIL_EXIT(); \
        ulTaskNotifyTake(pdFALSE, 1); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_SOCKET_EVENTS_HANDLER \
        asm ("waiti 0"); \
    } while (0);
#endif

// Functions that should go in IRAM
// For ESP32 with SPIRAM workaround, firmware is larger and uses more static IRAM,
// so in that configuration don't put too many functions in IRAM.
#if !(CONFIG_IDF_TARGET_ESP32 && CONFIG_SPIRAM && CONFIG_SPIRAM_CACHE_WORKAROUND)
#define MICROPY_WRAP_MP_BINARY_OP(f) IRAM_ATTR f
#endif
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_LOAD_GLOBAL(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_LOAD_NAME(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_MAP_LOOKUP(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_OBJ_GET_TYPE(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_SCHED_EXCEPTION(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(f) IRAM_ATTR f

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
// ssize_t, off_t as required by POSIX-signatured functions in stream.h
#include <sys/types.h>

// board specifics
#define MICROPY_PY_SYS_PLATFORM "esp32"

// ESP32-S3 extended IO for 47 & 48
#ifndef MICROPY_HW_ESP32S3_EXTENDED_IO
#define MICROPY_HW_ESP32S3_EXTENDED_IO      (1)
#endif

#ifndef MICROPY_HW_ENABLE_MDNS_QUERIES
#define MICROPY_HW_ENABLE_MDNS_QUERIES      (1)
#endif

#ifndef MICROPY_HW_ENABLE_MDNS_RESPONDER
#define MICROPY_HW_ENABLE_MDNS_RESPONDER    (1)
#endif

#ifndef MICROPY_BOARD_ENTER_BOOTLOADER
// RTC has a register to trigger bootloader on these targets
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
#define MICROPY_ESP32_USE_BOOTLOADER_RTC    (1)
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) machine_bootloader_rtc()
#endif
#endif

#ifdef MICROPY_BOARD_ENTER_BOOTLOADER
#define MICROPY_PY_MACHINE_BOOTLOADER       (1)
#else
#define MICROPY_PY_MACHINE_BOOTLOADER       (0)
#endif

// Workaround for upstream bug https://github.com/espressif/esp-idf/issues/14273
// Can be removed if a fix is available in supported ESP-IDF versions.
#define MICROPY_PY_MATH_GAMMA_FIX_NEGINF (1)

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP boardctrl_startup
#endif

void boardctrl_startup(void);

#ifndef MICROPY_PY_NETWORK_LAN
#if CONFIG_IDF_TARGET_ESP32 || (CONFIG_ETH_USE_SPI_ETHERNET && (CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL || CONFIG_ETH_SPI_ETHERNET_DM9051 || CONFIG_ETH_SPI_ETHERNET_W5500))
#define MICROPY_PY_NETWORK_LAN              (1)
#else
#define MICROPY_PY_NETWORK_LAN              (0)
#endif
#endif

#if MICROPY_PY_NETWORK_LAN && CONFIG_ETH_USE_SPI_ETHERNET
#ifndef MICROPY_PY_NETWORK_LAN_SPI_CLOCK_SPEED_MZ
#define MICROPY_PY_NETWORK_LAN_SPI_CLOCK_SPEED_MZ       (20)
#endif
#endif

// The minimum string length threshold for string printing to stdout operations to be GIL-aware.
#ifndef MICROPY_PY_STRING_TX_GIL_THRESHOLD
#define MICROPY_PY_STRING_TX_GIL_THRESHOLD  (20)
#endif
