# Basic TIMEZONE, NTP_SERVER mod
# https://github.com/micropython/micropython/blob/master/ports/esp8266/modules/ntptime.py

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

# The NTP host can be configured at runtime by doing: ntptime.host = 'myhost.org'
DEFAULT_HOST = "pool.ntp.org"
DEFAULT_TZONE = "CET"

NTP_LI_ClockUnsyncronized = 0b11000000
NTP_VN_Version4           = 0b00010000
NTP_PacketMode_Client     = 0b00000011

def time(host = DEFAULT_HOST):
    NTP_QUERY = bytearray(48)
    NTP_QUERY[0] = NTP_LI_ClockUnsyncronized | NTP_VN_Version4 | NTP_PacketMode_Client
    addr = socket.getaddrinfo(host, 123)[0][-1]
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.settimeout(1)
        res = s.sendto(NTP_QUERY, addr)
        msg = s.recv(48)
    finally:
        s.close()
    val = struct.unpack("!I", msg[40:44])[0]
    return val - NTP_DELTA


def settime( timezone = DEFAULT_TZONE, host = DEFAULT_HOST):
    t = time(host)
    import machine
    import utime

    if timezone == 'CET':
        tzonedelta = 1 # CET delta
    elif timezone[:4] in ["GMT+","GMT-"]:
        tzonedelta = eval(timezone[3:])
    else:
        tzonedelta = 0
    tm = utime.gmtime(t)
    hour = tm[3]+tzonedelta  #RTC().datetime() handles well -hours and 24+ hours
    #RTC().datetime( (year, month, day, weekday, hours, minutes, seconds, subseconds) )
    #weekday is 0-6 for Mon-Sun - like in utime.gmtime()
    machine.RTC().datetime((tm[0], tm[1], tm[2], tm[6], hour, tm[4], tm[5], 0))
