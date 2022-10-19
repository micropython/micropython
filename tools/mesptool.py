# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
#
# A minimal esptool implementation to communicate with ESP32 ROM bootloader.
# Note this tool does Not support advanced features, other ESP chips or stub loading.
# This is only meant to be used for updating the U-blox Nina module firmware.

import os
import struct
from machine import Pin
from machine import UART
from micropython import const
from time import sleep


class mesptool:
    FLASH_ID = const(0)
    FLASH_BLOCK_SIZE = const(64 * 1024)
    FLASH_SECTOR_SIZE = const(4 * 1024)
    FLASH_PAGE_SIZE = const(256)

    CMD_SYNC = const(0x08)
    CMD_CHANGE_BAUDRATE = const(0x0F)
    CMD_SPI_ATTACH = const(0x0D)
    CMD_SPI_FLASH_MD5 = const(0x13)
    CMD_SPI_FLASH_PARAMS = const(0x0B)
    CMD_SPI_FLASH_BEGIN = const(0x02)
    CMD_SPI_FLASH_DATA = const(0x03)
    CMD_SPI_FLASH_END = const(0x04)

    ESP_ERRORS = {
        0x05: "Received message is invalid",
        0x06: "Failed to act on received message",
        0x07: "Invalid CRC in message",
        0x08: "Flash write error",
        0x09: "Flash read error",
        0x0A: "Flash read length error",
        0x0B: "Deflate error",
    }

    def __init__(
        self, reset=3, gpio0=2, uart_id=1, uart_tx=Pin(8), uart_rx=Pin(9), log_enabled=False
    ):
        self.uart_id = uart_id
        self.uart_tx = uart_tx
        self.uart_rx = uart_rx
        self.uart_buf = 4096
        self.uart_baudrate = 115200
        self.log = log_enabled
        self.reset_pin = Pin(reset, Pin.OUT, Pin.PULL_UP)
        self.gpio0_pin = Pin(gpio0, Pin.OUT, Pin.PULL_UP)
        self.set_baudrate(115200)

    def _log(self, data, out=True):
        if self.log:
            size = len(data)
            print(
                f"out({size}) => " if out else f"in({size})  <= ",
                "".join("%.2x" % (i) for i in data[0:10]),
            )

    def set_baudrate(self, baudrate, timeout=350):
        if baudrate != self.uart_baudrate:
            print(f"Changing baudrate => {baudrate}")
            self.uart_drain()
            self.command(CMD_CHANGE_BAUDRATE, struct.pack("<II", baudrate, 0))
            self.uart_baudrate = baudrate
        self.uart = UART(
            self.uart_id,
            baudrate,
            tx=self.uart_tx,
            rx=self.uart_rx,
            rxbuf=self.uart_buf,
            txbuf=self.uart_buf,
            timeout=timeout,
        )
        self.uart_drain()

    def uart_drain(self):
        while self.uart.read(1) is not None:
            pass

    def write_slip(self, pkt):
        pkt = pkt.replace(b"\xDB", b"\xdb\xdd").replace(b"\xc0", b"\xdb\xdc")
        self.uart.write(b"\xC0" + pkt + b"\xC0")
        self._log(pkt)

    def read_slip(self):
        pkt = None
        # Find the packet start.
        if self.uart.read(1) == b"\xC0":
            pkt = bytearray()
            while True:
                b = self.uart.read(1)
                if b is None or b == b"\xC0":
                    break
                pkt += b
            pkt = pkt.replace(b"\xDB\xDD", b"\xDB").replace(b"\xDB\xDC", b"\xC0")
            self._log(b"\xC0" + pkt + b"\xC0", False)
        return pkt

    def esperror(self, err):
        if err in self.ESP_ERRORS:
            return self.ESP_ERRORS[err]
        return "Unknown error"

    def checksum(self, data):
        checksum = 0xEF
        for i in data:
            checksum ^= i
        return checksum

    def command(self, cmd, payload=b"", checksum=0):
        self.write_slip(struct.pack(b"<BBHI", 0, cmd, len(payload), checksum) + payload)
        for i in range(10):
            pkt = self.read_slip()
            if pkt is not None and len(pkt) >= 8:
                (inout, cmd_id, size, val) = struct.unpack("<BBHI", pkt[:8])
                if inout == 1 and cmd == cmd_id:
                    status = list(pkt[-4:])
                    if status[0] == 1:
                        raise Exception(f"Command {cmd} failed {self.esperror(status[1])}")
                    return pkt[8:]
        raise Exception(f"Failed to read response to command {cmd}.")

    def bootloader(self, retry=6):
        for i in range(retry):
            self.gpio0_pin(1)
            self.reset_pin(0)
            sleep(0.1)
            self.gpio0_pin(0)
            self.reset_pin(1)
            sleep(0.1)
            self.gpio0_pin(1)

            if "POWERON_RESET" not in self.uart.read():
                continue

            for i in range(10):
                self.uart_drain()
                try:
                    # 36 bytes: 0x07 0x07 0x12 0x20, followed by 32 x 0x55
                    self.command(CMD_SYNC, b"\x07\x07\x12\x20" + 32 * b"\x55")
                    self.uart_drain()
                    return True
                except Exception as e:
                    print(e)

        raise Exception("Failed to enter download mode!")

    def flash_init(self, flash_size=2 * 1024 * 1024):
        self.command(CMD_SPI_ATTACH, struct.pack("<II", 0, 0))
        self.command(
            CMD_SPI_FLASH_PARAMS,
            struct.pack(
                "<IIIIII",
                self.FLASH_ID,
                flash_size,
                self.FLASH_BLOCK_SIZE,
                self.FLASH_SECTOR_SIZE,
                self.FLASH_PAGE_SIZE,
                0xFFFF,
            ),
        )

    def flash_write(self, path, blksize=0x1000):
        size = os.stat(path)[6]
        total_blocks = (size + blksize - 1) // blksize
        erase_blocks = 1
        print(f"Flash write size: {size} total_blocks: {total_blocks} block size: {blksize}")
        with open(path, "rb") as f:
            seq = 0
            subseq = 0
            for i in range(total_blocks):
                buf = f.read(blksize)
                if len(buf) < blksize:
                    # The last data block should be padded to the block size with 0xFF bytes.
                    buf += b"\xFF" * (blksize - len(buf))
                checksum = self.checksum(buf)
                if seq % erase_blocks == 0:
                    # print(f"Erasing {seq} -> {seq+erase_blocks}...")
                    self.command(
                        self.CMD_SPI_FLASH_BEGIN,
                        struct.pack(
                            "<IIII", erase_blocks * blksize, erase_blocks, blksize, seq * blksize
                        ),
                    )
                print(f"Writing sequence number {seq}/{total_blocks}...")
                self.command(
                    self.CMD_SPI_FLASH_DATA,
                    struct.pack("<IIII", len(buf), seq % erase_blocks, 0, 0) + buf,
                    checksum,
                )
                seq += 1

        print("Flash write finished")

    def flash_verify(self, path, md5sum, offset=0):
        size = os.stat(path)[6]
        data = self.command(CMD_SPI_FLASH_MD5, struct.pack("<IIII", offset, size, 0, 0))
        print(f"Flash verify file MD5 {md5sum}")
        print(f"Flash verify flash MD5 {bytes(data[0:32])}")
        if md5sum == data[0:32]:
            print("Firmware write verified")
        else:
            raise Exception(f"Firmware verification failed")

    def reboot(self):
        payload = struct.pack("<I", 0)
        self.write_slip(
            struct.pack(b"<BBHI", 0, self.CMD_SPI_FLASH_END, len(payload), 0) + payload
        )
