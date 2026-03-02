from machine import RTC

print("*** RTC memory write test ***")
rtc = RTC()
expected = b"rtc_mem_psoc_edge"
print(
    "\nRTC memory retrieved post soft-reset is same as previously set : ",
    rtc.memory() == expected,
)
