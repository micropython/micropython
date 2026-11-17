# This is a regression test for https://github.com/micropython/micropython/issues/16619
# it runs in two parts by necessity:
#
# - This "part1" creates a non-terminating thread.
# - The test runner issues a soft reset, which will terminate that thread.
# - "part2" is the actual test, which is whether flash access works correctly
#   after the thread was terminated by soft reset.

import _thread


def infinite():
    while True:
        pass


_thread.start_new_thread(infinite, ())
print("Part 1 complete...")
