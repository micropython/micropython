from machine import RTC

print("*** RTC memory write test ***")
rtc = RTC()
print(
    "\ndatetime retrieved post soft-reset is same as previously set : ",
    rtc.memory() <= (2023, 1, 1, 0, 0, 2, 0, 0),
)
