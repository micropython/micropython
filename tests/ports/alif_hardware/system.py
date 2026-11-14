# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test basic system services: clocks, unique ID (SE services), tick counters,
# soft timers, RTC and backup SRAM.

import machine
import time

# Core clock, as configured by the run profile.
print("freq", machine.freq() in (400_000_000, 160_000_000))

# Unique ID comes from the SE, it must be stable and not all zeros.
uid = machine.unique_id()
print("unique_id", len(uid) == 8, uid != bytes(8), uid == machine.unique_id())

# Millisecond and microsecond ticks against sleep.
t0 = time.ticks_ms()
time.sleep_ms(200)
dt = time.ticks_diff(time.ticks_ms(), t0)
print("ticks_ms", 199 <= dt <= 210)

t0 = time.ticks_us()
time.sleep_us(5000)
dt = time.ticks_diff(time.ticks_us(), t0)
print("ticks_us", 4990 <= dt <= 6000)

# ticks_cpu is the cycle counter, so it must advance faster than ticks_us.
t0 = time.ticks_cpu()
time.sleep_ms(1)
print("ticks_cpu", time.ticks_diff(time.ticks_cpu(), t0) > 1000)

# Periodic and one-shot soft timers.
count = 0


def timer_cb(t):
    global count
    count += 1


tim = machine.Timer(mode=machine.Timer.PERIODIC, freq=100, callback=timer_cb)
time.sleep_ms(500)
tim.deinit()
print("timer_periodic", 45 <= count <= 55)

count = 0
tim = machine.Timer(mode=machine.Timer.ONE_SHOT, period=50, callback=timer_cb)
time.sleep_ms(200)
print("timer_one_shot", count == 1)

# RTC set, then check it advances.
rtc = machine.RTC()
rtc.datetime((2025, 1, 2, 3, 4, 5, 6, 0))
time.sleep_ms(2100)
dt = rtc.datetime()
print("rtc", dt[:6] == (2025, 1, 2, 3, 4, 5) and dt[6] in (8, 9))

# Backup SRAM is readable and writable.
mem = machine.mem_backup()
old = mem[0]
mem[0] = 0x12345678
print("mem_backup", mem[0] == 0x12345678)
mem[0] = old
