import time

wday = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]


def get_default_date_time():
    seconds = 0
    default_dtime = time.gmtime(0)
    print(
        "\ngmtime() at default returned [Year/Month/Day - WeekDay - HH:MM:SS] : %d/%02d/%02d - %s - %02d:%02d:%02d "
        % (
            default_dtime[0],
            default_dtime[1],
            default_dtime[2],
            wday[default_dtime[6]],
            default_dtime[3],
            default_dtime[4],
            default_dtime[5],
        )
    )

    if time.mktime(default_dtime) != seconds:
        print("\nmktime() FAILED to create right dtime from secs\n")
    else:
        print("\nmktime() SUCCESSFULLY created right dtime from secs\n")


def get_dtime_from_secs(secs):
    return time.localtime(secs)


def spot_test(seconds, expected_time):
    actual_time = time.localtime(seconds)
    for i in range(len(actual_time)):
        if actual_time[i] != expected_time[i]:
            print(
                "time.localtime(",
                seconds,
                ") returned",
                actual_time,
                "expecting",
                expected_time,
                "(Test : FAILED)",
            )
            return
    print("time.localtime(", seconds, ") returned", actual_time, "(Test : PASSED)")


get_default_date_time()

spot_test(0, (1970, 1, 1, 0, 0, 0, 3, 1))
spot_test(1, (1970, 1, 1, 0, 0, 1, 3, 1))
spot_test(59, (1970, 1, 1, 0, 0, 59, 3, 1))
spot_test(60, (1970, 1, 1, 0, 1, 0, 3, 1))
spot_test(3599, (1970, 1, 1, 0, 59, 59, 3, 1))
spot_test(3600, (1970, 1, 1, 1, 0, 0, 3, 1))
spot_test(-1, (1969, 12, 31, 23, 59, 59, 2, 365))
spot_test(447549467, (1984, 3, 7, 23, 17, 47, 2, 67))
spot_test(-940984933, (1940, 3, 7, 23, 17, 47, 3, 67))
spot_test(-1072915199, (1936, 1, 2, 0, 0, 1, 3, 2))
spot_test(-1072915200, (1936, 1, 2, 0, 0, 0, 3, 2))
spot_test(-1072915201, (1936, 1, 1, 23, 59, 59, 2, 1))
