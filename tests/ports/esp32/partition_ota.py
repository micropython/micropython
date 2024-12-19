# Test ESP32 OTA updates, including automatic roll-back.
# Running this test requires firmware with an OTA Partition, such as the GENERIC_OTA "board".
# This test also requires patience as it copies the boot partition into the other OTA slot.
# The test runs two times to test both copy functions: ota_write() and ota_write_with_offset()

import machine
from esp32 import Partition

# start by checking that the running partition table has OTA partitions, 'cause if
# it doesn't there's nothing we can test
cur = Partition(Partition.RUNNING)
cur_name = cur.info()[4]
if not cur_name.startswith("ota_"):
    print("SKIP")
    raise SystemExit

DEBUG = True


def log(*args):
    if DEBUG:
        print(*args)


# If step does not exists, this means that this is the first pass
try:
    from step import PASS
except:
    PASS = 1

    # replace boot.py with the test code that will run on each reboot
    import os

    try:
        os.rename("boot.py", "boot-orig.py")
    except:
        pass

with open("boot.py", "w") as f:
    f.write("DEBUG=" + str(DEBUG))
    f.write(
        """
import machine
from esp32 import Partition
cur = Partition(Partition.RUNNING)
cur_name = cur.info()[4]

def log(*args):
    if DEBUG: print(*args)

from step import STEP, EXPECT, PASS
log("Running partition: " + cur_name + " STEP=" + str(STEP) + " EXPECT=" + EXPECT)
if cur_name != EXPECT:
    print("FAILED: step " + str(STEP) + " expected " + EXPECT + " got " + cur_name + "\\n")

if STEP == 0:
    log("Not confirming boot ok and resetting back into first")
    nxt = cur.get_next_update()
    with open("step.py", "w") as f:
        f.write("STEP=1\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\nPASS=" + str(PASS) + "\\n")
    machine.reset()
elif STEP == 1:
    log("Booting into second partition again")
    nxt = cur.get_next_update()
    nxt.set_boot()
    with open("step.py", "w") as f:
        f.write("STEP=2\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\nPASS=" + str(PASS) + "\\n")
    machine.reset()
elif STEP == 2:
    log("Confirming boot ok and rebooting into same partition")
    Partition.mark_app_valid_cancel_rollback()
    with open("step.py", "w") as f:
        f.write("STEP=3\\nEXPECT=\\"" + cur_name + "\\"\\nPASS=" + str(PASS) + "\\n")
    machine.reset()
elif STEP == 3:
    log("Mark new app invalid and revert to the original partition!")
    nxt = cur.get_next_update()
    with open("step.py", "w") as f:
        f.write("STEP=4\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\nPASS=" + str(PASS) + "\\n")
    if Partition.check_rollback_is_possible():
        Partition.mark_app_invalid_rollback_and_reboot()
elif STEP == 4:
    if PASS == 1:
        with open("step.py", "w") as f:
            f.write("STEP=0\\nEXPECT=\\"" + cur_name + "\\"\\nPASS=2\\n")
        
        with open("boot.py", "w") as f:
            f.write("import partition_ota")

        print("\\nGoing for pass 2\\n")
    elif PASS == 2:
        log("Confirming boot ok and DONE!")
        Partition.mark_app_valid_cancel_rollback()
        import os
        os.remove("step.py")
        os.remove("boot.py")
        os.rename("boot-orig.py", "boot.py")
        print("\\nSUCCESS!\\n")
    machine.reset()

"""
    )


def copy_partition_incr(src, dest):
    log("Partition copy: {} --> {}".format(src.info(), dest.info()))
    sz = src.info()[3]
    blk = bytearray(4096)
    addr = 0
    handle = dest.ota_begin()
    while addr < sz:
        if sz - addr < 4096:
            blk = blk[: sz - addr]
        if addr & 0xFFFF == 0:
            # need to show progress to run-tests.py else it times out
            print("   ... 0x{:06x}".format(addr))
        src.readblocks(addr >> 12, blk)
        dest.ota_write(handle, blk)
        addr += len(blk)
    dest.ota_end(handle)


def copy_partition_offset(src, dest):
    log("Partition copy: {} --> {}".format(src.info(), dest.info()))
    sz = src.info()[3]
    blk = bytearray(4096)
    addr = 0
    handle = dest.ota_begin()
    while addr < sz:
        if sz - addr < 4096:
            blk = blk[: sz - addr]
        if addr & 0xFFFF == 0:
            # need to show progress to run-tests else it times out
            print("   ... 0x{:06x}".format(addr))
        src.readblocks(addr >> 12, blk)
        dest.ota_write_with_offset(handle, blk, addr)
        addr += len(blk)
    dest.ota_end(handle)


# Test current partition if it holds a valid app description.
try:
    if cur.app_state() == "valid":
        print("TEST [SUCESSFUL]: Current partition contains a valid application")
    else:
        print(
            'TEST [FAILED]: Current partition state is "{}" expected "valid"'.format(
                cur.app_state()
            )
        )
        raise SystemExit
except OSError as e:
    print("TEST [FAILED]: Current partition state error is {}".format(e))

# Test to overwrite the running partition
try:
    copy_partition_incr(cur, cur)
except OSError as e:
    print("TEST [SUCESSFUL]: Prevent overwriting the running partition. Message: {}".format(e))
else:
    print("TEST [FAILED]: Current partition overwritten")
    raise SystemExit

nxt = cur.get_next_update()

# Test if the next partition already contains an app. This test is better to be conducted on an empty partition
try:
    nxt.app_state()
except OSError as e:
    print(
        "TEST [SUCESSFUL]: The next ota partition does not contain an app. Message: {}".format(e)
    )
else:
    print(
        'TEST [WARNING]: The state of the next ota partition is "{}". You should run the test on a freshly erased device'.format(
            nxt.app_state()
        )
    )

# Get things started by copying the current partition into the next slot and rebooting
# On the first pass use the incremental copy method
# On the second pass use the offset copy method
print("Copying current to next partition")
if PASS == 1:
    # Get ESP-IDF version as integer
    cur_desc = cur.app_description()[5]
    ver = int(cur_desc[1] + cur_desc[3])
    if ver >= 42:
        copy_partition_offset(cur, nxt)
    else:
        print(
            "TEST [SKIP]: Function 'ota_write_with_offset' is available since ESP-IDF version 4.2. Current ESP-IDF version {}.{}".format(
                cur_desc[1], cur_desc[3]
            )
        )
        PASS = 2
        copy_partition_incr(cur, nxt)
else:
    copy_partition_incr(cur, nxt)
print("Partition copied, booting into it")

# Test next partition state
try:
    nxt_sha = nxt.app_description()[6]
    cur_sha = cur.app_description()[6]
    if cur_sha == nxt_sha:
        print("TEST [SUCESSFUL]: The SHA256 of current and next partition are the same")
    else:
        print(
            "TEST [FAILED]: The SHA256 of current and next partition are different. Must be the same"
        )
        raise SystemExit
except OSError as e:
    print("TEST [FAILED]: Next partition is not valid. Error: {}".format(e))
    raise SystemExit

try:
    nxt.set_boot()
except OSError as e:
    print("TEST [FAILED]: Can not set the new partition for the next boot. Error: {}".format(e))
else:
    print("TEST [SUCESSFUL]: The next partition is set for boot")

# the step.py file is used to keep track of state across reboots
# EXPECT is the name of the partition we expect to reboot into
with open("step.py", "w") as f:
    f.write('STEP=0\nEXPECT="' + nxt.info()[4] + '"\nPASS=' + str(PASS) + "\n")

machine.reset()
