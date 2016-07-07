1. Introduction

This is a driver for ED060SC4 800x600 e-ink display panel manufactured by
Prime View. Note that this driver uses a direct connection to the panel from
the processor; it does not use a separate controller chip between the processor
and the panel.

To use the driver, you need to be able to control the following signals from
your processor:

Gate driver: SPV, CKV, GMODE.
Source driver: CL, LE, OE, SPH, D0-D7.
Power supply: +-15V, +22V, -20V, digital Vdd

The file "example_schematics.png" shows how to connect the signals to e.g. a
STM32L151 microcontroller. It also includes an example circuit for providing
the panel supply voltages.

Note that the larger panels (such as the 800x600) consist of multiple segments
with separate gate driver signals. These can be daisy chained as shown in the
example schematic.




2. Frame buffer emulation

Because there is not enough memory to store a full frame buffer on the
processor, this driver emulates a frame buffer by storing the display data in
blocks. It will buffer up to EINK_NUMBUFFERS blocks and then write them to the
screen. The following #defines control the buffering. Larger numbers will
result in faster drawing, but also use more RAM on the processor:

#define EINK_BLOCKWIDTH 20    // Width of a single block in buffer
#define EINK_BLOCKHEIGHT 20   // Height of a single block in buffers
#define EINK_NUMBUFFERS 40    // Number of blocks to buffer

After drawing your images, you should flush the buffers using the following
command:

    gdispFlush();

The buffers are also flushed whenever you turn the display off using:

    gdispSetPowerMode(powerOff);



3. Display clearing and writing waveforms

This driver does not know the official Prime View waveforms for driving the
display, mostly because those are trade secrets and not publicly available.
Instead, it uses reverse engineered waveforms that are mostly functional.

The following #defines control the waveforms:

#define EINK_BLINKCLEAR TRUE      // Clear to opposite color first
#define EINK_CLEARCOUNT 10        // Number of sweeps to clear the display
#define EINK_WRITECOUNT 4         // Number of sweeps when writing to display

Increasing the clearcount and writecount can improve contrast, but will also
slow down the drawing and use more power.



4. Clock speeds

Because the driver bit bangs the display, the clock frequency of the processor
is quite significant. This is controlled by EINK_CLOCKDELAY variable. Set it
so that the delay is atleast 50 nanoseconds.



5. Support for other kinds of panels

Most of the Prime View panels should work using this driver, but only ED060SC4
has been tested so far. Some points of consideration:

- Some displays may use 4 bits per pixel. The driver currently assumes 2 bits
  per pixel.
- Larger displays may require some other way of daisy chaining than shown in
  the example schematics.

  
