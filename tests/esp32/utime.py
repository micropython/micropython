try:
    import sys, os, time
    from time import gmtime  # trigger exception if old utime

    MP = sys.implementation.name == "micropython"
except:
    print("SKIP")
    raise SystemExit

# print a time 9-tuple
def ptup(tup):
    print([i for i in tup])


# adjust a seconds-since-epoch timestamp to the MP 2000/1/1 epoch
def adj(secs):
    if sys.platform == "linux":
        secs -= 946684800
    return int(secs)


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
    (2016, 6, 1, 7, 0, 0, 0, 0, -1),  # PDT
]

# set time zones and test mktime/localtime/gmtime
for tz in [
    "PST+8PDT,M3.2.0/2,M11.1.0/2",
    "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00",
    "AEST-10AEDT-11,M10.5.0/02:00:00,M4.1.0/03:00:00",
]:
    print("---", tz)
    if MP:
        time.tzset(tz)
    else:
        os.environ["TZ"] = tz
        time.tzset()

    for st in SAMPLE_TIMES:
        t = time.mktime(st)
        print("-- ", st)
        print(adj(t))
        ptup(time.gmtime(t))
        ptup(time.localtime(t))

# test 8-tuple error
try:
    print(time.mktime((1, 2, 3, 4, 5, 6, 7, 8)))
except Exception as e:
    print("8-tuple exception")

## can't get mktime to produce an error...
# try:
#    print(int(time.mktime((2000, 99, 99, -99, -99, -99, 99, 99, 99))))
# except Exception as e:
#    print(e)
#
## can't get gmtime to produce an error...
# try:
#    ptup(time.gmtime(-2147483646))
# except Exception as e:
#    print(e)
