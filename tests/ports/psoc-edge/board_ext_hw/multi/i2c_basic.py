# I2C multi-device test
# Tests I2C master and slave communication between two boards
#
# KIT_PSE84_AI board Hardware connections:
#   Board 0 (Target)    Board 1 (Master)
#   P17_0 (SCL)    <->  P17_0 (SCL)
#   P17_1 (SDA)    <->  P17_1 (SDA)
#   GND            <->  GND
#
#   Pull-up resistors (4.7kΩ):
#   SCL -> VDD 3V3
#   SDA -> VDD 3V3

import time

try:
    from machine import I2C
except ImportError:
    print("SKIP")
    raise SystemExit


def instance0():
    # I2C Target (Slave)
    from machine import I2CTarget

    SLAVE_ADDR = 0x43

    # Create target with memory buffer
    mem = bytearray([0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00])
    i2c_target = I2CTarget(addr=SLAVE_ADDR, mem=mem)

    # Signal to master that slave is ready
    multitest.next()

    # Wait for transactions
    time.sleep(10)

    # Verify data was written by master
    if mem[0] == 0x01 and mem[1] == 0x02 and mem[2] == 0x03:
        print("target: PASS")
    else:
        print("target: FAIL")

    i2c_target.deinit()


def instance1():
    # I2C Master
    from machine import I2C

    print("\n***** I2C Master Test *****\n")

    # Wait for slave to be ready
    print("Waiting for slave...")
    multitest.next()
    print("Slave ready\n")

    SLAVE_ADDR = 0x43

    # Test 1: Scan
    print("***** Test 1: I2C Scan *****\n")
    i2c = I2C(freq=100000)
    devices = i2c.scan()
    print("Found devices:", [hex(d) for d in devices])
    scan_pass = SLAVE_ADDR in devices
    print("Status:", "PASS" if scan_pass else "FAIL")

    # Test 2: Write
    print("\n***** Test 2: Write *****\n")
    try:
        i2c.writeto(SLAVE_ADDR, b"\x01\x02\x03")
        time.sleep_ms(100)
        write_pass = True
        print("Status: PASS")
    except Exception as e:
        write_pass = False
        print("Status: FAIL -", e)

    # Test 3: Read
    print("\n***** Test 3: Read *****\n")
    try:
        data = i2c.readfrom(SLAVE_ADDR, 4)
        print("Received:", data)
        read_pass = len(data) == 4
        print("Status:", "PASS" if read_pass else "FAIL")
    except Exception as e:
        read_pass = False
        print("Status: FAIL -", e)

    # Summary
    print("\n***** Summary *****\n")
    all_pass = scan_pass and write_pass and read_pass
    print("Scan:  ", "PASS" if scan_pass else "FAIL")
    print("Write: ", "PASS" if write_pass else "FAIL")
    print("Read:  ", "PASS" if read_pass else "FAIL")
    print("\nOverall:", "PASS" if all_pass else "FAIL")
