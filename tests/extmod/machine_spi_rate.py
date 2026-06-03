# Test machine.SPI data transfer rates

try:
    from machine import Pin, SPI
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    wr_short = b"abcdefghijklmnop" * 10
    rd_short = bytearray(len(wr_short))
    wr_long = wr_short * 20
    rd_long = bytearray(len(wr_long))
except MemoryError:
    print("SKIP")
    raise SystemExit

import time, sys
from target_wiring import spi_standalone_args_list

MAX_DELTA_MS = 8

# Tune test parameters based on the target.
if "alif" in sys.platform:
    MAX_DELTA_MS = 20
elif "esp8266" in sys.platform:
    MAX_DELTA_MS = 50  # port requires much looser timing requirements


def get_real_baudrate(spi):
    # Return the 'real' baudrate for a SPI object, from parsing 'print' output
    # i.e.
    # SPI(id=1, baudrate=500000, polarity=0, phase=0, bits=8, firstbit=0, sck=14, mosi=13, miso=12)
    #
    # Note the 'real' rate may be quite different to the requested rate, i.e.
    # on ports where the SPI hardware only supports power of 2 clock dividers.
    #
    # Implementation is somewhat fiddly and inefficient to avoid dependency on
    # 're' module,
    s = str(spi)
    s = s.split("baudrate=")[1].split(",")[0]
    return int(s)


def test_instances():
    print_results = True
    for spi_args in spi_standalone_args_list:
        for baudrate in (
            100_000,
            250_000,
            800_000,
            1_000_000,
            2_000_000,
            4_000_000,
            8_000_000,
        ):
            test_spi(spi_args, baudrate, 0, 0, print_results)

        for baudrate in (100_000, 4_000_000):
            # Test the other polarity and phase settings
            for polarity, phase in ((0, 1), (1, 0), (1, 1)):
                test_spi(spi_args, baudrate, polarity, phase, print_results)

        # Ensure the same test output regardless of how many SPI instances are tested
        print_results = False


def test_spi(spi_args, baudrate, polarity, phase, print_results):
    s = SPI(*spi_args, baudrate=baudrate, polarity=polarity, phase=phase)

    # to keep the test runtime down, use shorter buffer for lower baud rate
    wr_buf = wr_long if baudrate > 500_000 else wr_short
    rd_buf = rd_long if baudrate > 500_000 else rd_short

    real_baudrate = get_real_baudrate(s)
    assert real_baudrate
    transfer_time_ms = len(wr_buf) * 8 * 1000 // real_baudrate

    def test_write_readinto():
        s.write_readinto(wr_buf, rd_buf)

    def test_write():
        s.write(wr_buf)

    def test_readinto():
        s.readinto(rd_buf)

    for test_func, name in (
        (test_write_readinto, "write_readinto"),
        (test_write, "write"),
        (test_readinto, "readinto"),
    ):
        t0 = time.ticks_ms()
        test_func()
        transfer_time = time.ticks_diff(time.ticks_ms(), t0)
        t_delta = abs(transfer_time - transfer_time_ms)
        t_ok = t_delta < MAX_DELTA_MS

        if print_results or not t_ok:
            print(
                None if print_results else spi_args,
                baudrate,
                polarity,
                phase,
                name,
                t_ok or t_delta,
            )

    s.deinit()


test_instances()
