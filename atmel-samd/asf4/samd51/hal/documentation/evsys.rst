===================
Event system driver
===================

Event system allows autonomous, low-latency and configurable communication between peripherals.

Several peripherals can be configured to generate and/or respond to signals known as events. The exact condition
to generate an event, or the action taken upon receiving an event, is specific to each peripheral. Peripherals that
respond to events are event users, peripherals that generate events are called event generators. A peripheral can have
one or more event generators and can have one or more event users.

Communication is made without CPU intervention and without consuming system resources such as bus or RAM bandwidth. This
reduces the load on the CPU and system resources, compared to a traditional interrupt-based system.

The Event System consists of several channels which route the internal events from generators
to users. Each event generator can be selected as source for multiple channels, but a channel cannot
be set to use multiple event generators at the same time.

Event system driver allows to configure event system of an MCU.

Limitations
-----------

 - Event channel configuration is static
