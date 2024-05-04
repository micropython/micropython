# The RP2 port

This is a port of MicroPython to the Raspberry Pi RP2 series of microcontrollers.
Currently supported features are:

- REPL over USB VCP, and optionally over UART (on GP0/GP1).
- Filesystem on the internal flash, using littlefs2.
- Support for native code generation and inline assembler.
- `time` module with sleep, time and ticks functions.
- `os` module with VFS support.
- `machine` module with the following classes: `Pin`, `ADC`, `PWM`, `I2C`, `SPI`,
  `SoftI2C`, `SoftSPI`, `Timer`, `UART`, `WDT`.
- `rp2` module with programmable IO (PIO) support.

See the `examples/rp2/` directory for some example code.

## Building

The MicroPython cross-compiler must be built first, which will be used to
pre-compile (freeze) built-in Python code.  This cross-compiler is built and
run on the host machine using:

    $ make -C mpy-cross

This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/rp2/ directory.

Building of the RP2 firmware is done entirely using CMake, although a simple
Makefile is also provided as a convenience.  To build the firmware run (from
this directory):

    $ make submodules
    $ make clean
    $ make

You can also build the standard CMake way.  The final firmware is found in
the top-level of the CMake build directory (`build` by default) and is
called `firmware.uf2`.

If you are using a board other than a Raspberry Pi Pico, you should
pass the board name to the build; e.g. for Raspberry Pi Pico W:

    $ make BOARD=RPI_PICO_W submodules
    $ make BOARD=RPI_PICO_W clean
    $ make BOARD=RPI_PICO_W

## Deploying firmware to the device

Firmware can be deployed to the device by putting it into bootloader mode
(hold down BOOTSEL while powering on or resetting) and then copying
`firmware.uf2` to the USB mass storage device that appears.

If MicroPython is already installed then the bootloader can be entered by
executing `import machine; machine.bootloader()` at the REPL.

## Sample code

The following samples can be easily run on the board by entering paste mode
with Ctrl-E at the REPL, then cut-and-pasting the sample code to the REPL, then
executing the code with Ctrl-D.

### Blinky

This blinks the on-board LED on the Pico board at 1.25Hz, using a Timer object
with a callback.

```python
from machine import Pin, Timer
led = Pin(25, Pin.OUT)
tim = Timer()
def tick(timer):
    global led
    led.toggle()

tim.init(freq=2.5, mode=Timer.PERIODIC, callback=tick)
```

### PIO blinky

This blinks the on-board LED on the Pico board at 1Hz, using a PIO peripheral and
PIO assembler to directly toggle the LED at the required rate.

```python
from machine import Pin
import rp2

@rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
def blink_1hz():
    # Turn on the LED and delay, taking 1000 cycles.
    set(pins, 1)
    set(x, 31)                  [6]
    label("delay_high")
    nop()                       [29]
    jmp(x_dec, "delay_high")

    # Turn off the LED and delay, taking 1000 cycles.
    set(pins, 0)
    set(x, 31)                  [6]
    label("delay_low")
    nop()                       [29]
    jmp(x_dec, "delay_low")

# Create StateMachine(0) with the blink_1hz program, outputting on Pin(25).
sm = rp2.StateMachine(0, blink_1hz, freq=2000, set_base=Pin(25))
sm.active(1)
```

See the `examples/rp2/` directory for further example code.
