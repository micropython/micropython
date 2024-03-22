# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Damien P. George
# Copyright (c) 2020 Jim Mussared
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This script provides some helpers to allow Python code to access the IPCC
# mechanism in the WB55, and works with the memory layout configured in
# ports/stm32/rfcore.c -- i.e. it expects that rfcore_init() has been run.

# e.g.
# ../../tools/pyboard.py --device /dev/ttyACM0 boards/NUCLEO_WB55/rfcore.py
# to print out SRAM2A, register state and FUS/WS info.
#
# The `stm` module provides some helper functions to access rfcore functionality.
# See rfcore_firmware.py for more information.

from machine import mem8, mem16, mem32
from micropython import const
import stm

SRAM2A_BASE = const(0x2003_0000)

# for vendor OGF
OGF_VENDOR = const(0x3F)
OCF_FUS_GET_STATE = const(0x52)
OCF_FUS_FW_UPGRADE = const(0x54)
OCF_FUS_FW_DELETE = const(0x55)
OCF_FUS_START_WS = const(0x5A)
OCF_BLE_INIT = const(0x66)

TABLE_DEVICE_INFO = const(0)
TABLE_BLE = const(1)
TABLE_SYS = const(3)
TABLE_MEM_MANAGER = const(4)

CHANNEL_BLE = const(1)
CHANNEL_SYS = const(2)
CHANNEL_TRACES = const(4)
CHANNEL_ACL = const(6)

INDICATOR_HCI_COMMAND = const(0x01)
INDICATOR_HCI_EVENT = const(0x04)
INDICATOR_FUS_COMMAND = const(0x10)
INDICATOR_FUS_RESPONSE = const(0x11)
INDICATOR_FUS_EVENT = const(0x12)

MAGIC_FUS_ACTIVE = const(0xA94656B9)


def get_ipccdba():
    return mem32[stm.FLASH + stm.FLASH_IPCCBR] & 0x3FFF


def get_ipcc_table(table):
    return mem32[SRAM2A_BASE + get_ipccdba() + table * 4]


def get_ipcc_table_word(table, offset):
    return mem32[get_ipcc_table(table) + offset * 4] & 0xFFFFFFFF


def get_ipcc_table_byte(table, offset):
    return mem8[get_ipcc_table(table) + offset] & 0xFF


def sram2a_dump(num_words=64, width=8):
    print("SRAM2A @%08x" % SRAM2A_BASE)
    for i in range((num_words + width - 1) // width):
        print("  %04x " % (i * 4 * width), end="")
        for j in range(width):
            print(" %08x" % (mem32[SRAM2A_BASE + (i * width + j) * 4] & 0xFFFFFFFF), end="")
        print()


SYS_CMD_BUF = 0  # next*,prev*,type8,...; 272 bytes
SYS_SYS_QUEUE = 0  # next*,prev*

MM_BLE_SPARE_EVT_BUF = 0  # next*,prev*; 272 bytes
MM_SYS_SPARE_EVT_BUF = 0  # next*,prev*; 272 bytes
MM_BLE_POOL = 0  # ?
MM_BLE_POOL_SIZE = 0  # ?
MM_FREE_BUF_QUEUE = 0  # next*,prev*
MM_EV_POOL = 0  # ?
MM_EV_POOL_SIZE = 0  # ?

BLE_CMD_BUF = 0
BLE_CS_BUF = 0
BLE_EVT_QUEUE = 0
BLE_HCI_ACL_DATA_BUF = 0


def ipcc_init():
    global SYS_CMD_BUF, SYS_SYS_QUEUE
    SYS_CMD_BUF = get_ipcc_table_word(TABLE_SYS, 0)
    SYS_SYS_QUEUE = get_ipcc_table_word(TABLE_SYS, 1)

    global \
        MM_BLE_SPARE_EVT_BUF, \
        MM_SYS_SPARE_EVT_BUF, \
        MM_BLE_POOL, \
        MM_BLE_POOL_SIZE, \
        MM_FREE_BUF_QUEUE, \
        MM_EV_POOL, \
        MM_EV_POOL_SIZE
    MM_BLE_SPARE_EVT_BUF = get_ipcc_table_word(TABLE_MEM_MANAGER, 0)
    MM_SYS_SPARE_EVT_BUF = get_ipcc_table_word(TABLE_MEM_MANAGER, 1)
    MM_BLE_POOL = get_ipcc_table_word(TABLE_MEM_MANAGER, 2)
    MM_BLE_POOL_SIZE = get_ipcc_table_word(TABLE_MEM_MANAGER, 3)
    MM_FREE_BUF_QUEUE = get_ipcc_table_word(TABLE_MEM_MANAGER, 4)
    MM_EV_POOL = get_ipcc_table_word(TABLE_MEM_MANAGER, 5)
    MM_EV_POOL_SIZE = get_ipcc_table_word(TABLE_MEM_MANAGER, 6)

    global BLE_CMD_BUF, BLE_CS_BUF, BLE_EVT_QUEUE, BLE_HCI_ACL_DATA_BUF
    BLE_CMD_BUF = get_ipcc_table_word(TABLE_BLE, 0)
    BLE_CS_BUF = get_ipcc_table_word(TABLE_BLE, 1)
    BLE_EVT_QUEUE = get_ipcc_table_word(TABLE_BLE, 2)
    BLE_HCI_ACL_DATA_BUF = get_ipcc_table_word(TABLE_BLE, 3)

    # Disable interrupts, the code here uses polling
    mem32[stm.IPCC + stm.IPCC_C1CR] = 0

    print("IPCC initialised")
    print("SYS: 0x%08x 0x%08x" % (SYS_CMD_BUF, SYS_SYS_QUEUE))
    print("BLE: 0x%08x 0x%08x 0x%08x" % (BLE_CMD_BUF, BLE_CS_BUF, BLE_EVT_QUEUE))


def fus_active():
    return get_ipcc_table_word(TABLE_DEVICE_INFO, 0) == MAGIC_FUS_ACTIVE


def info():
    sfr = mem32[stm.FLASH + stm.FLASH_SFR]
    srrvr = mem32[stm.FLASH + stm.FLASH_SRRVR]

    print("IPCCDBA  : 0x%08x" % (get_ipccdba() & 0x3FFF))
    print("DDS      : %r" % bool(sfr & (1 << 12)))
    print("FSD      : %r" % bool(sfr & (1 << 8)))
    print("SFSA     : 0x%08x" % (sfr & 0xFF))
    print("C2OPT    : %r" % bool(srrvr & (1 << 31)))
    print("NBRSD    : %r" % bool(srrvr & (1 << 30)))
    print("SNBRSA   : 0x%08x" % ((srrvr >> 25) & 0x1F))
    print("BRSD     : %r" % bool(srrvr & (1 << 23)))
    print("SBRSA    : 0x%08x" % ((srrvr >> 18) & 0x1F))
    print("SBRV     : 0x%08x" % (srrvr & 0x3FFFF))


def dev_info():
    def dump_version(offset):
        x = get_ipcc_table_word(TABLE_DEVICE_INFO, offset)
        print(
            "0x%08x (%u.%u.%u.%u.%u)"
            % (x, x >> 24, x >> 16 & 0xFF, x >> 8 & 0xFF, x >> 4 & 0xF, x & 0xF)
        )

    def dump_memory_size(offset):
        x = get_ipcc_table_word(TABLE_DEVICE_INFO, offset)
        print(
            "0x%08x (SRAM2b=%uk SRAM2a=%uk flash=%uk)"
            % (x, x >> 24, x >> 16 & 0xFF, (x & 0xFF) * 4)
        )

    print("Device information table @%08x:" % get_ipcc_table(TABLE_DEVICE_INFO))
    if fus_active():
        # layout when running FUS
        print("FUS is active")
        print("state                    : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 0))
        print("last FUS active state    : 0x%02x" % get_ipcc_table_byte(TABLE_DEVICE_INFO, 5))
        print("last wireless stack state: 0x%02x" % get_ipcc_table_byte(TABLE_DEVICE_INFO, 6))
        print("cur wireless stack type  : 0x%02x" % get_ipcc_table_byte(TABLE_DEVICE_INFO, 7))
        print("safe boot version        : ", end="")
        dump_version(2)
        print("FUS version              : ", end="")
        dump_version(3)
        print("FUS memory size          : ", end="")
        dump_memory_size(4)
        print("wireless stack version   : ", end="")
        dump_version(5)
        print("wireless stack mem size  : ", end="")
        dump_memory_size(6)
        print("wireless FW-BLE info     : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 7))
        print("wireless FW-thread info  : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 8))
        print(
            "UID64                    : 0x%08x 0x%08x"
            % (
                get_ipcc_table_word(TABLE_DEVICE_INFO, 9),
                get_ipcc_table_word(TABLE_DEVICE_INFO, 10),
            )
        )
        print("device ID                : 0x%04x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 11))
    else:
        # layout when running WS
        print("WS is active")
        print("safe boot version        : ", end="")
        dump_version(0)
        print("FUS version              : ", end="")
        dump_version(1)
        print("FUS memory size          : ", end="")
        dump_memory_size(2)
        print("FUS info                 : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 3))
        print("wireless stack version   : ", end="")
        dump_version(4)
        print("wireless stack mem size  : ", end="")
        dump_memory_size(5)
        print("wireless stack info      : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 7))
        print("wireless reserved        : 0x%08x" % get_ipcc_table_word(TABLE_DEVICE_INFO, 7))


def ipcc_state():
    print("IPCC:")
    print("  C1CR:     0x%08x" % (mem32[stm.IPCC + stm.IPCC_C1CR] & 0xFFFFFFFF), end="")
    print("  C2CR:     0x%08x" % (mem32[stm.IPCC + stm.IPCC_C2CR] & 0xFFFFFFFF))
    print("  C1MR:     0x%08x" % (mem32[stm.IPCC + stm.IPCC_C1MR] & 0xFFFFFFFF), end="")
    print("  C2MR:     0x%08x" % (mem32[stm.IPCC + stm.IPCC_C2MR] & 0xFFFFFFFF))
    # these always read 0
    # print('  C1SCR:    0x%08x' % (mem32[stm.IPCC + stm.IPCC_C1SCR] & 0xffffffff), end='')
    # print('  C2SCR:    0x%08x' % (mem32[stm.IPCC + stm.IPCC_C2SCR] & 0xffffffff))
    print("  C1TOC2SR: 0x%08x" % (mem32[stm.IPCC + stm.IPCC_C1TOC2SR] & 0xFFFFFFFF), end="")
    print("  C2TOC1SR: 0x%08x" % (mem32[stm.IPCC + stm.IPCC_C2TOC1SR] & 0xFFFFFFFF))


sram2a_dump(264)
ipcc_init()
info()
dev_info()
ipcc_state()
