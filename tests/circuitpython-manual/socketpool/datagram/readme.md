# Circuitpython Datagrams

This example demonstrates using UDP (datagrams) with the Socket module, accessing the time from an NTP server using `sendto` and `recvfrom_into`.

## Prerequisites

Circuitpython V6.2.0 minimum.

## Expected behavior

The Circuitpython device will attempt to connect to wifi, and send a request for the time to `pool.ntp.org`. It will then convert the seconds returned into a unix time struct.

Expected output:
```
Sending packet
Received packet
Address: ('82.197.188.130', 31488)
Time: struct_time(tm_year=2021, tm_mon=2, tm_mday=11, tm_hour=22, tm_min=22, tm_sec=40, tm_wday=3, tm_yday=42, tm_isdst=-1)
```
