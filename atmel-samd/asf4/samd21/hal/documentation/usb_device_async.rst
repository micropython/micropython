The USB Device Asynchronous Driver
==================================

Universal Serial Bus (USB) is an industry standard that defines the cables,
connectors and communication protocols used in a bus for connection,
communication, and power supply between computers and electronic devices.

The USB device driver provides necessary APIs to support USB Device states and
USB data flow. So that the USB Device enumeration, class and vendor support can
be implemented base on it. The driver is asynchronous, which means that all USB
data processing is done in callbacks..

To be recognized by a host, a USB device must handle a subset of the USB events.
The USB device should build up control communication to report its descriptors
to the host and accept host's requests. An application or upper stack that uses
the USB device driver should have its descriptors prepared, catch the callbacks,
monitor the control requests and handle them correctly.
Usually, a USB device application that can be enumerated may use the following
sequence:

* Initialize
* Register callback and handle Reset event, where:

  * Initialize endpoint 0
  * Register callbacks for endpoint 0, where some of the standard requests
    defined in Chapter 9, USB specification
    (see `USB 2.0 Documents <http://www.usb.org/developers/docs/usb20_docs>`_)
    are handled

    * Setup request handling callback

      * On *GetDeviceDescriptor* request, sends device descriptor
      * On *GetConfigurationDescriptor* request, sends configuration descriptors
      * On *SetAddress* request sends no data, just goes to status phase
      * On *SetConfigure* request initialize and enable other endpoints, sends
        no data, just goes to status phase
    * Transfer done handling callback

      * On *SetAddress* request apply the new address
      * On *SetConfigure* request a global variable can be set to indicates
        that the host driver is loaded and device is ready to work
  * Enable endpoint 0
* Enable
* Attach device

To support USB transfer on endpoints, endpoints information is stored
by the driver internally, including control request data, endpoint status,
callbacks and transfer data pointers. The number of endpoints that the driver
can support is defined through configuration. To optimize RAM usage, the
number of endpoints the driver needs to support should be minimized.

Features
--------

* Initialization/de-initialization
* Enabling/disabling
* USB device attachment/detachment
* USB device address control
* USB working speed status
* USB device frame number and micro frame number status
* Sending remote wakeup to host
* Callback management for following USB events:

  * Start of Frame (SOF)
  * Other USB events:

    * VBus change
    * Reset
    * Wakeup
    * Linked Power Management (LPM) Suspend
    * Suspend
    * Error
* Endpoints management:

  * Endpoint initialization/de-initialization
  * Endpoint enabling/disabling
  * Control endpoint setup request packet
  * Data transfer and abort
  * Endpoint halt state control
  * Endpoint status, including:

    * Endpoint address
    * Last transfer result status code
    * Last error status code
    * Current transfer state
    * Transfer size and done count
  * Endpoint callback management for endpoint and its transfer events:

    * In case a setup request received on control endpoint
    * In case transfer finished with/without error

Applications
------------

USB Device stack, to monitor USB events, handle control requests and process
data.

Dependencies
------------

* USB device capable hardware
* 48MHz clock for low-speed and full-speed and 480MHz clock for high-speed

Concurrency
-----------

N/A

Limitations
-----------

* When a buffer is used by a USB endpoint to transfer data, it must be kept
  unchanged while the transfer is in progress.
* After receiving a request that has no data expected, the transfer function
  must be called with data length zero to complete control status phase.

Known issues and workarounds
----------------------------

N/A

Considerations for D21 USB
--------------------------

Clocks
^^^^^^

DFLL must be used to generate 48MHz clock for USB device with either of the
following mode:

* USB Clock Recovery Mode

  * Set "DFLL Enable", "Bypass Coarse Lock", "Chill Cycle Disable",
    "USB Clock Recovery Mode", "Stable DFLL Frequency"
  * Clear "Wait Lock"
  * Leave "Operating Mode Selection" to "Closed Loop Mode"

* Closed Loop Mode

  * Set "DFLL Enable"
  * Clear "USB Clock Recovery Mode", "Stable DFLL Frequency"
  * Select "Closed Loop Mode" of "Operating Mode Selection"
  * Set "DFLL Multiply Factor" to 1464 or 1465 (48000000/32768)
  * Select "Reference Clock Source" to use 32768Hz source, e.g., use GCLK1 and
    for GCLK1 settings:

    * Set "Generic Clock Generator Enable"
    * Select "XOSC32K" of "Generic clock generator 1 source", and for XOSC32K
      settings:

      * Set "External 32K Oscillator Enable", "Enable 32KHz Output",
        "Enable XTAL"
      * Set a right value for "Startup time for the 32K Oscillator", e.g.,
        1125092 us

Endpoints
^^^^^^^^^

Each USB device endpoint number supports two endpoint addresses, corresponding
to IN and OUT endpoint. E.g., for endpoint 1, the endpoint IN has address 0x81
and endpoint OUT has address 0x01. Thus, the possible supported endpoint
addresses are almost two times of max endpoint number (endpoint 0 must be used
as control endpoint instead of dedicated IN and OUT endpoints).

Buffering and RAM usage optimization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When transferring data through USB device endpoints, buffer pointers can be
used to let endpoint get access to the buffer, but there are some limits:

* For control endpoint there must always be a buffer available to put received
  setup packet.
* For IN endpoint (transmit to host) the data must in RAM.
* For OUT endpoint (receive from host) the data pointer must be aligned, and
  the data size must be aligned by max endpoint size and not zero.

The driver has option for each endpoint to allocate internal static buffer as
cache to buffer input/output data, to remove upper limits. The configuration
affects the parameter check of transfer functions, and the RAM usage.

* For control endpoints, cache buffer must be enabled to fill setup packet.
  In addition, to support unaligned OUT packet and IN packet inside flash, the
  buffer size must be equal to or larger than max endpoint size.
* For OUT endpoints, if the cache is allocated, it's possible to pass unaligned
  buffer address and buffer size to transfer function. Else the transfer
  function only accepts aligned buffer with it's size multiple of endpoint
  packet size.
* For IN endpoints, if the cache is allocated, it's possible to pass buffer
  pointer to internal flash or other memory part other than RAM to the transfer
  function.

To optimize the RAM usage, the configuration of max endpoint number, max number
of endpoints supported and the buffer usage of used input and/or output
endpoints can be adjusted.

