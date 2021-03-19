import pyb, stm
from pyb import RTC

rtc = RTC()
rtc.init()
print(rtc)

# make sure that 1 second passes correctly
rtc.datetime((2014, 1, 1, 1, 0, 0, 0, 0))
pyb.delay(1002)
print(rtc.datetime()[:7])


def set_and_print(datetime):
    rtc.datetime(datetime)
    print(rtc.datetime()[:7])


# make sure that setting works correctly
set_and_print((2000, 1, 1, 1, 0, 0, 0, 0))
set_and_print((2000, 1, 31, 1, 0, 0, 0, 0))
set_and_print((2000, 12, 31, 1, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 1, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 1, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 12, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 13, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 1, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 1, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 59, 0))
set_and_print((2099, 12, 31, 7, 23, 59, 59, 0))

# check that calibration works correctly
# save existing calibration value:
cal_tmp = rtc.calibration()


def set_and_print_calib(cal):
    rtc.calibration(cal)
    print(rtc.calibration())


set_and_print_calib(512)
set_and_print_calib(511)
set_and_print_calib(345)
set_and_print_calib(1)
set_and_print_calib(0)
set_and_print_calib(-1)
set_and_print_calib(-123)
set_and_print_calib(-510)
set_and_print_calib(-511)

# restore existing calibration value
rtc.calibration(cal_tmp)

# Check register settings for wakeup
def set_and_print_wakeup(ms):
    try:
        rtc.wakeup(ms)
        wucksel = stm.mem32[stm.RTC + stm.RTC_CR] & 7
        wut = stm.mem32[stm.RTC + stm.RTC_WUTR] & 0xFFFF
    except ValueError:
        wucksel = -1
        wut = -1
    print((wucksel, wut))


set_and_print_wakeup(0)
set_and_print_wakeup(1)
set_and_print_wakeup(4000)
set_and_print_wakeup(4001)
set_and_print_wakeup(8000)
set_and_print_wakeup(8001)
set_and_print_wakeup(16000)
set_and_print_wakeup(16001)
set_and_print_wakeup(32000)
set_and_print_wakeup(32001)
set_and_print_wakeup(0x10000 * 1000)
set_and_print_wakeup(0x10001 * 1000)
set_and_print_wakeup(0x1FFFF * 1000)
set_and_print_wakeup(0x20000 * 1000)
set_and_print_wakeup(0x20001 * 1000)  # exception
