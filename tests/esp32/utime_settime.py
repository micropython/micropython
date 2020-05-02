try:
    import sys, time
    from time import adjtime  # trigger exception if old utime
except:
    print("SKIP")
    raise SystemExit

# print a time 9-tuple
def ptup(tup):
    print([i for i in tup])


SAMPLE_TIMES = [
    (2000, 1, 1, 1, 0, 0, 0, 0, -1),
    (2000, 1, 31, 1, 0, 0, 0, 0, -1),
    (2000, 12, 31, 1, 0, 0, 0, 0, -1),
    (2016, 12, 31, 1, 0, 0, 0, 0, -1),
    (2016, 12, 31, 7, 0, 0, 0, 0, -1),
    (2016, 12, 31, 7, 1, 0, 0, 0, -1),
    (2016, 12, 31, 7, 12, 0, 0, 0, -1),
    (2016, 12, 31, 7, 13, 0, 0, 0, -1),
    (2016, 12, 31, 7, 23, 0, 0, 0, -1),
    (2016, 12, 31, 7, 23, 1, 0, 0, -1),
    (2016, 12, 31, 7, 23, 59, 0, 0, -1),
    (2016, 12, 31, 7, 23, 59, 1, 0, -1),
    (2016, 12, 31, 7, 23, 59, 59, 0, -1),
    (2037, 12, 31, 7, 23, 59, 59, 0, -1),
]

# set time zone, set time, then read time and expect it to be as set
# for tz in [
# "PST+8PDT,M3.2.0/2,M11.1.0/2",
# "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00",
# "AEST-10AEDT-11,M10.5.0/02:00:00,M4.1.0/03:00:00",
# ]:
# print("---", tz)
# time.tzset(tz)

# for st in SAMPLE_TIMES:
# ptup(st)
# t = time.mktime(st)
# print(" ", int(t))
# time.settime(t)
# print("  ", end="")
# ptup(time.gmtime())
# print("  ", end="")
# ptup(time.localtime())

# set time zone, set time, set adjtime, expect it to report change back
for adj in [1000, 0, -1000, 1000000, -1000000, 2300000, -2800000]:
    print("---", adj)
    time.tzset("GMT+0")
    time.settime(time.mktime((2020, 4, 1, 10, 23, 45, 0, 0, 0)), 0)
    ptup(time.localtime())
    oadj = time.adjtime(adj)
    print(oadj)
    oadj = time.adjtime(adj)
    print(adj == 0 or abs(oadj) < abs(adj))
    time.sleep_ms(400)
    oadj = time.adjtime(0)
    # print(oadj)
    print(adj == 0 or abs(oadj) < abs(adj))
    ptup(time.localtime())

try:
    oadj = time.adjtime(0x7FFFFFFF)
    print(oadj)
except ValueError as e:
    print(e)

# verify that settime and time_us handle sub-second precision
t1 = time.mktime((2020, 4, 1, 10, 23, 45, 0, 0, 0))
time.settime(t1, 500000)
td = time.time_us() - t1 * 1000000 - 500000
print("frac settime", td > 0 and td < 600000)
