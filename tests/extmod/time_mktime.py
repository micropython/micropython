# test conversion from date tuple to timestamp and back

try:
    import time

    time.localtime
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Range of date expected to work on all MicroPython platforms
MIN_YEAR = 1970
MAX_YEAR = 2099
# CPython properly supported date range:
# - on Windows: year 1970 to 3000+
# - on Unix:    year 1583 to 3000+

# Start test from Jan 1, 2001 13:00 (Feb 2000 might already be broken)
SAFE_DATE = (2001, 1, 1, 13, 0, 0, 0, 0, -1)


# mktime function that checks that the result is reversible
def safe_mktime(date_tuple):
    try:
        res = time.mktime(date_tuple)
        chk = time.localtime(res)
    except OverflowError:
        print("safe_mktime:", date_tuple, "overflow error")
        return None
    if chk[0:5] != date_tuple[0:5]:
        print("safe_mktime:", date_tuple[0:5], " -> ", res, " -> ", chk[0:5])
        return None
    return res


# localtime function that checks that the result is reversible
def safe_localtime(timestamp):
    try:
        res = time.localtime(timestamp)
        chk = time.mktime(res)
    except OverflowError:
        print("safe_localtime:", timestamp, "overflow error")
        return None
    if chk != timestamp:
        print("safe_localtime:", timestamp, " -> ", res, " -> ", chk)
        return None
    return res


# look for smallest valid timestamps by iterating backwards on tuple
def test_bwd(date_tuple):
    curr_stamp = safe_mktime(date_tuple)
    year = date_tuple[0]
    month = date_tuple[1] - 1
    if month < 1:
        year -= 1
        month = 12
    while year >= MIN_YEAR:
        while month >= 1:
            next_tuple = (year, month) + date_tuple[2:]
            next_stamp = safe_mktime(next_tuple)
            # at this stage, only test consistency and monotonicity
            if next_stamp is None or next_stamp >= curr_stamp:
                return date_tuple
            date_tuple = next_tuple
            curr_stamp = next_stamp
            month -= 1
        year -= 1
        month = 12
    return date_tuple


# test day-by-day to ensure that every date is properly converted
def test_fwd(start_date):
    DAYS_PER_MONTH = (0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31)
    curr_stamp = safe_mktime(start_date)
    curr_date = safe_localtime(curr_stamp)
    while curr_date[0] <= MAX_YEAR:
        if curr_date[2] < 15:
            skip_days = 13
        else:
            skip_days = 1
        next_stamp = curr_stamp + skip_days * 86400
        next_date = safe_localtime(next_stamp)
        if next_date is None:
            return curr_date
        if next_date[2] != curr_date[2] + skip_days:
            # next month
            if next_date[2] != 1:
                print("wrong day of month:", next_date)
                return curr_date
            # check the number of days in previous month
            month_days = DAYS_PER_MONTH[curr_date[1]]
            if month_days == 28 and curr_date[0] % 4 == 0:
                if curr_date[0] % 100 != 0 or curr_date[0] % 400 == 0:
                    month_days += 1
            if curr_date[2] != month_days:
                print("wrong day count in prev month:", curr_date[2], "vs", month_days)
                return curr_date
            if next_date[1] != curr_date[1] + 1:
                # next year
                if curr_date[1] != 12:
                    print("wrong month count in prev year:", curr_date[1])
                    return curr_date
                if next_date[1] != 1:
                    print("wrong month:", next_date)
                    return curr_date
                if next_date[0] != curr_date[0] + 1:
                    print("wrong year:", next_date)
                    return curr_date
        curr_stamp = next_stamp
        curr_date = next_date
    return curr_date


small_date = test_bwd(SAFE_DATE)
large_date = test_fwd(small_date)
print("tested from", small_date[0:3], "to", large_date[0:3])
print(small_date[0:3], "wday is", small_date[6])
print(large_date[0:3], "wday is", large_date[6])
