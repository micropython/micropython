# Test ESP32 OTA updates, including automatic roll-back.
# Running this test requires firmware with an OTA Partition, such as the GENERIC_OTA "board".
# This test also requires patience as it copies the boot partition into the other OTA slot.

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


# replace boot.py with the test code that will run on each reboot
import uos

try:
    uos.rename("boot.py", "boot-orig.py")
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

from step import STEP, EXPECT
log("Running partition: " + cur_name + " STEP=" + str(STEP) + " EXPECT=" + EXPECT)
if cur_name != EXPECT:
    print("\\x04FAILED: step " + str(STEP) + " expected " + EXPECT + " got " + cur_name + "\\x04")

if STEP == 0:
    log("Not confirming boot ok and resetting back into first")
    nxt = cur.get_next_update()
    with open("step.py", "w") as f:
        f.write("STEP=1\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\n")
    machine.reset()
elif STEP == 1:
    log("Booting into second partition again")
    nxt = cur.get_next_update()
    nxt.set_boot()
    with open("step.py", "w") as f:
        f.write("STEP=2\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\n")
    machine.reset()
elif STEP == 2:
    log("Confirming boot ok and rebooting into same partition")
    Partition.mark_app_valid_cancel_rollback()
    with open("step.py", "w") as f:
        f.write("STEP=3\\nEXPECT=\\"" + cur_name + "\\"\\n")
    machine.reset()
elif STEP == 3:
    log("Booting into original partition")
    nxt = cur.get_next_update()
    nxt.set_boot()
    with open("step.py", "w") as f:
        f.write("STEP=4\\nEXPECT=\\"" + nxt.info()[4] + "\\"\\n")
    machine.reset()
elif STEP == 4:
    log("Confirming boot ok and DONE!")
    Partition.mark_app_valid_cancel_rollback()
    import uos
    uos.remove("step.py")
    uos.remove("boot.py")
    uos.rename("boot-orig.py", "boot.py")
    print("\\nSUCCESS!\\n\\x04\\x04")

"""
    )


def copy_partition(src, dest):
    log("Partition copy: {} --> {}".format(src.info(), dest.info()))
    sz = src.info()[3]
    if dest.info()[3] != sz:
        raise ValueError("Sizes don't match: {} vs {}".format(sz, dest.info()[3]))
    addr = 0
    blk = bytearray(4096)
    while addr < sz:
        if sz - addr < 4096:
            blk = blk[: sz - addr]
        if addr & 0xFFFF == 0:
            # need to show progress to run-tests else it times out
            print("   ... 0x{:06x}".format(addr))
        src.readblocks(addr >> 12, blk)
        dest.writeblocks(addr >> 12, blk)
        addr += len(blk)


# get things started by copying the current partition into the next slot and rebooting
print("Copying current to next partition")
nxt = cur.get_next_update()
copy_partition(cur, nxt)
print("Partition copied, booting into it")
nxt.set_boot()

# the step.py file is used to keep track of state across reboots
# EXPECT is the name of the partition we expect to reboot into
with open("step.py", "w") as f:
    f.write('STEP=0\nEXPECT="' + nxt.info()[4] + '"\n')

machine.reset()
