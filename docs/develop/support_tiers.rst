MicroPython Support Tiers
=========================

MicroPython operates with a set of Support Tier levels for the various ports.
Tiers 1, 2 and 3 are the main Tier levels with Tier 1 being the most mature and
actively maintained.  There is also Tier M for additional ports used primarily
for maintenance, development and testing. These Tier levels are defined in the
table below.

.. table::
  :widths: 40 9 9 9 9

  +-----------------------------------------------+--------+--------+--------+--------+
  |                                               | Tier 1 | Tier 2 | Tier 3 | Tier M |
  +-----------------------------------------------+--------+--------+--------+--------+
  | builds pass under CI                          | ✔      | ✔      | ✔      | ✔      |
  +-----------------------------------------------+--------+--------+--------+--------+
  | tests run under CI (where possible)           | ✔      | ✔      | ✔      | ✔      |
  +-----------------------------------------------+--------+--------+--------+--------+
  | actively maintained                           | ✔      | ✔      |        | ✔      |
  +-----------------------------------------------+--------+--------+--------+--------+
  | stable Python API                             | ✔      | ✔      |        |        |
  +-----------------------------------------------+--------+--------+--------+--------+
  | new features actively developed               | ✔      | ✔      |        |        |
  +-----------------------------------------------+--------+--------+--------+--------+
  | tested on hardware for releases               | ✔      | ✔      |        |        |
  +-----------------------------------------------+--------+--------+--------+--------+
  | prioritized bug reports                       | ✔      |        |        | ✔      |
  +-----------------------------------------------+--------+--------+--------+--------+
  | regressions warrant a patch release           | ✔      |        |        | ✔      |
  +-----------------------------------------------+--------+--------+--------+--------+
  | has port-specific documentation               | ✔      |        |        |        |
  +-----------------------------------------------+--------+--------+--------+--------+

Lower Tiers may tick more boxes, but the above table defines the minimum requirements
for a port to belong to a Tier.

Tier 1 ports:

  - esp32
  - mimxrt
  - rp2
  - samd
  - stm32
  - unix
  - windows

Tier 2 ports:

  - alif
  - embed
  - nrf
  - renesas-ra
  - webassembly
  - zephyr

Tier 3 ports:

  - cc3200
  - esp8266
  - pic16bit
  - powerpc

Tier M ports:

  - bare-arm
  - minimal
  - qemu
