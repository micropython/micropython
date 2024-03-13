#include "genhdr/mpversion.h"
#include "py/mpconfig.h"

#ifdef MICROPY_BUILD_TYPE
#define MICROPY_BUILD_TYPE_PAREN " (" MICROPY_BUILD_TYPE ")"
#else
#define MICROPY_BUILD_TYPE_PAREN
#endif

#ifdef CONFIG_ESP_PANIC_HANDLER_IRAM
#define MICROPY_WRAP_PANICHANDLER_FUN(f) IRAM_ATTR f
#define MICROPY_WRAP_PANICHANDLER_STR(s) DRAM_STR(s)
#else
#define MICROPY_WRAP_PANICHANDLER_FUN(f) f
#define MICROPY_WRAP_PANICHANDLER_STR(s) (s)
#endif

void __real_esp_panic_handler(void *);
void esp_panic_handler_reconfigure_wdts(uint32_t timeout_ms);
void panic_print_str(const char *str);

void MICROPY_WRAP_PANICHANDLER_FUN(__wrap_esp_panic_handler)(void *info) {
    esp_panic_handler_reconfigure_wdts(1000);

    const static char *msg = MICROPY_WRAP_PANICHANDLER_STR(
        "\r\n"
        "A fatal error occurred. The crash dump printed below may be used to help\r\n"
        "determine what caused it. If you are not already running the most recent\r\n"
        "version of MicroPython, consider upgrading. New versions often fix bugs.\r\n"
        "\r\n"
        "To report this crash please copy all text below until 'Rebooting...' and\r\n"
        "file a bug report: https://github.com/micropython/micropython/issues/new\r\n"
        "\r\n"
        "MPY version : " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE MICROPY_BUILD_TYPE_PAREN "\r\n"
        "IDF version : " IDF_VER "\r\n"
        "Machine     : " MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME "\r\n"
        "\r\n"
        );
    panic_print_str(msg);

    __real_esp_panic_handler(info);
}
