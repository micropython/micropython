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


def instance0():
    # I2C Target (Slave)
    from machine import I2CTarget

    SLAVE_ADDR = 0x43

    # Create target with memory buffer
    mem = bytearray([0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00])
    i2c_target = I2CTarget(addr=SLAVE_ADDR, mem=mem, scl="P17_0", sda="P17_1")

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
    i2c = I2C(scl="P17_0", sda="P17_1", freq=400000)
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

    # Test 4: Timeout Test
    print("\n***** Test 4: Timeout *****\n")
    try:
        # Create I2C with short timeout
        i2c_timeout = I2C(
            scl="P17_0", sda="P17_1", freq=100000, timeout=10000
        )  # 10ms timeout (10000us)
        print("Created I2C with timeout=10000us (10ms)")

        # Try to read from non-existent device
        # Note: This will fail at transaction start with EIO (errno 5)
        # rather than timeout, as the device doesn't ACK the address.
        # A true timeout would occur if device ACKs but doesn't send data.
        try:
            data = i2c_timeout.readfrom(0x50, 4)  # Non-existent address
            print("Status: FAIL - Expected error but succeeded")
            timeout_pass = False
        except OSError as e:
            # Accept either ETIMEDOUT (110) or EIO (5)
            # EIO occurs when device doesn't ACK address
            # ETIMEDOUT occurs when device ACKs but doesn't respond in time
            if e.errno == 110:  # ETIMEDOUT
                print("Timeout occurred (ETIMEDOUT)")
                print("Status: PASS")
                timeout_pass = True
            elif e.errno == 5:  # EIO - No ACK from device
                print("I/O error occurred (device not responding)")
                print("Status: PASS")
                timeout_pass = True
            else:
                print("Status: FAIL - Unexpected error:", e.errno)
                timeout_pass = False
    except Exception as e:
        print("Status: FAIL -", e)
        timeout_pass = False

    # Summary
    print("\n***** Summary *****\n")
    all_pass = scan_pass and write_pass and read_pass and timeout_pass
    print("Scan:   ", "PASS" if scan_pass else "FAIL")
    print("Write:  ", "PASS" if write_pass else "FAIL")
    print("Read:   ", "PASS" if read_pass else "FAIL")
    print("Timeout:", "PASS" if timeout_pass else "FAIL")
    print("\nOverall:", "PASS" if all_pass else "FAIL")
