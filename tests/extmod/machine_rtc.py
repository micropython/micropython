# Test basic behaviour of machine.RTC.

try:
    from machine import RTC
except ImportError:
    print("SKIP")
    raise SystemExit

rtc = RTC()

# Save datetime.
orig_datetime = rtc.datetime()

# Set datetime to a known value.
rtc.datetime((2020, 1, 1, 0, 0, 0, 0, 0))

# Check that getting and setting the datetime give expected values.
for i in range(4):
    now = rtc.datetime()
    print(now[0], now[1], now[2], now[4], now[5])
    rtc.datetime(now)

# Restore datetime.
rtc.datetime(orig_datetime)
