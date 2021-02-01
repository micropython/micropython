try:
    import utime as time
except ImportError:
    import time

DAYS_PER_MONTH = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

tzseconds = -time.mktime((1970, 1, 1, 14, 0, 0, 0, 0, 0))


def is_leap(year):
    return (year % 4) == 0


def test():
    seconds = 0
    wday = 3  # Jan 1, 1970 was a Thursday
    for year in range(1970, 2038):
        print("Testing %d" % year)
        yday = 1
        for month in range(1, 13):
            if month == 2 and is_leap(year):
                DAYS_PER_MONTH[2] = 29
            else:
                DAYS_PER_MONTH[2] = 28
            for day in range(1, DAYS_PER_MONTH[month] + 1):
                secs = time.mktime((year, month, day, 14, 0, 0, 0, 0, 0)) + tzseconds
                if secs != seconds:
                    print(
                        "mktime failed for %d-%02d-%02d got %d expected %d"
                        % (year, month, day, secs, seconds)
                    )
                    return
                tuple = time.localtime(seconds)
                secs = time.mktime(tuple)
                if secs != seconds:
                    print(
                        "localtime failed for %d-%02d-%02d got %d expected %d"
                        % (year, month, day, secs, seconds)
                    )
                    return
                seconds += 86400
                if yday != tuple[7]:
                    print(
                        "locatime for %d-%02d-%02d got yday %d, expecting %d"
                        % (year, month, day, tuple[7], yday)
                    )
                    return
                if wday != tuple[6]:
                    print(
                        "locatime for %d-%02d-%02d got wday %d, expecting %d"
                        % (year, month, day, tuple[6], wday)
                    )
                    return
                yday += 1
                wday = (wday + 1) % 7


test()
