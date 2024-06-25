import machine
import time

# Verify that machine.idle() resumes execution within 0.1 and 1.1ms (should be
# 1ms max but allowing for some overhead).
#
# (A minimum sleep time for machine.idle() isn't specified but in a system like
# this with no active interrupts then we should expect some idle time before
# resuming. If it's consistently resuming immediately then that indicates a bug
# is preventing proper idle.)
#
# This test doesn't contain any rp2-specific code, but rp2 is currently the only
# tickless port - which is what led to the bug this is a regression test for.
# Some other ports (unix, esp32) have idle behaviour that resumes immediately on
# a quiet system, so this test is also less useful for those.
#
# Verification uses the average idle time, as individual iterations will always
# have outliers due to interrupts, scheduler, etc.

ITERATIONS = 500
total = 0

for _ in range(ITERATIONS):
    before = time.ticks_us()
    machine.idle()
    total += time.ticks_diff(time.ticks_us(), before)

total /= 1000  # us to ms
average = total / ITERATIONS

# print(f"Total {total}ms average {average}ms")  # uncomment for debug

if 0.1 < average < 1.1:
    print("PASS")
else:
    print(f"Total {total}ms average {average}ms, out of spec")
