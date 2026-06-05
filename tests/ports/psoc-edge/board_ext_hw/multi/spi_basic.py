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

import time


def instance0():
    # SPI Target (Slave)
    from machine import SPITarget

    print("\n***** SPI Target Test *****\n")

    target = SPITarget(
        polarity=0,
        phase=0,
        bits=8,
        firstbit=SPITarget.MSB,
        sck="P16_0",
        mosi="P16_1",
        miso="P16_2",
        ssel="P16_3",
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
        sck="P16_0",
        mosi="P16_1",
        miso="P16_2",
    )
    cs = Pin("P16_3", Pin.OUT, value=1)

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
