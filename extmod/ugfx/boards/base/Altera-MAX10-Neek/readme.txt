This directory contains the board files for the Altera MAX10 NEEK board.

In order to use this board file, the following things are needed:
	- You need to use the FrameBuffer reader component in QSys
	- You need to enable the feature of the NIOS-II CPU core that allows to bypass the
		data cache if the MSB is set.

On this board uGFX currently supports:
	- GDISP via the framebuffer driver
