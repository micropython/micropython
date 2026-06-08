# SPI multi-device test
# Tests SPI master and target communication between two boards
#
# KIT_PSE84_AI board Hardware connections:
#   Board 0 (Target)    Board 1 (Master)
#   P16_0 (SCK)    <->  P16_0 (SCK)
#   P16_1 (MOSI)   <->  P16_1 (MOSI)
#   P16_2 (MISO)   <->  P16_2 (MISO)
#   P16_3 (SSEL)   <->  P16_3 (CS out)
#   GND            <->  GND
#
# Other supported board mappings:
#   CY8CPROTO-062-4343W / CY8CPROTO-063-BLE:
#   SCK=P9_2, MOSI=P9_0, MISO=P9_1, SSEL=P9_3

import os
import time


def _get_spi_pins_for_board():
    uname_fn = getattr(os, "uname", None)
    if uname_fn is None:
        # Some firmware builds do not expose os.uname(); default to KIT mapping.
        return ("P16_0", "P16_1", "P16_2", "P16_3")

    board = uname_fn().machine
    board_norm = board.upper().replace("-", "_")

    if "KIT_PSE84_AI" in board_norm or "PSE84" in board_norm:
        return ("P16_0", "P16_1", "P16_2", "P16_3")
    if "CY8CPROTO_062_4343W" in board_norm or "CY8CPROTO_063_BLE" in board_norm:
        return ("P9_2", "P9_0", "P9_1", "P9_3")
    if "CY8CKIT_062S2_AI" in board_norm:
        print("SKIP: SPI target path is not supported on CY8CKIT-062S2-AI")
        raise SystemExit

    print("SKIP: Unsupported board for SPI multi-device test:", board)
    raise SystemExit


SCK_PIN, MOSI_PIN, MISO_PIN, SSEL_PIN = _get_spi_pins_for_board()


def instance0():
    # SPI Target (Slave)
    from machine import SPITarget

    print("\n***** SPI Target Test *****\n")

    target = SPITarget(
        polarity=0,
        phase=0,
        bits=8,
        firstbit=SPITarget.MSB,
        sck=SCK_PIN,
        mosi=MOSI_PIN,
        miso=MISO_PIN,
        ssel=SSEL_PIN,
    )

    # Signal to master that target is ready
    multitest.next()

    # Test 1: Read from master
    print("***** Test 1: Read *****\n")
    rx = bytearray(4)
    n = target.read(rx)
    print("Received:", rx[:n])
    read_pass = rx[:n] == b"\x01\x02\x03\x04"
    print("Status:", "PASS" if read_pass else "FAIL")

    # Signal master that read is done
    multitest.next()

    # Wait for master to be ready for write test
    multitest.next()

    # Test 2: Write to master
    print("\n***** Test 2: Write *****\n")
    tx = b"\xaa\xbb\xcc\xdd"
    n = target.write(tx)
    print("Wrote:", n, "bytes")
    write_pass = n == 4
    print("Status:", "PASS" if write_pass else "FAIL")

    # Signal master that write is done
    multitest.next()

    # Wait for master to be ready for write_readinto test
    multitest.next()

    # Test 3: Write and read simultaneously
    print("\n***** Test 3: Write+Read *****\n")
    tx = b"\x11\x22\x33\x44"
    rx = bytearray(4)
    n = target.write_readinto(tx, rx)
    print("Sent:", tx)
    print("Received:", rx[:n])
    readinto_pass = rx[:n] == b"\xde\xad\xbe\xef"
    print("Status:", "PASS" if readinto_pass else "FAIL")

    multitest.next()

    target.deinit()

    # Summary
    print("\n***** Target Summary *****")
    all_pass = read_pass and write_pass and readinto_pass
    print("Overall:", "PASS" if all_pass else "FAIL")


def instance1():
    # SPI Master (Controller)
    from machine import SPI, Pin

    print("\n***** SPI Master Test *****\n")

    spi = SPI(
        baudrate=1_000_000,
        polarity=0,
        phase=0,
        bits=8,
        firstbit=SPI.MSB,
        sck=SCK_PIN,
        mosi=MOSI_PIN,
        miso=MISO_PIN,
    )
    cs = Pin(SSEL_PIN, Pin.OUT, value=1)

    # Wait for target to be ready
    print("Waiting for target...")
    multitest.next()
    print("Target ready\n")

    # Test 1: Write to target
    print("***** Test 1: Write *****\n")
    tx = b"\x01\x02\x03\x04"
    cs(0)
    time.sleep_us(10)
    spi.write(tx)
    time.sleep_us(10)
    cs(1)
    print("Sent:", tx)
    print("Status: PASS")

    # Wait for target to finish reading
    multitest.next()

    # Test 2: Read from target
    print("\n***** Test 2: Read *****\n")
    # Signal target to start writing
    multitest.next()
    rx = bytearray(4)
    cs(0)
    time.sleep_us(10)
    spi.readinto(rx)
    time.sleep_us(10)
    cs(1)
    print("Received:", rx)
    read_pass = rx == b"\xaa\xbb\xcc\xdd"
    print("Status:", "PASS" if read_pass else "FAIL")

    # Wait for target to finish write
    multitest.next()

    # Test 3: Simultaneous write and read
    print("\n***** Test 3: Transfer *****\n")
    # Signal target to start write_readinto
    multitest.next()

    tx = b"\xde\xad\xbe\xef"
    rx = bytearray(4)
    cs(0)
    time.sleep_us(10)
    spi.write_readinto(tx, rx)
    time.sleep_us(10)
    cs(1)
    print("Sent:", tx)
    print("Received:", rx)
    transfer_pass = rx == b"\x11\x22\x33\x44"
    print("Status:", "PASS" if transfer_pass else "FAIL")

    multitest.next()

    spi.deinit()

    # Summary
    print("\n***** Master Summary *****")
    all_pass = read_pass and transfer_pass
    print("Overall:", "PASS" if all_pass else "FAIL")
