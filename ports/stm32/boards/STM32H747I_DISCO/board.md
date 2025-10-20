The Ethernet interface requires a hardware modification due to a pin conflict
between ETH_MDC (PC1) and the SAI4_D1 digital MEMS microphone. To enable
Ethernet, the MEMS microphone must be disconnected from PC1. See the
STM32H747I-DISCO user manual (UM2411) for modification details.

The board includes additional hardware not currently configured in MicroPython:
4" LCD touchscreen (DSI), camera connector (DCMI), WM8994 audio codec, and
digital MEMS microphones.
