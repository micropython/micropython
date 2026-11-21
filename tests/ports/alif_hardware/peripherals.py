# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test UART and SPI peripheral setup and clocking.
#
# IMPORTANT: This test requires hardware connections: the UART in
# `uart_loopback_args` must have TX and RX wired together.
#
# requires: target-wiring

import time
from machine import SPI, UART
from target_wiring import uart_loopback_args, uart_loopback_kwargs, spi_standalone_args_list

# UART loopback at a range of baud rates.  Checks peripheral clock setup and the
# baud rate divisor calculation.
for baudrate in (9600, 115200, 921600, 2000000):
    uart = UART(*uart_loopback_args, **uart_loopback_kwargs)
    uart.init(baudrate=baudrate)
    uart.read()  # flush
    msg = bytes(range(64))
    uart.write(msg)
    time.sleep_ms(len(msg) * 10 * 1000 // baudrate + 20)
    print("uart", baudrate, uart.read() == msg)
uart.deinit()

# SPI transfers without a loopback, checking they complete.
for spi_args in spi_standalone_args_list:
    spi = SPI(*spi_args, baudrate=10_000_000)
    buf = bytearray(256)
    spi.write_readinto(bytes(256), buf)
    spi.write(bytes(range(256)))
    print("spi", True)
    spi.deinit()
