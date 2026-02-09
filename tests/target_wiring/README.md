# Target wiring

Some tests require hardware configuration and/or external connections, for example
bridging a pair of GPIO pins.  Each board that such tests run on needs to be configured
individually.  That is achieved by providing a target wiring configuration script that
defines the necessary hardware parameters for each test.

## Selecting the target wiring

There are three ways to provide the target wiring configuration:

1. Specify it explicitly when running the test: `./run-tests.py --target-wiring <script>`

2. Have a `target_wiring.py` file on the target device somewhere in the import path.

3. Let the configuration be selected automatically.  In that case the best matching file
   from this directory is chosen using the following logic: prefer an exact filename
   match to `sys.implementation._build`, followed by a match to the start of
   `sys.implementation._build` if the filename ends in "x", followed by a filename
   that matches the port of the target device.

## Target wiring variables

A target wiring script should define the following variables if the corresponding test
is to be supported.

### UART tests

UART tests require one UART to be connected in loopback mode, ie TX connected physically
to RX. The variables are:

    uart_loopback_args: tuple
    uart_loopback_kwargs: dict

The UART instance will be created using:

    machine.UART(*uart_loopback_args, **uart_loopback_kwargs)

### SPI tests

SPI tests require one or more SPI instances.  They don't require any connections.
The variables are:

    spi_standalone_args_list: list[tuple]

SPI instances will be created using:

    for spi_args in spi_standalone_args_list:
        machine.SPI(*spi_args)

### Encoder tests

Encoder tests require one encoder to be connected in loopback mode to two other GPIO
pins. The variables are:

    encoder_loopback_id: int
    encoder_loopback_out_pins: tuple[Any, Any]
    encoder_loopback_in_pins: tuple[Any, Any]

The Encoder instance will be created using:

    out0_pin, out1_pin = encoder_loopback_out_pins
    in0_pin, in1_pin = encoder_loopback_in_pins
    machine.Encoder(id, in0_pin, in1_pin)

The two elements in both `encoder_loopback_out_pins` and `encoder_loopback_in_pins` will
be used to create individual `machine.Pin` instances.
