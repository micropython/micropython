CYW43xx WiFi SoC driver
=======================

This is a driver for the CYW43xx WiFi SoC.

There are four layers to the driver:

1. SDIO bus interface, provided by the host device/system.

2. Low-level CYW43xx interface, managing the bus, control messages, Ethernet
   frames and asynchronous events.  Includes download of SoC firmware.  The
   header file `cyw43_ll.h` defines the interface to this layer.

3. Mid-level CYW43xx control, to control and set WiFi parameters and manage
   events.  See `cyw43_ctrl.c`.

4. TCP/IP bindings to lwIP.  See `cyw43_lwip.c`.
