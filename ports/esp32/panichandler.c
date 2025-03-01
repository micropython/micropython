/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Mindhash B.V.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
        "To learn more about how to debug and/or report this crash visit the wiki\r\n"
        "page at: https://github.com/micropython/micropython/wiki/ESP32-debugging\r\n"
        "\r\n"
        "MPY version : " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE MICROPY_BUILD_TYPE_PAREN "\r\n"
        "IDF version : " IDF_VER "\r\n"
        "Machine     : " MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME "\r\n"
        "\r\n"
        );
    panic_print_str(msg);

    __real_esp_panic_handler(info);
}
