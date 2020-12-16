import utime
try:
    utime.sleep_ms
except AttributeError:
    print("SKIP")
    raise SystemExit

utime.sleep_ms(1)
utime.sleep_us(1)

t0 = utime.ticks_ms()
t1 = utime.ticks_ms()
print(0 <= utime.ticks_diff(t1, t0) <= 1)

t0 = utime.ticks_us()
t1 = utime.ticks_us()
print(0 <= utime.ticks_diff(t1, t0) <= 500)

# ticks_cpu may not be implemented, at least make sure it doesn't decrease
t0 = utime.ticks_cpu()
t1 = utime.ticks_cpu()
print(utime.ticks_diff(t1, t0) >= 0)
