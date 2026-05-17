# Ensure that disabling IRQs creates mutual exclusion between threads
# (also tests nesting of disable_irq across threads)
import machine
import time
import _thread

if not hasattr(machine, "disable_irq"):
    print("SKIP")
    raise SystemExit

count = 0
thread_done = False


def inc_count():
    global count
    a = machine.disable_irq()
    try:
        count += 1
        i = 0
        while i < 20:
            b = machine.disable_irq()
            try:
                count += 1
                count -= 1
                i += 1
            finally:
                machine.enable_irq(b)
    finally:
        machine.enable_irq(a)


def inc_count_multiple(times):
    for _ in range(times):
        inc_count()


def thread_entry(inc_times):
    global thread_done
    inc_count_multiple(inc_times)
    thread_done = True


_thread.start_new_thread(thread_entry, (1000,))
inc_count_multiple(1000)

time.sleep(1)

print("count", count, thread_done)
if count == 2000:
    print("PASS")
