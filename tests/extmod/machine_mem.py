# This test requires root privilege, so is usually skipped
# It also assumes x86 legacy hardware (with Video BIOS present).

try:
    import machine
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

try:
    print(hex(machine.mem16[0xc0000]))
except OSError:
    print("SKIP")
    import sys
    sys.exit()
