# Test using micropython.schedule() to schedule a KeyboardInterrupt.
# Note: this test cannot use unittest because the unittest overhead
# may trigger the scheduler when we don't want it to.

try:
    from time import sleep
    from micropython import schedule, kbd_intr
except ImportError:
    print("SKIP")
    raise SystemExit

########################################################################
# Test basic scheduling.

try:
    schedule(kbd_intr)
except ValueError:
    # Scheduling `kbd_intr` is not supported on this target.
    print("SKIP")
    raise SystemExit

try:
    sleep(0)
    sleep(0)
    print("fail")
except KeyboardInterrupt:
    print("KeyboardInterrupt")

########################################################################
# Should be able to schedule it many times without error, and that
# should only trigger one KeyboardInterrupt.

schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
schedule(kbd_intr)
try:
    sleep(0)
    sleep(0)
    print("fail")
except KeyboardInterrupt:
    print("KeyboardInterrupt")

# This should not raise.
for _ in range(100):
    sleep(0)

########################################################################
# Test nested scheduling.


def callback(_):
    schedule(kbd_intr)
    # This should not raise because we are in schedule context.
    for _ in range(100):
        sleep(0)


schedule(callback, None)
sleep(0)
try:
    sleep(0)
    sleep(0)
    print("fail")
except KeyboardInterrupt:
    print("KeyboardInterrupt")
