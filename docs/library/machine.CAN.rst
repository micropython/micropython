.. currentmodule:: machine
.. _machine.CAN:

class CAN -- Controller Area Network protocol
=============================================

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

.. note:: The planned ``can`` and ``aiocan`` micropython-lib modules will be the
          recommended way to use CAN with MicroPython.

Constructor
-----------

.. class:: CAN(id, *args, **kwargs)

   Construct a CAN controller object of the given id:

   - ``id`` identifies a particular CAN controller object; it is board and port specific.
   - All other arguments are passed to :func:`CAN.init`. At least one argument (``bitrate``)
     must be provided.

   Future versions of this class may also accept port-specific keyword arguments
   here which configure the hardware. Currently no such keyword arguments are
   implemented.

.. Add a table of port-specific keyword arguments here, once they exist

Methods
-------

.. method:: CAN.init(bitrate, mode=CAN.MODE_NORMAL, sample_point=75, sjw=1, tseg1=None, tseg2=None, auto_retransmit=True)

   Initialise the CAN bus with the given parameters:

   - *bitrate* is the desired bus bit rate in bits per second.
   - *mode* is one of the values shown under `can-modes`, indicating the
     desired mode of operation. The default is "normal" operation on the bus.

   The next parameters are optional and relate to CAN bit timings. In most cases
   you can leave these parameters set to the default values:

   - *sample_point* is an integer percentage of the data bit time. It
     specifies the position of the bit sample with respect to the whole nominal
     bit time. The CAN driver will calculate parameters accordingly. This
     parameter is ignored if *tseg1* and *tseg2* are set.
   - *sjw* is the resynchronisation jump width in units of time quanta for
     nominal bits; it can be a value between 1 and 4 inclusive for classic CAN.
   - *tseg1* defines the location of the sample point in units of time quanta
     for nominal bits; it can be a value between 1 and 16 inclusive for classic
     CAN. This is the sum of the ``Prop_Seg`` and ``Phase_Seg1`` phases as
     defined in the ISO-11898 standard. If this value is set then *tseg2*
     must also be set and *sample_point* is ignored.
   - *tseg2* defines the location of the transmit point in units of the time
     quanta for nominal bits; it can be a value between 1 and 8 inclusive for
     classic CAN. This corresponds to ``Phase_Seg2`` in the ISO-11898 standard.
     If this value is set then *tseg1* must also be set.

   If these arguments are specified then the CAN controller is configured
   correctly for the desired *bitrate* and the specified total number of time
   quanta per bit. The *tseg1* and *tseg2* values override the
   *sample_point* argument if all of these are supplied.

   Additional parameters:

   - *auto_retransmit* may be set to ``False`` to disable automatic
     retransmit of messages in case of errors or lost arbitration.

   .. note:: Individual controller hardware may have additional restrictions on
             valid values for these parameters, and will raise a ``ValueError``
             if a given value is not supported.

   .. note:: Specific controller hardware may accept additional optional
             keyword parameters for hardware-specific features such as oversampling.

.. method:: CAN.set_filters(filters)

   Set receive filters in the CAN controller. *filters* can be:

   - ``[]`` or ``()`` to accept all incoming messages, or
   - ``None`` to disable all message receiving, or
   - An iterable of one or more items defining the filter criteria. Each item
     should be a tuple or list with three elements:

     - ``identifier`` is a CAN identifier (int).
     - ``bit_mask`` is a bit mask for bits in the CAN identifier field (int).
     - ``flags`` is an integer with zero or more of the bits defined in
       `can-flags` set. This specifies properties that the incoming message needs
       to match. Not all controllers support filtering on all flags, a
       ``ValueError`` is raised if an unsupported flag is requested.

   Incoming messages are accepted if the bits masked in ``bit_mask`` match between
   the message identifier and the filter ``identifier`` value, and flags set in the
   filter match the incoming message.

   If the `CAN.FLAG_EXT_ID` bit is set in flags, the filter matches Extended
   CAN IDs only. If the `CAN.FLAG_EXT_ID` bit is not set, the filter matches
   Standard CAN IDs only.

   All filters are ORed together in the controller. Passing an empty list or tuple
   for the filters argument disables the receive filter (all messages received.)

   Some CAN controllers require each filter to be associated with only one
   receive FIFO. In these cases, the filter items in the argument are allocated
   round-robin to the available FIFOs. This driver does not distinguish between
   FIFOs in the receive IRQ.

   .. note:: If the caller passes an iterable with more items than
      :data:`CAN.filters_max`, ``ValueError`` will be raised.

   Examples
   ^^^^^^^^

   Receive all incoming messages::

     can.set_filters([])

   Receive messages with Standard ID in range 0x300-0x3FF, plus Extended ID
   0x50700 only::

     can.set_filters(((0x300, 0x700, 0),
                      (0x50700, 0x1fff_ffff, CAN.FLAG_EXT_ID)))

.. data:: CAN.filters_max

   Constant value that reads the maximum number of supported receive filters
   for this hardware controller.

   Note that some controllers may have more complex hardware restrictions on the
   number of filters in use (for example, counting Standard and Extended ID
   filters independently.) In these cases `CAN.set_filters` may raise a
   ``ValueError`` even when the ``filters_max`` limit is not exceeded.

.. method:: CAN.send(id, data, flags=0)

   Copy a new CAN message into the controller's hardware transmit queue to be
   sent onto the bus. The transmit queue is a priority queue sorted on CAN
   identifier priority (lower numeric identifiers have higher priority).

   - *id* is an integer CAN identifier value.
   - *data* is a bytes object (or similar) containing the CAN message data,
     unless sending a remote transmission request (see below).
   - *flags* is an integer with zero or more of the bits defined in
     `can-flags` set, specifying properties of the outgoing CAN message
     (Extended ID, Remote Transmission Request, etc.)

   If the queue is full then the send will fail and ``-1`` is returned.

   The send can also fail and return ``-1`` if the provided *id* value has
   equal priority to an existing message in the transmit queue and the CAN
   controller hardware cannot guarantee that messages with the same ID will be
   sent onto the bus in the same order they were added to the queue. To queue
   the message anyway, pass the value :data:`CAN.FLAG_UNORDERED` flag in
   the *flags* argument. This flag indicates that it's OK to send messages with
   the same CAN ID onto the bus in any order.

   If the bit `CAN.FLAG_RTR` is set in the *flags* argument then the
   controller will send a Remote Transmission Request instead of a message.
   In this case the *data* argument should be an integer equal to the ``DLC``
   length of the request.

   If the controller is in the "Bus Off" error state or disabled then calling
   this function will raise an ``OSError``.

   If the message is successfully queued for transmit onto the bus, the function
   returns an integer in the range ``0`` to `CAN.TX_QUEUE_LEN` (exclusive). This
   value is the transmit buffer index where the message is queued to send, and
   can be used by the `CAN.cancel_send` function and in `CAN.IRQ_TX` events.

   .. note:: This intentionally low-level implementation is designed so the
      caller can establish a software queue of outgoing messages.

   .. important:: The CAN "transmit queue" is not a FIFO queue, it is priority
                  ordered, and although it can hold up to `CAN.TX_QUEUE_LEN`
                  items there may be other hardware restrictions on messages
                  which can be queued at the same time.

.. method:: CAN.recv(arg=None)

   Return a CAN message that has been received by the controller, according to
   filters set by :func:`CAN.set_filters`.

   This function takes a single optional argument, if provided then it must be a
   list of at least 4 elements where the second element is a `memoryview` object
   that refers to a `bytearray` or similar object that has enough capacity to hold
   any received CAN message (8 bytes for CAN Classic, 64 bytes for CAN FD). The
   provided list will be returned as a successful result, and avoids memory
   allocation inside the function.

   If no messages have been received by the CAN controller, this function
   returns ``None``.

   .. note:: `CAN.set_filters` must be called before any messages can be received by
             the controller.

   If a message has been received by the CAN controller, this function returns a
   list with 4 elements:

   - Index 0 is the CAN ID of the received message, as an integer.
   - Index 1 is a memoryview that provides access to the received message data.

     - If *arg* is not provided then this is a `memoryview` holding the bytes
       which were received. This `memoryview` is backed by a newly allocated
       `bytearray` large enough to hold any received CAN message. This allows
       the result to be safely reused as a future *arg*, to save memory allocations.
     - If *arg* is provided then the provided `memoryview` will be resized to
       hold exactly the bytes which were received. The caller is responsible for
       making sure the backing object for the `memoryview` can hold a CAN
       message of any length.

     If a remote transmission request is received then the bit `CAN.FLAG_RTR`
     will be set in Index 2 and the memoryview contents will be all zeroes, with
     a length equal to the ``DLC`` field of the received request.

   - Index 2 is an integer with zero or more of the bits defined in
     `can-flags` set. It indicates metadata about the received message.
   - Index 3 is an integer with zero or more of the bits defined in
     `can-recv-errors` set. Any non-zero value indicates potential issues when
     receiving CAN messages. These flags are reset inside the controller each
     time this function returns.

.. method:: CAN.irq(handler=None, trigger=0, hard=False)

   Sets an interrupt *handler* function to be called when one or more of the
   events flagged in *trigger* has occurred.

     - *handler* is a function to be called when the interrupt event
       triggers.  The handler must take exactly one argument which is the
       :class:`CAN` instance.

     - *trigger* configures the event(s) which can generate an interrupt.
       Possible values are a mask of one or more of the following:

       - `CAN.IRQ_RX` event occurs after the CAN controller has received
         at least one message into its RX FIFO (meaning that :func:`CAN.recv()`
         will return successfully).
       - `CAN.IRQ_TX` event occurs after the CAN controller has either
         successfully sent a message onto the CAN bus or failed to send a
         message. This trigger has additional requirements for the handler, see
         `machine_can_irq_flags` for details.
       - `CAN.IRQ_STATE` event occurs when the CAN controller has transitioned
         into a more severe error state. Call :func:`CAN.get_state()` to get the
         updated state.

     - *hard* if True, a hard interrupt is used. This reduces the delay between
       the CAN controller event and the handler being called. Hard interrupt
       handlers may not allocate memory; see :ref:`isr_rules`.

   Returns an irq object. If called with no arguments then a previously-configured
   irq object is returned.

.. method:: CAN.cancel_send(index)

   Request the CAN controller to cancel sending a message onto the bus.

   Argument *index* identifies a single transmit buffer. It should be an
   integer in the range ``0`` to ``CAN.TX_QUEUE_LEN`` (exclusive). Generally
   this will be a value previously returned by :func:`CAN.send()`.

   The result is ``True`` if a message was pending transmission in this buffer
   and transmission was cancelled.

   The result is ``False`` otherwise (either no message was pending transmission
   in this buffer, or transmission succeeded already).

   The IRQ event `CAN.IRQ_TX` should be used to determine if a message
   was definitely sent or not, but note there are potential race conditions if a
   transmission is cancelled and then the same buffer is used to send another
   message (especially if the CAN controller IRQ is not "hard").

.. method:: CAN.get_state()

   Returns an integer value indicating the current state of the controller.
   The value will be one of the values defined in `can-states`.

   Lower severity error states may automatically clear if the bus recovers, but
   the `CAN.STATE_BUS_OFF` state can only be recovered by calling
   :func:`CAN.restart()`.

.. method:: CAN.get_counters([list] /)

   Returns controller's error counter values. The result is a list of eight
   values. If the optional *list* parameter is specified then the provided
   list object is updated and returned as the result, to avoid an allocation.

   The list items are:

   - TEC (Transmit Error Counter) value
   - REC (Receive Error Counter) value
   - Number of times the controller entered the Warning state from the Active state.
   - Number of times the controller entered the Error Passive state from the Warning state.
   - Number of times the controller entered the Bus Off state from the Error Passive state.
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
   The result is a list of six values. If the optional *list* parameter is
   specified then the provided list object is updated and returned as the
   result, to avoid an allocation.

   The list items are:

   - Exact bitrate used by the controller. May vary from *bitrate* argument
     passed to :func:`CAN.init()` due to quantisation to meet hardware
     constraints.
   - Resynchronisation jump width (SJW) in units of time quanta for nominal
     bits. Has the same meaning as the *sjw* parameter of :func:`CAN.init()`.
   - Location of the sample point in units of time quanta for nominal bits. Has
     the same meaning as the *tseg1* parameter of :func:`CAN.init()`.
   - Location of the transmit point in units of time quanta for nominal bits.
     Has the same meaning as the *tseg2* parameter of :func:`CAN.init()`.
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

.. method:: CAN.restart()

   Causes the controller to exit `STATE_BUS_OFF` without clearing any other
   internal state. Also clears some of the error counters (always the number
   of times each error state has been entered, possibly TEC and REC depending
   on the controller.)

   Calling this function also cancels any messages waiting to be sent. No
   `IRQ_TX` interrupts are delivered for these messages.

   Note that this function may or may not cause the controller to exit the
   "Error Passive" state, depending whether the controller hardware zeroes
   TEC and REC or not.

.. method:: CAN.mode([mode])

   With an argument, transitions the CAN controller to a new mode of operation,
   without resetting it. Argument is one of `can-modes`. Returns the
   previous mode of operation.

   Without any arguments, returns the current mode of operation.

   .. note:: Not all modes are supported by all CAN controller hardware.
             Passing an unsupported mode value will raise ``ValueError``.

.. method:: CAN.deinit()

   De-initialises a previously active CAN instance. All pending messages
   (transmit and receive) are dropped and the controller stops interacting on
   the bus. To use this instance again, call :func:`CAN.init()`.

   No `IRQ_TX` or `IRQ_RX` interrupts are called in response to calling this
   function.

   See also :func:`CAN.restart()`.

Constants
---------

.. data:: CAN.TX_QUEUE_LEN

   Maximum number of CAN messages which can be queued in the outgoing hardware
   message queue of the controller. The "transmit buffer indexes" used by
   :func:`CAN.send()`, :func:`CAN.cancel_send()` and `machine_can_irq_flags`
   will be in this range.

.. _can-modes:

Modes
^^^^^

These values represent controller modes of operation. Not all controllers may support all modes.

.. data:: CAN.MODE_NORMAL

   The controller is active as a standard CAN network node (will acknowledge valid messages and may transmit errors
   depending on its current `State <can-states>`.

.. data:: CAN.MODE_SLEEP

   CAN controller is asleep in a low power mode. Depending on the controller, this may support waking the controller and transitioning to `CAN.MODE_NORMAL` if CAN traffic is received.

.. data:: CAN.MODE_LOOPBACK

   A testing mode. The CAN controller is still connected to the external bus, but will also receive its own transmitted messages and ignore any ACK errors.

.. data:: CAN.MODE_SILENT

   CAN controller receives messages but does not interact with the CAN bus (including sending ACKs, errors, etc.)

.. data:: CAN.MODE_SILENT_LOOPBACK

   A testing mode that does not require a CAN transceiver to be connected at
   all. The CAN controller receives its own transmitted messages without
   interacting with the CAN bus at all. The CAN TX and RX pins remain idle.

.. _can-states:

States
^^^^^^

These values are returned by :func:`CAN.get_state()` and reflect the error state of the CAN controller:

.. data:: CAN.STATE_STOPPED

    The controller has not been initialised.

.. data:: CAN.STATE_ACTIVE

    The controller is active and ``TEC`` and ``REC`` error counters are both below the
    warning threshold of 96. See :func:`CAN.get_counters()`.

.. data:: CAN.STATE_WARNING

    The controller is active but at last one of the ``TEC`` and ``REC`` error counters
    are between 96 and 127. See :func:`CAN.get_counters()`.

.. data:: CAN.STATE_PASSIVE

    The controller is in the "Error Passive" state meaning it no longer transmits active
    errors to the bus, but it is otherwise functional. This state is entered when at
    least one of the ``TEC`` and ``REC`` error counters is 128 or greater, but
    ``TEC`` is less than 255. See :func:`CAN.get_counters()`.

.. data:: CAN.STATE_BUS_OFF

    The controller is in the Bus-Off state, meaning ``TEC`` error counter is
    greater than 255. The CAN controller will not interact with the bus in this
    state, and needs to be restarted manually to continue.

.. _can-flags:

Message Flags
^^^^^^^^^^^^^

   These values represent metadata about a CAN message. Functions
   :func:`CAN.send()`, :func:`CAN.recv()`, and :func:`CAN.set_filters()`` either
   accept or return an integer value made up of zero or more of these flags bitwise
   ORed together.

.. data:: CAN.FLAG_RTR

   Indicates a message is a remote transmission request.

.. data:: CAN.FLAG_EXT_ID

   If set, indicates a Message identifier is Extended (29-bit). If not set,
   indicates a message identifier is Standard (11-bit).

.. data:: CAN.FLAG_FD_F

   If set, indicates a Message is in the CAN FD data format. This means its data
   payload can be up to 64 bytes long. Passing this flag to a controller which
   lacks CAN FD support will raise ``ValueError``.

.. data:: CAN.FLAG_BRS

   For CAN FD controllers and FD data format messages, indicates the
   bitrate should be switched (raised) during the data phase.

.. data:: CAN.FLAG_UNORDERED

   If set in the ``flags`` argument of :func:`CAN.send`, indicates that it's
   OK if messages with the same CAN ID are sent in any order onto the bus.

   Otherwise trying to queue multiple messages with the same ID may result in
   :func:`CAN.send` failing if the controller hardware can't enforce ordering.

   This flag is never set on received messages, and is ignored by
   `CAN.set_filters()`.

.. _can-recv-errors:

Receive Error Flags
^^^^^^^^^^^^^^^^^^^

The result of :func:`CAN.recv()` includes an integer value made up of zero or
more of these flags bitwise ORed together. If set, these flags indicate
potential general issues with receiving CAN messages.

.. data:: CAN.RECV_ERR_FULL

The hardware FIFO where this message was received is full, and additional incoming messages may be lost.

.. data:: CAN.RECV_ERR_OVERRUN

The hardware FIFO where this message was received is full, and one or more incoming messages has been lost.

.. data:: CAN.RECV_ERR_ESI

The ESI flag of a received CAN FD message was set. This flag is only ever set if the controllers is in CAN FD mode.

IRQ values
^^^^^^^^^^

.. data:: CAN.IRQ_RX
          CAN.IRQ_TX
          CAN.IRQ_STATE

    IRQ event triggers. Used with :func:`CAN.irq()` and `machine_can_irq_flags`.

.. data:: CAN.IRQ_TX_FAILED
          CAN.IRQ_TX_IDX_SHIFT
          CAN.IRQ_TX_IDX_MASK

    Additional IRQ event flags for `CAN.IRQ_TX`. See `machine_can_irq_flags`.

.. _machine_can_irq_flags:

IRQ flags
---------

Calling :func:`CAN.irq()` registers an interrupt handler with one or more of the
triggers `CAN.IRQ_RX`, `CAN.IRQ_TX` and `CAN.IRQ_STATE`.

The function returns an IRQ object, and calling the ``flags()`` function on this
object returns an integer indicating which trigger event(s) triggered the
interrupt. A CAN IRQ handler should call the ``flags()`` function repeatedly
until it returns ``0``.

When the ``flags()`` function returns with `CAN.IRQ_TX` bit set, the
handler can also check the following flag bits in the result for additional
information about the TX event:

* ``CAN.IRQ_TX_FAILED`` bit is set if the transmit failed. Usually this will only
  happen if :func:`CAN.cancel_send()` was called, although it may also happen if
  the controller enters an error state or is reset while messages are queued to
  send.
* ``CAN.IRQ_TX_MASK << CAN.IRQ_TX_SHIFT`` is a bitmasked region of the flags
  value that holds the index of the transmit buffer which generated the event.
  This will be an integer in the range ``0`` to `CAN.TX_QUEUE_LEN` (exclusive),
  and will match the result of a previous call to `CAN.send()`.

IRQ_TX Example
^^^^^^^^^^^^^^
::

    from machine import CAN

    def irq_send(can):
        while flags := can.irq().flags():
            if flags & can.IRQ_TX:
                idx = (flags >> can.IRQ_TX_IDX_SHIFT) & can.IRQ_TX_IDX_MASK
                success = not (flags & can.IRQ_TX_FAILED)
                print("irq_send", idx, success)

    can = CAN(1, 500_000)
    can.irq(irq_send, trigger=can.IRQ_TX, hard=True)

.. important:: If the `CAN.IRQ_TX` trigger is set then the handler **must**
               call ``flags()`` repeatedly until it returns ``0``, as shown in
               this example. Otherwise, CAN interrupts may not be correctly
               re-enabled.
