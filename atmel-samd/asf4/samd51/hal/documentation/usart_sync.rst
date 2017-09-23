The USART Synchronous Driver
============================

The universal synchronous and asynchronous receiver and transmitter
(USART) is usually used to transfer data from one device to the other.

User can set action for flow control pins by function usart_set_flow_control,
if the flow control is enabled. All the available states are defined in union
usart_flow_control_state.

Note that user can set state of flow control pins only if automatic support of
the flow control is not supported by the hardware.

Features
--------

* Initialization/de-initialization
* Enabling/disabling
* Control of the following settings:

  * Baudrate
  * UART or USRT communication mode
  * Character size
  * Data order
  * Flow control
* Data transfer: transmission, reception

Applications
------------

They are commonly used in a terminal application or low-speed communication
between devices.

Dependencies
------------

USART capable hardware.

Concurrency
-----------

Write buffer should not be changed while data is being sent.


Limitations
-----------

* The driver does not support 9-bit character size.
* The "USART with ISO7816" mode can be only used in ISO7816 capable devices. 
And the SCK pin can't be set directly. Application can use a GCLK output PIN 
to generate SCK. For example to communicate with a SMARTCARD with ISO7816 
(F = 372 ; D = 1), and baudrate=9600, the SCK pin output frequency should be 
config as 372*9600=3571200Hz. More information can be refer to ISO7816 Specification.

Known issues and workarounds
----------------------------

N/A
