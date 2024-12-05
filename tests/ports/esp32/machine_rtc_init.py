# Test init behaviour of machine.RTC.

try:
    from machine import RTC
except ImportError:
    print("SKIP")
    raise SystemExit

rtc = RTC()

year = 2024
month = 12
day = 26
# weekday is 3 (Thursday)
hour = 6
minute = 30
second = 20
microsecond = 0
tzinfo = 0

# Save datetime.
orig_datetime = rtc.datetime()

# Set datetime to a known value using init.
rtc.init((year, month, day, hour, minute, second, microsecond, tzinfo))
now = rtc.datetime()
print(now[0], now[1], now[2], now[3], now[4], now[5], now[6])

# Test with date only.
rtc.init((year, month, day))
print(now[0], now[1], now[2], now[3])

# Test with too many arguments.
try:
    rtc.init((year, month, day, hour, minute, second, microsecond, tzinfo, 1234))
except ValueError as e:
    print(e)

# Test with too few arguments.
try:
    rtc.init((year, month))
except ValueError as e:
    print(e)

# Restore datetime.
rtc.datetime(orig_datetime)
