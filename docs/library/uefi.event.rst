:mod:`uefi.event` --- events, timers and hot-plug
=================================================

.. module:: uefi.event
    :synopsis: pollable UEFI events, timers and protocol-install (hot-plug) notification

The ``uefi.event`` submodule exposes UEFI events as :mod:`select`/:mod:`asyncio`
-friendly Python objects.  It builds on the C :class:`uefi.Event` type and adds a
:class:`Timer`, a :class:`ProtocolWatch` for device hot-plug, and the
:func:`wait_async` helper.

:class:`Event`, :class:`Timer` and :class:`ProtocolWatch` are re-exported at the
top level as :class:`uefi.Event`, :class:`uefi.Timer` and
:class:`uefi.ProtocolWatch`.

.. class:: Event(timer=False)

    A pollable UEFI ``EVT_NOTIFY_SIGNAL`` event (a C type from the built-in
    ``_uefi`` module).  Its notify sets a sticky flag and wakes the idle loop, and
    it reports readiness for :mod:`select`/:mod:`asyncio` (it behaves like a
    pollable stream).  Pass ``timer=True`` to create an ``EVT_TIMER`` event that
    :func:`uefi.raw.set_timer` can arm.

    .. attribute:: ptr

        The raw ``EFI_EVENT`` as an integer (the
        :ref:`.ptr contract <uefi_ptr_contract>`).

    .. method:: signal()

        Signal the event.

    .. method:: is_signaled()

        Return ``True`` if the event has fired since the last :meth:`clear`.

    .. method:: clear()

        Clear the sticky signalled flag.

    .. method:: wait(timeout_ms=None)

        Block (yielding to the firmware via ``WaitForEvent``) until the event is
        signalled or *timeout_ms* elapses.  ``None`` waits indefinitely.

    .. method:: close()

        Close the event (``CloseEvent``).  For a timer event this also cancels a
        pending timer.

.. function:: wait_async(ev)

    Coroutine.  Suspend the current :mod:`asyncio` task until *ev* (a
    :class:`Event`) is signalled.  It yields to the scheduler through the asyncio
    I/O queue -- no busy-poll and no ``WaitForEvent`` -- because :class:`Event` is
    a pollable stream.

Timer
-----

.. class:: Timer

    A UEFI timer built on an ``EVT_TIMER`` event -- pollable and awaitable.  A
    context manager that closes the underlying event on exit.

    .. method:: set_periodic(interval_ms)

        Arm the timer to fire repeatedly every *interval_ms* milliseconds.

    .. method:: set_oneshot(delay_ms)

        Arm the timer to fire once after *delay_ms* milliseconds.

    .. method:: cancel()

        Cancel a pending/periodic timer.

    .. method:: is_signaled()
    .. method:: clear()

    .. method:: wait(timeout_ms=None)

        Block until the timer fires (see :meth:`Event.wait`).

    .. method:: wait_async()

        Coroutine.  Suspend the current :mod:`asyncio` task until the timer fires.

    .. method:: sleep_async(delay_ms)

        Coroutine.  Clear, arm a one-shot for *delay_ms*, and await it.

    .. method:: close()

        Close the underlying event (cancels a pending timer).

    .. note::

        Timer resolution is firmware-dependent (often at least 1 ms, despite the
        100 ns unit used internally).

ProtocolWatch
-------------

.. class:: ProtocolWatch(guid)

    Fires whenever a new instance of *guid* is installed -- i.e. when a device of
    that class is hot-plugged.  Implemented over
    :func:`uefi.raw.register_protocol_notify`.  Async-iterable, and a context
    manager that closes the underlying event on exit.

    .. method:: poll()

        Return the list of :class:`~uefi.handle.Handle` that have (re)installed
        *guid* since the last :meth:`poll`.

    .. method:: __anext__()

        Async iterator: awaits the next arriving :class:`~uefi.handle.Handle`.

    .. method:: close()

Notes and pitfalls
------------------

* An ``EVT_NOTIFY_SIGNAL`` :class:`Event` cannot be passed to ``WaitForEvent``
  (a UEFI rule); the pollable path used here sidesteps that.
* Event notify functions run at an elevated task-priority level and behave like
  interrupt handlers.  The C notify only sets a flag and wakes the loop; Python
  callbacks are always run later from the main loop at a safe point.  Never call
  arbitrary Boot Services or Python from a notify.

Example
-------

::

    import asyncio
    from uefi import guid
    from uefi.event import Timer, ProtocolWatch

    async def blink():
        with Timer() as t:
            for _ in range(3):
                await t.sleep_async(500)
                print('blink')

    async def watch():
        with ProtocolWatch(guid.BLOCK_IO_PROTOCOL) as w:
            async for h in w:
                print('new block device:', h)

    asyncio.run(blink())
