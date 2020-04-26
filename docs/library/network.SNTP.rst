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

.. method:: SNTP.start(servers=["pool.ntp.org"], poll=True, slew=True)

    Starts the SNTP process to run in the background.
    The keyword parameters are:
    * servers: a list of NTP servers to query, the max number is implementation
      dependent.
    * poll: whether to explicitly poll NTP servers (True) or passively listen
      for NTP broadcasts
    * slew: whether to gradually adjust the time using adjtime (True) or
      step the time (False). (If the time difference is large the system
      time may be stepped even if slew==True.)

.. method:: SNTP.stop()

    Stops the SNTP process. The system clock continues to run but is no
    longer synchronized.

.. method:: SNTP.status()

    Returns the current clock synchronization status: (semantics not yet clear)
    * SNTP.STOPPED
    * SNTP.INSYNC
    * SNTP.ADJUSTING
