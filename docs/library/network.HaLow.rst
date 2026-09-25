.. currentmodule:: network
.. _network.HaLow:

class HaLow -- control 802.11ah (Wi-Fi HaLow) network interfaces
================================================================

This class controls 802.11ah (Wi-Fi HaLow) transceivers.  802.11ah is sub-1 GHz
Wi-Fi: much longer range and much lower power than 2.4GHz Wi-Fi, at a fraction
of the throughput.  It carries ordinary Ethernet frames, so sockets, DHCP and
DNS all work as they do on :class:`network.WLAN`.

``network.HaLow`` is the single class for every supported transceiver family;
the underlying driver ("PHY") is chosen with the ``phy`` argument to the
constructor.  The Morse Micro MM6108/MM8108 driver is currently the only PHY, so
it is the default and ``phy`` may be omitted.  When a firmware image is built
with more than one PHY, ``phy`` must be given to say which one to use.

Example usage::

    import network
    # The channel plan is set by regulation and there is no worldwide default,
    # so the country must be set before the interface is brought up.
    network.country('US')

    nic = network.HaLow(network.HaLow.IF_STA)
    nic.active(True)
    nic.connect('your-ssid', 'your-passphrase')
    while not nic.isconnected():
        pass
    print(nic.ifconfig())

Constructors
------------

.. class:: HaLow(interface_id=HaLow.IF_STA, phy=HaLow.MORSE_MICRO)

    Create a HaLow network interface object.  The only supported interface is
    ``network.HaLow.IF_STA`` (station), which is the default.

    *phy* selects the transceiver driver, as one of the ``HaLow.<PHY>``
    constants (e.g. ``HaLow.MORSE_MICRO``).  If the firmware was built with a
    single PHY it is the default and *phy* may be omitted; if it was built with
    more than one, *phy* is required.  Passing a PHY that was not built in
    raises ``ValueError``.

Methods
-------

.. method:: HaLow.active([is_active])

    Query or set whether the interface is active.  Bringing the station
    interface up powers on the transceiver, which takes a moment: the firmware
    image is loaded over the bus before the interface is usable.

    Raises ``ValueError`` if no country has been set; see
    :func:`network.country`.

.. method:: HaLow.connect(ssid, key=None, *, security=None, bssid=None)

    Connect to a network.  ``security`` is one of the ``HaLow.SEC_*`` constants;
    when it is not given, an open network is assumed if no key is supplied and
    ``SEC_SAE`` otherwise.  802.11ah has no WPA2-PSK, so a passphrase always
    means SAE (WPA3).

.. method:: HaLow.disconnect()

    Disconnect from the currently connected network.

.. method:: HaLow.deinit()

    Deactivate the interface and release the memory the driver allocated.

.. method:: HaLow.scan()

    Scan for available networks.  Only usable on ``IF_STA``.

    Sweeps every channel in the regulatory domain and returns what it found, up
    to 32 networks; a sweep that sees more drops the rest.  It takes around
    5 seconds in the United States.  Most of that is dwell time:
    the sweep waits longer than a beacon interval on each channel, because a
    shorter visit misses networks that are plainly there.  Returns an empty list
    while a connection is in progress or established, as the radio cannot scan
    and hold a link at the same time.

    Fails with ``EPERM`` when power saving is enabled; see ``HaLow.PM_NONE``.

    Returns a list of tuples with the fields
    ``(ssid, bssid, channel, rssi, security, hidden)``, where ``security`` is one
    of the ``SEC_*`` constants.

    ``channel`` is the S1G channel the beacon was *received* on, and is ``0``
    for an access point operating outside the local regulatory plan.  An access
    point using a wide channel beacons on its primary channel so that narrowband
    stations can hear it, so this is not its operating channel: an 8 MHz access
    point whose management page reports 916 MHz is seen here on its primary
    channel, 1 MHz lower.  Use ``config('channel')`` and ``config('bandwidth')``
    once associated to read the operating channel itself.

.. method:: HaLow.status([param])

    With no argument, return the link status, one of the ``STAT_*`` constants.
    With
    ``'rssi'``, return the signal strength of the connected AP in dBm; with
    ``'bssid'``, its BSSID; with ``'duty_cycle'``, the regulatory duty cycle as
    ``(duty_cycle, mode, burst_remaining_us, burst_window_us)`` where
    ``duty_cycle`` is in hundredths of a percent; with ``'rates'``, the rate
    control table as a list of ``(mcs, bandwidth_mhz, gi, sent, success)``,
    covering only the entries that have been used.

.. method:: HaLow.isconnected()

    Returns ``True`` when connected to an AP *and* an IP address has been
    obtained.

.. method:: HaLow.ifconfig([(ip, subnet, gateway, dns)])

    See :meth:`network.WLAN.ifconfig`.

.. method:: HaLow.ipconfig('param')
            HaLow.ipconfig(param=value, ...)

    See :meth:`network.AbstractNIC.ipconfig`.

.. method:: HaLow.config('param')
            HaLow.config(param=value, ...)

    Get or set general network interface parameters:

    ==================  ========================================================
    Parameter           Description
    ==================  ========================================================
    mac                 MAC address (bytes), read only
    version             ``(morselib, firmware, chip)`` version strings, read
                        only; fails with ``ENODEV`` before the interface is
                        active
    ssid                network name of the connected network (string), read
                        only
    channel             S1G channel number the interface is operating on, read
                        only
    bandwidth           width of the primary channel in MHz, read only
    hostname            the hostname used by DHCP (deprecated, use
                        :func:`network.hostname`)
    pm                  power management mode, one of the ``PM_*`` constants
    ps_timeout          time in ms the transceiver stays awake after activity
                        before dozing again; only has an effect when *pm* is
                        ``PM_POWERSAVE``
    txpower             maximum transmit power in dBm
    ampdu               whether frame aggregation is enabled (default ``True``)
    sgi                 whether the short guard interval is enabled (default
                        ``True``)
    subbands            whether subband operation is enabled (default ``True``)
    rts_threshold       RTS threshold in octets, ``0`` to disable
    fragment_threshold  fragmentation threshold in octets, ``0`` to disable
    listen_interval     beacons the station may sleep for between wakes, which
                        the access point uses to size how long it buffers
                        frames; ``0`` disables
    duty_cycle          how permitted air time is spread, ``DUTY_CYCLE_SPREAD``
                        or ``DUTY_CYCLE_BURST``.  How much of the allowance is
                        left is reported by ``status('duty_cycle')``
    twt                 ``(interval_us, duration_us[, setup])``, write only.
                        Asks the access point for a Target Wake Time agreement:
                        awake for *duration_us* every *interval_us* rather than
                        at every DTIM. *setup* is ``TWT_REQUEST``,
                        ``TWT_SUGGEST`` or ``TWT_DEMAND``. Carried in the
                        association request, so it fails with ``EPERM`` once
                        associated
    wnm_sleep           write only. Enter or leave WNM sleep, in which the
                        transceiver sleeps across many DTIM periods and the
                        access point buffers traffic for it. Requires
                        ``PM_POWERSAVE`` and an established connection. Do not
                        transmit while asleep
    wnm_powerdown       whether entering WNM sleep also powers the transceiver
                        down, which uses the least power but takes longer to
                        wake
    health_check        ``(min_ms, max_ms)``, write only. How often the driver
                        checks the transceiver is responding; each check wakes
                        it
    fixed_rate          ``(mcs, bandwidth, gi)``, write only. Pins the transmit
                        rate so emissions can be measured at a known modulation.
                        *mcs* is 0 to 9, *bandwidth* is 1, 2, 4 or 8 MHz, *gi*
                        is 0 for short or 1 for long.  ``(-1, -1, -1)``
                        restores automatic selection
    trace               diagnostic trace bitmask, write only.  Bits: ``1`` async
                        events (association state, link up/down, fatal errors),
                        ``2`` transmitted frames, ``4`` received frames, ``8``
                        full frame contents, ``16`` MAC operations.  ``0`` (the
                        default) is off.  Output goes to the REPL, so enable
                        ``1`` before ``connect()`` to see why an association does
                        not complete
    ==================  ========================================================

    ``ampdu``, ``sgi``, ``subbands`` and ``listen_interval`` are only accepted
    by the transceiver while it is not associated -- the last is carried in the
    association request -- so setting them on a connected interface takes effect
    the next time it associates.

    .. note::

       Only station mode is currently supported.  Access-point operation --
       and with it setting ``ssid`` and ``channel`` -- is not available.

Regulatory testing
------------------

.. method:: HaLow.ioctl(cmd, buf)

    Execute a vendor command.  ``buf`` holds the command on entry and receives
    the response, so it must be a writable buffer large enough for both; the
    length of the response is returned.  ``cmd`` is reserved and should be
    ``None``, as the transceiver's test interface has no command code.

    The command format is defined by Morse Micro's tooling rather than here.
    Intended for certification work, where it is used together with the
    ``fixed_rate`` configuration option.


Constants
---------

.. data:: HaLow.IF_STA

    Interface identifier, for the constructor.

.. data:: HaLow.MORSE_MICRO

    PHY (transceiver driver) identifier, for the constructor's ``phy``
    argument.  ``MORSE_MICRO`` selects the Morse Micro MM6108/MM8108 driver.
    A constant is present only for a PHY that was built into the firmware, so
    the available identifiers depend on the build (``MORSE_MICRO`` is the only
    one at present).

.. data:: HaLow.SEC_OPEN
          HaLow.SEC_OWE
          HaLow.SEC_SAE
          HaLow.SEC_WPA3

    Security modes.  ``SEC_OWE`` is opportunistic wireless encryption
    (unauthenticated but encrypted) and ``SEC_SAE`` is WPA3; ``SEC_WPA3`` is an
    alias for ``SEC_SAE``.  802.11ah does not define WPA2-PSK.

.. data:: HaLow.TWT_REQUEST
          HaLow.TWT_SUGGEST
          HaLow.TWT_DEMAND

    How firmly a Target Wake Time agreement is asked for.

.. data:: HaLow.DUTY_CYCLE_SPREAD
          HaLow.DUTY_CYCLE_BURST

    Duty cycle modes.  Where regulation limits air time, ``DUTY_CYCLE_SPREAD``
    spreads it evenly and ``DUTY_CYCLE_BURST`` makes it available in bursts.
    Regions without a limit are unaffected.

.. data:: HaLow.STAT_IDLE
          HaLow.STAT_CONNECTING
          HaLow.STAT_NOIP
          HaLow.STAT_GOT_IP
          HaLow.STAT_CONNECT_FAIL

    Link states, as returned by :meth:`HaLow.status()`.  ``STAT_NOIP`` means
    associated but without an address yet; :meth:`HaLow.isconnected()` is only
    true at ``STAT_GOT_IP``.

    .. warning::

       There is no state for "wrong passphrase" or "no such network".  The
       transceiver reports only connecting, connected and disabled, and it
       retries a failed association indefinitely with a growing backoff, so
       :meth:`HaLow.status()` alternates between ``STAT_CONNECTING`` and
       ``STAT_IDLE`` for as long as it keeps trying.  ``STAT_CONNECT_FAIL``
       means the transceiver itself failed, not that the join was refused.

       Enforce your own deadline, and to work out *why* it is not joining,
       :meth:`HaLow.disconnect()` first and then :meth:`HaLow.scan()` --
       scanning returns an empty list while a join is in progress::

           nic.connect(ssid, key)
           start = time.ticks_ms()
           while not nic.isconnected():
               if time.ticks_diff(time.ticks_ms(), start) > 30000:
                   nic.disconnect()
                   seen = [n[0] for n in nic.scan()]
                   raise OSError("not found" if ssid.encode() not in seen
                                 else "found, but would not associate")
               time.sleep_ms(100)

.. data:: HaLow.PM_NONE
          HaLow.PM_POWERSAVE

    Power management modes.  The default is ``PM_NONE``, in which the
    transceiver is always listening and the interface behaves like any other
    network interface.

    ``PM_POWERSAVE`` puts the interface into a **transmit-only mode**, intended
    for battery powered sensors that wake, send, and sleep again.  A dozing
    transceiver is not listening, so in this mode:

    * the interface cannot be reached by an incoming connection;
    * :meth:`HaLow.scan` fails with ``EPERM``, as probe responses cannot be
      received;
    * association takes considerably longer.

    Outgoing connections and the replies to them continue to work.  How long
    the transceiver stays awake after activity is set by the ``ps_timeout``
    configuration option.

    Associating with power saving already enabled is slow and unreliable, as
    the transceiver is dozing while it tries.  Associate first and enable it
    afterwards::

        nic.active(True)
        nic.connect(ssid, key)
        while not nic.isconnected():
            pass
        nic.config(pm=network.HaLow.PM_POWERSAVE)


Link status
-----------

:meth:`HaLow.status` with no argument returns one of:

=======================  =====  =============================
Constant                 Value  Meaning
=======================  =====  =============================
``STAT_IDLE``            0      down
``STAT_CONNECTING``      1      connecting
``STAT_NOIP``            2      associated, no IP address yet
``STAT_GOT_IP``          3      up, with an IP address
``STAT_CONNECT_FAIL``    -1     connection failed
=======================  =====  =============================
