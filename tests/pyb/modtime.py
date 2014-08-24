import time

DAYS_PER_MONTH = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

def is_leap(year):
    return (year % 4) == 0

def test():
    seconds = 0
    wday = 5    # Jan 1, 2000 was a Saturday
    for year in range(2000, 2034):
        print("Testing %d" % year)
        yday = 1
        for month in range(1, 13):
            if month == 2 and is_leap(year):
                DAYS_PER_MONTH[2] = 29
            else:
                DAYS_PER_MONTH[2] = 28
            for day in range(1, DAYS_PER_MONTH[month] + 1):
                secs = time.mktime((year, month, day, 0, 0, 0, 0, 0))
                if secs != seconds:
                    print("mktime failed for %d-%02d-%02d got %d expected %d" % (year, month, day, secs, seconds))
                tuple = time.localtime(seconds)
                secs = time.mktime(tuple)
                if secs != seconds:
                    print("localtime failed for %d-%02d-%02d got %d expected %d" % (year, month, day, secs, seconds))
                    return
                seconds += 86400
                if yday != tuple[7]:
                    print("locatime for %d-%02d-%02d got yday %d, expecting %d" % (year, month, day, tuple[7], yday))
                    return
                if wday != tuple[6]:
                    print("locatime for %d-%02d-%02d got wday %d, expecting %d" % (year, month, day, tuple[6], wday))
                    return
                yday += 1
                wday = (wday + 1) % 7

test()
