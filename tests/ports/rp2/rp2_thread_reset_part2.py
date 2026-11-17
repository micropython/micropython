# This is part2 of a two-part regression test, see part1
# for details of what's expected.
import os

FILENAME = "/rp2_thread_reset_test.txt"

print("Starting")
with open(FILENAME, "w") as f:
    f.write("test")
print("Written")
os.unlink(FILENAME)
print("Removed")
