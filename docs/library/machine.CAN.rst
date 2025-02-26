.. currentmodule:: machine
.. _machine.CAN:

class CAN -- Controller Area Network protocol
=============================================

.. warning:: Currently no MicroPython ports have ``machine.CAN`` controllers.
             This is a design document only.

CAN is a two-wire serial protocol used for reliable real-time message delivery
between one or more nodes connected to a common bus. CAN 2.0 was standardised in
ISO-11898, and is now also known as CAN Classic.

There is also a newer, backwards compatible, protocol named CAN FD (CAN with
Flexible Data-Rate).

CAN support requires a controller (often an internal microcontroller
peripheral), and an external transceiver to level-shift the signals onto the CAN
bus.

The ``machine.CAN`` interface is a *low level basic* CAN messaging interface
that abstracts a CAN controller as an outgoing priority queue for sending
messages, an incoming queue for receiving messages, and mechanisms for reporting
errors.

.. note:: The forthcoming ``can`` and ``aiocan`` micropython-lib modules are the
          recommended way to use CAN with MicroPython.

Constructor
-----------

.. class:: CAN(id, **kwargs)

   Construct a CAN controller object of the given id:

   - ``id`` identifies a particular CAN controller object; it is board and port specific.
   - ``**kwargs`` are hardware-specific identifiers for the particular
     board and port. For example, to set which GPIO pins to use for the
     CAN controller.

Methods
-------

.. method:: CAN.init(bitrate, mode=CAN.Mode.NORMAL, sample_point=75, sjw=1, tseg1=None, tseg2=None, f_clock=None, auto_retransmit=True)

   Initialise the CAN bus with the given parameters:

   - ``bitrate`` is the desired bus bit rate in bits per second.
   - ``mode`` is one of :class:`CAN.Mode` enumerated values, indicating the
      desired mode of operation.

   The remaining parameters are optional and relate to CAN bit timings. In many
   cases these parameters can be omitted or left at the default values:

   - ``sample_point`` is an integer percentage of the data bit time. It
     specifies the position of the bit sample with respect to the whole nominal
     bit time. The CAN driver will calculate parameters accordingly. This
     parameter is ignored if ``tseg1`` and ``tseg2`` are set.
   - ``sjw`` is the resynchronisation jump width in units of time quanta for
     nominal bits; it can be a value between 1 and 4 inclusive for classic CAN.
   - ``tseg1`` defines the location of the sample point in units of time quanta
      for nominal bits; it can be a value between 1 and 16 inclusive for classic
      CAN. This is the sum of the ``Prop_Seg`` and ``Phase_Seg1`` phases as
      defined in the ISO-11898 standard. If this value is set then ``tseg2``
      must also be set and ``sample_point`` is ignored.
   - ``tseg2`` defines the location of the transmit point in units of the time
     quanta for nominal bits; it can be a value between 1 and 8 inclusive for
     classic CAN. This corresponds to ``Phase_Seg2`` in the ISO-11898 standard.
     If this value is set then ``tseg1`` must also be set.
   - ``f_clock`` is the system clock frequency for the CAN peripheral, before
     any prescaler is applied to achieve a desired bit rate. This can be
     omitted on most ports where the value is known to the hardware.
   - ``auto_retransmit`` may be set to ``False`` to disable automatic
     retransmit of messages in case of errors or lost arbitration.

   If these arguments are specified then the CAN controller is configured
   correctly for the desired ``bitrate`` and the specified total number of time
   quanta per bit. The ``tseg1`` and ``tseg2`` values override the
   ``sample_point`` argument if all of these are supplied.

   .. note:: Individual controller hardware may have additional restrictions on
             valid values for these parameters, and will raise a ``ValueError``
             if a given value is not supported.

   .. note:: Specific controller hardware may accept additional optional
             parameters for hardware-specific features such as oversampling.

.. method:: CAN.init_fd(bitrate, sample_point=None, sjw=None, tseg1=None, tseg2=None)

   Initialise CAN FD for controllers which support it.

   .. note:: This method is not present on the class for controllers that only
             support CAN Classic.

   This must be called after :func:`CAN.init()`. The parameters all have the
   same meanings and behaviour as that function, however they configure the
   Bit Rate Switch (BRS) feature of CAN FD.

   When specifying exact timing parameters in units of time quanta, the accepted
   ranges are:

   - ``sjw`` can be a value between 1 and 128 inclusive.
   - ``tseg1`` can be a value between 2 and 256 inclusive.
   - ``tseg2`` can be a value between 2 and 128 inclusive

   .. note:: To avoid the controller temporarily becoming active on the bus
             without CAN FD support configured, first call :func:`CAN.init()`
             with ``mode`` set to ``CAN.mode.SLEEP``, then call this function to
             configure CAN FD, then call :func:`CAN.mode()` to start operation.

   .. note:: Specific controllers may have tighter constraints on allowed
             timings. If a value is out of range, the controller will raise a
             ``ValueError``.

.. method:: CAN.set_filters(filters)

   Set receive filters in the CAN controller. ``filters`` should be an iterable,
   where each item is a tuple or list with three elements:

   - ``identifier`` is a CAN identifier (int).
   - ``bit_mask`` is a bit mask for bits in the CAN identifier field (int).
   - ``flags`` is one or more bitwise OR-ed values from
     :class:`CAN.MessageFlags` that the incoming message needs to match. Not
     all controllers support filtering on all flags, a ``ValueError`` is raised
     if an unsupported flag is requested.

  Incoming messages are accepted if the bits masked in ``bit_mask`` match between
  the message identifier and the filter ``identifier`` value, and flags set in the
  filter match the incoming message.

  All filters are ORed together in the controller. Passing an empty list (``[]``)
  for the filters argument disables the receive filter (all messages received.)

   .. note:: If the caller passes a list with more entries than :data:`CAN.filters_max`,
      ``ValueError`` will be raised.

.. data:: CAN.filters_max

   Constant value that reads the maximum number of supported receive filters
   for this hardware controller.

.. method:: CAN.send(identifier, data, flags=0, fifo_equal=True)

   - ``identifier`` is an integer CAN identifier value.
   - ``data`` is a bytes object (or similar) containing the CAN message data.
   - ``flags`` is OR-ed together flags :class:`CAN.MessageFlags` specifying
     properties of the outgoing CAN message (Extended ID, Remote request, etc.)
   - ``keep_equal`` is a flag for whether messages with the same identifiers
     should be kept in the queue or replaced (see below).

   Write a new CAN message into the controller's hardware transmit queue to be
   sent onto the bus. The transmit queue is a priority queue sorted first on CAN
   identifier priority (lower numeric identifiers have higher priority), and
   second on order of insertion.

   If the queue is full and the new message has higher priority than the lowest
   priority message in the queue then the lowest priority message will be
   de-queued and replaced.

   If the queue is full and the new message has equal priority to the lowest
   priority message in the queue, then the behaviour depends on the
   ``keep_equal`` argument:

   - If ``True`` (default), the queue remains unchanged and the new message is
     not queued. This allows reliable transmission of messages in FIFO order.
   - If ``False``, the oldest matching message in the transmit queue is
     de-queued and replaced with the new message.

   The function returns an integer index (1-based) which identifies the queue
   entry where the new message was written. This is a hardware-specific index
   value that does not reflect which message will be sent next.

   An additional value ``CAN.SEND_FLAG`` is bitwise ORed with the result if the
   previously queued message at this entry indexed was de-queued and replaced
   with the new message.

   If the hardware queue was full and the new message could not be written into
   the queue at all due to priorities, then the return value is ``0``.

   .. note:: This intentionally low-level implementation is designed so the
      caller tracks the messages in each hardware queue entry. See the ``can`` and
      ``aiocan`` modules for details.

.. data:: CAN.SEND_FLAG

    Constant bit value that is ORed with the index result from :func:`CAN.send`
    if another message was de-queued and replaced with the new message.

.. data:: CAN.send_max

   Constant, indicating the maximum number of entries in the hardware transmit
   queue (this is also the highest possible index returned from ``send``).

.. method:: CAN.irq_send(callback, hard=False)

   Sets an interrupt ``callback`` function to be called when the controller sends
   a message on the bus, or has attempted to send.

   Callback arguments are a tuple or list with four elements:

   - The integer index value that was returned from :func:`CAN.send` when the
     corresponding message was queued.
   - A :func:`time.ticks_us()` timestamp indicating when the message was
     acknowledged sent (or when sending failed).
   - Send Status. 0 if the message was sent successfully, or a bitwise OR
     of :class:`CAN.SendErrors` flags otherwise.
   - Boolean flag, set to ``True`` if the CAN controller hardware has already
     re-queued this message to retry sending. ``False`` if sending succeeded,
     or if the message was not re-qeueud.

   .. note:: The callback function must not store a reference to the argument.
             The same tuple or list object may be reused for each IRQ.

   If ``hard`` is set to True then the callback will be called in "hard"
   interrupt context on ports which support this. The parameter is ignored
   otherwise.

.. method:: CAN.irq_recv(callback, hard=False)

   Sets an interrupt ``callback`` function to be called when the controller receives
   a message on the bus, or detects an error condition while receiving.

   Callback arguments for a successful receive are a tuple or list with the
   following elements:

   - A :func:`time.ticks_us()` timestamp indicating when the
     message was received.
   - The integer CAN identifier which was received. If receive filters are set,
     the identifier will be one that matches.
   - Data payload. A bytes object (or equivalent) showing the data portion of the
     message.
   - Message flags. An integer comprised of bitwise ORed values from
     :class:`CAN.MessageFlags`. This indicates metadata about the message.
   - Error flags. ``0`` if the message was received successfully, or bitwise
     ORed values of :func:`CAN.RecvErrors` otherwise. Note that if error flags
     are set then the values of other elements may be ``None``.

   .. note:: The callback function must not store a reference to the argument or
             the data payload. The same objects may be reused in future IRQs.

   If ``hard`` is set to True then the callback will be called in "hard"
   interrupt context on ports which support this. The parameter is ignored
   otherwise.

   Unless not possible due to hardware limitations, the controller will call the
   ``irq_recv()`` callback in the same order that messages were received on the
   bus.

   .. note:: As a low-level class, :class:`machine.CAN` does not contain a
             normal ``recv()`` method. Refer to (planned) ``can`` and ``iocan``
             modules, instead.

.. method:: CAN.get_state()

   Returns a :class:`CAN.State` value indicating the state of the controller.

.. method:: CAN.irq_state(callback, hard=False)

   Sets a callback which is called whenever the controller state changes. The
   callback argument is the new :class:`CAN.State` value.

   If ``hard`` is set to True then the callback will be called in "hard"
   interrupt context on ports which support this. The parameter is ignored
   otherwise.

.. method:: CAN.get_counters([list] /)

   Returns controller's error counter values. The result is a list of eight
   values. If the optional ``list`` parameter is specified then the provided
   list object is updated and returned as the result, to avoid an allocation.

   The list items are:

   - TEC (Transmit Error Counter) value
   - REC (Receive Error Counter) value
   - Number of times the controller entered the Error Warning state.
   - Number of times the controller entered the Error Passive state.
   - Number of times the controller entered the Bus Off state.
   - Total number of pending TX messages in the hardware queue.
   - Total number of pending RX messages in the hardware queue.
   - Number of times an RX overrun occurred.

  .. note:: Depending on the controller, these values may overflow back to 0 after
            a certain value.

  .. note:: If a controller doesn't support a particular counter, it will return
            ``None`` for that list element.

.. method:: CAN.get_timings([list])

   Returns a list of elements indicating the current timings configured in the
   CAN controller. This can be used to verify timings for debugging purposes.
   The result is a list of six values. If the optional ``list`` parameter is
   specified then the provided list object is updated and returned as the
   result, to avoid an allocation.

   The list items are:

   - Exact bitrate used by the controller. May vary from ``bitrate`` argument
     passed to :func:`CAN.init()` due to quantisation to meet hardware
     constraints.
   - Resynchronisation jump width (SJW) in units of time quanta for nominal
     bits. Has the same meaning as the ``sjw`` parameter of :func:`CAN.init()`.
   - Location of the sample point in units of time quanta for nominal bits. Has
      the same meaning as the ``tseg1`` parameter of :func:`CAN.init()`.
   - Location of the transmit point in units of time quanta for nominal bits.
      Has the same meaning as the ``tseg2`` parameter of :func:`CAN.init()`.
   - CAN FD timing information. ``None`` for controllers which don't support CAN
     FD, or if CAN FD is not initialised. Otherwise, a nested list of four
     elements corresponding to the items above but applicable to the CAN FD BRS
     feature.
   - Optional controller-specific timing information. Depending on the
     controller this will either be ``None`` if controller doesn't report any,
     or it will be a constant length list whose elements are specific to a
     particular hardware controller.

   .. note:: If :func:`CAN.init()` has not been called then this function
             still returns a result, but the result is implementation defined
             and may not be accurate.

.. method:: CAN.reset(mode=CAN.Mode.NORMAL)

   Fully resets the CAN controller hardware. Restores values previously set by :func:`CAN.init()`.

   All hardware queues are emptied, error counters and flags reset, etc.

   The controller transitions into the ``mode`` specified by the argument (see :class:`CAN.Mode`).

.. method:: CAN.restart()

   Restart the CAN controller without resetting its internal state, except for
   TEC and REC. Can be used to manually recover from the ``BUS_OFF`` state.

.. method:: CAN.mode([mode])

   With an argument, transitions the CAN controller to a new mode of operation,
   without resetting it. Argument is one of :func:`CAN.Mode`. Returns the
   previous mode of operation.

   Without any arguments, returns the current mode of operation.

   .. note:: Note all modes are supported by all CAN controller hardware.
             Passing an unsupported mode value will raise ``ValueError``.

.. class:: CAN.Mode

   Enumeration class that holds the following constant values (integers) representing controller modes of operation:

   - ``NORMAL`` - CAN controller interacts normally on the bus.
   - ``SLEEP`` - CAN controller is asleep in a low power mode. Depending on the
     controller, this may support waking the controller and transitioning to
     ``NORMAL`` mode if CAN traffic is received.
   - ``LOOPBACK`` - A testing mode. The CAN controller is still connected to the
     external bus, but will also receive its own transmitted messages and ignore
     any ACK errors.
   - ``SILENT`` - CAN controller receives messages but does not interact with
     the CAN bus (including sending ACKs, errors, etc.)
   - ``SILENT_LOOPBACK`` - A testing mode that does not require a CAN bus. The
     CAN controller receives its own transmitted messages without interacting
     with the CAN bus at all. The CAN TX and RX pins remain idle.

.. class:: CAN.State

   Enumeration class that holds the following constant values (integers) representing the state of the controller on the bus:

   - ``STOPPED`` - The controller is not interacting with the bus.
   - ``ERROR_ACTIVE`` - The controller is in the Error-Active and TEC and REC are both less than 96.
   - ``ERROR_WARNING`` - The controller is in the Error-Warning state, meaning
     at least one of ``TEC`` or ``REC`` is 96 or greater.
   - ``ERROR_PASSIVE`` - The controller is in the Error-Passive state, meaning
     at least one of ``TEC`` or ``REC`` is 128 or greater.
   - ``BUS_OFF`` - The controller is in the Bus-Off state, meaning ``TEC`` is
     greater than 255. It does not currently have any influence on Bus activity.

.. class:: CAN.MessageFlags

   Enumeration class representing possible conditions of a CAN message, as bitwise ORed together flags.

   - ``RTR`` - Message is a remote transmission request. If this bit is set, the
     ``data`` value should be an integer (indicating the length from the ``DLC``
     field), not a bytes object.
   - ``EXTENDED_ID`` - Message identifier is Extended (29-bit). If not set, message
     identifier is Standard (11-bit).
   - ``FD_F``- Message is CAN FD in the FD data format, meaning data payload
     can be up to 64 bytes long. Passing this flag to a controller which lacks
     CAN FD support will raise ``ValueError``.
   - ``BRS`` - For CAN FD controllers and FD data format messages, indicates the
     bitrate should be switched during the data phase.

.. class:: CAN.RecvErrors

   Enumeration class representing possible error statuses when
   receiving a CAN message. Multiple values may be ORed together.

   - ``CRC`` - A CRC error occurred.
   - ``FORM`` - A form error occurred in a fixed-form bit field.
   - ``OVERRUN`` - One or more messages overran the receive hardware
                   queue and were lost.
   - ``ESI`` - The ESI flag of a received CAN FD message was set.

.. class:: CAN.SendErrors

    Enumeration class representing possible error statuses when
    sending a CAN message. Multiple values may be ORed together.

   - ``ARB`` - Arbitration was lost.
   - ``NACK`` - Sent message was not ACKed.
   - ``ERR`` - A bus error was detected during transmission.
