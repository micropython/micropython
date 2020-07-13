/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include "py/builtin.h"

const char esp_help_text[] =
    "Welcome to MicroPython!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/en/latest/esp8266/ .\n"
    "For diagnostic information to include in bug reports execute 'import port_diag'.\n"
    "\n"
    "Basic WiFi configuration:\n"
    "\n"
    "import network\n"
    "sta_if = network.WLAN(network.STA_IF); sta_if.active(True)\n"
    "sta_if.scan()                             # Scan for available access points\n"
    "sta_if.connect(\"<AP_name>\", \"<password>\") # Connect to an AP\n"
    "sta_if.isconnected()                      # Check for successful connection\n"
    "# Change name/password of ESP8266's AP:\n"
    "ap_if = network.WLAN(network.AP_IF)\n"
    "ap_if.config(essid=\"<AP_NAME>\", authmode=network.AUTH_WPA_WPA2_PSK, password=\"<password>\")\n"
    "\n"
    "Control commands:\n"
    "  CTRL-A        -- on a blank line, enter raw REPL mode\n"
    "  CTRL-B        -- on a blank line, enter normal REPL mode\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, do a soft reset of the board\n"
    "  CTRL-E        -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
;
