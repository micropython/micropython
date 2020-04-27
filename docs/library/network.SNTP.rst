.. currentmodule:: network
.. _network.SNTP:

class SNTP -- Simple Network Time Protocol
==========================================

This class provides an implementation of the standard SNTP protocol.

    from network import SNTP
    import time
    # start SNTP to run in the background
    sntp = SNTP()
    sntp.start()
    # check the status
    while sntp.status != sntp.INSYNC:
        time.sleep(1)
    print("Time is synchronized()")

Constructors
------------
.. class:: SNTP()

Returns the SNTP singleton object.

Methods
-------

.. method:: SNTP.start(server=None, poll=3600, smooth=600)

    Starts the SNTP process to run in the background.
    The keyword parameters are:
    * server: the hostname of the NTP server to query, a value of None causes the
      server (if any) provided via DHCP to be used. If DHCP doesn't provide a server
      then `status()` will be stuck in `SNTP.NOSRV`.
    * poll: interval at which the SNTP server should be polled in seconds;
      a value of zero switches to passive mode listening for NTP broadcasts.
    * smooth: a time difference threshold in milliseconds below which the system
      clock should be adjusted smoothly using ``time.adjtime()``. Above the threshold
      the system clock is stepped.

.. method:: SNTP.stop()

    Stops the SNTP process. The system clock continues to run but is no
    longer synchronized.

.. method:: SNTP.status()

    Returns the number of seconds since the last server response. If the
    return value is negative it indicates one of the following error conditions:
    * SNTP.STOPPED - SNTP is not running
    * SNTP.NOIP - SNTP does not have an IP address, e.g. DNS lookup failed
    * SNTP.NOSRV - No SNTP server has been provided by DHCP
    * SNTP.UNREACHABLE - the SNTP server has not responded to the last eight requests

    If the return value is positive the time may still not be truly in-synch if
    smooth synchronization has been requested in `start()`. The clock adjustment
    state can be queried via ``time.adjtime()``.
