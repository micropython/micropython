from Tiger import RTC1


rtc = RTC1(True)
now = rtc.datetime()
print(now)
rtc.setAlarm([now[0], now[1] + 1, now[4]])