try:
    import usocket as socket
except:
    import socket
try:
    import ustruct as struct
except:
    import struct

# (date(2000, 1, 1) - date(1900, 1, 1)).days * 24*60*60
NTP_DELTA = 3155673600
MILLIS_PER_SECOND = 1000

host = "pool.ntp.org"


def time(ms_accuracy=False):
    NTP_QUERY = bytearray(48)
    NTP_QUERY[0] = 0x1b
    addr = socket.getaddrinfo(host, 123)[0][-1]
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(1)

    res = s.sendto(NTP_QUERY, addr)
    msg = s.recv(48)
    s.close()

    sec = struct.unpack("!I", msg[40:44])[0] - NTP_DELTA
    frac = struct.unpack("!I", msg[44:48])[0] * MILLIS_PER_SECOND >> 32
    if ms_accuracy:
        return sec, frac
    return sec


# There's currently no timezone support in MicroPython, so
# utime.localtime() will return UTC time (as if it was .gmtime())
def settime(ms_accuracy=False):
    import machine
    import utime
    if ms_accuracy:
        t, ms = time(ms_accuracy=True)
    else:
        t, ms = time(ms_accuracy=False), 0
    tm = utime.localtime(t)
    tm = tm[0:3] + (0,) + tm[3:6] + (ms,)
    r = machine.RTC()
    r.datetime(tm)
    print(r.datetime())
