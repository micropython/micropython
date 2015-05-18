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

def spot_test(seconds, expected_time):
    actual_time = time.localtime(seconds)
    for i in range(len(actual_time)):
        if actual_time[i] != expected_time[i]:
            print("time.localtime(", seconds, ") returned", actual_time, "expecting", expected_time)
            return
    print("time.localtime(", seconds, ") returned", actual_time, "(pass)")


test()
spot_test(          0,  (2000,  1,  1,  0,  0,  0, 5,   1))
spot_test(          1,  (2000,  1,  1,  0,  0,  1, 5,   1))
spot_test(         59,  (2000,  1,  1,  0,  0, 59, 5,   1))
spot_test(         60,  (2000,  1,  1,  0,  1,  0, 5,   1))
spot_test(       3599,  (2000,  1,  1,  0, 59, 59, 5,   1))
spot_test(       3600,  (2000,  1,  1,  1,  0,  0, 5,   1))
spot_test(         -1,  (1999, 12, 31, 23, 59, 59, 4, 365))
spot_test(  447549467,  (2014,  3,  7, 23, 17, 47, 4,  66))
spot_test( -940984933,  (1970,  3,  7, 23, 17, 47, 5,  66))
spot_test(-1072915199,  (1966,  1,  1,  0,  0,  1, 5,   1))
spot_test(-1072915200,  (1966,  1,  1,  0,  0,  0, 5,   1))
spot_test(-1072915201,  (1965, 12, 31, 23, 59, 59, 4, 365))
