*****************************************************
:mod:`ntptime` --- Network Time Protocol (NTP) Client
*****************************************************

.. module:: ntptime
   :synopsis: Network Time Protocol (NTP) Client

This module provides a very simple SNTP client.  It fetches the current time
from a server on the Internet, or from a server on a local network.

To use this module, a MicroPython variant/build with network capabilities must
be installed, and the ntptime module must be enabled.

For details of the SNTP protocol, see RFC 4330.

All times are in UTC.  There is no timezone support in this module, although
you could apply your own timezone adjustment to the returned UTC times.

This module only provides time with 1-second resolution.

This module does not make any attempt to account for network latencies.
On a normal internet connection, this won't matter, especially given the
1 second resolution.  On a high-latency internet connection, you will have to
increase the default 1 second request timeout for this module to work at all,
and then the time will be slightly wrong due to the time the response takes to
travel from the NTP server to this client.

There is no special handling of leap seconds.  This probably means that time
will jump by 1 second when a leap second happens.  But some NTP servers may
do "leap smearing" instead.  Given that the resolution is only 1 second
anyway, leap seconds are unlikely to cause any extra issues.

There is no security whatsoever.  An attacker who wants to change the time
of your system, can send forged responses to your system.  If any of them
arrive after the NTP request was sent, but before the genuine NTP response
is received, then the time set by the attacker will be used.  Note that for
Internet-facing devices, the attacker does not even need to spoof their IP
address for this attack to work, this module does not check the sender of the
packet in any way.  A typical home NAT firewall may make the attack slightly
harder, requiring an attacker to spoof their IP address, but attackers can
do that.

This module does not do any sanity checking on the returned time to detect
faulty or malicious NTP servers.

This module does not support the "Kiss-o'-Death Packet" defined in RFC4330.

This module does not suffer from the Y2036 bug.  It supports timestamps up
to and including the year 2159.  It does suffer from a Y2160 bug, which will
need fixing closer to that time, if MicroPython is still being used.

For example::

    # Print the current time, as seconds since the epoch.
    import ntptime
    print(ntptime.time())

    # Set the platfom's RTC 
    ntptime.settime()

    # Print the current time
    import time
    print(time.gmtime())


Functions
=========

.. function:: time() -> int

    Query the configured NTP server for the current time, and returns that time.

    The returned value is seconds since the Epoch, where the Epoch is as defined
    in the MicroPython :mod:`time` module.

    Raises an exception if the configured NTP server hostname cannot be resolved.

    Raises :exc:`OSError` if the SNTP request times out.

    Raises an exception if the SNTP response is too short.

.. function:: settime()

    Query the configured NTP server for the current time, and set the platform's
    clock to that time.

    This sets the clock using :class:`machine.RTC`.

.. data:: host
    :type: str
    :value: "pool.ntp.org"

    The hostname of the NTP server to use.

    May be set by applications.

    For example, may be set to an NTP server provided by your ISP, or one that
    is on the local network.

    For commercial products, you should set this to your assigned NTP hostname,
    you should not use the default.  You need to contact the NTP Pool to get
    permission to use their servers, and to get your own NTP hostname assigned.
    See the NTP Pool's
    `Information for vendors <https://www.ntppool.org/en/vendors.html>`_

.. data:: timeout
    :type: float | int
    :value: 1

    The timeout period for SNTP requests, in seconds.

    May be changed by applications.  The default is 1 second.
