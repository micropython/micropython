# Test rp2 PIO support

import math
import rp2


def test_float_ratio():
    # Note Micropython only stores 21 of the 23 bits expected for IEEE
    assert rp2._float_ratio(3.14) == (205783, 65536)  # 3.139999
    assert rp2._float_ratio(math.pi) == (1647099, 524288)  # 3.141593
    assert rp2._float_ratio(3.141592654) == (1647099, 524288)
    assert rp2._float_ratio(120e6) == (120_000_000, 1)
    assert rp2._float_ratio(125e6) == (125_000_000, 1)


def test_pio_clock_divisor():
    for sys_clk, pio_clk, expected in (
        # Trivial example
        (125e6, 5e6, (25, 0, 0.0)),
        # stepdir examples
        (125_000_000, 3_125_000, (40, 0, 0.0)),
        (125e6, 3.125e6, (40, 0, 0.0)),
        (133_000_000, 3_125_000, (42, 143, -4394.531)),
        (133e6, 3.125e6, (42, 143, -4394.531)),
        # From table in description of #11593
        (120e6, 30 * 2560, (1562, 128, 0.0)),
        (120e6, 30 * 2560 * 1000 / 1001, (1564, 16, -41.79688)),
        (120e6, (30 * 2560 * 1000, 1001), (1564, 16, 0.0)),
        (120e6, 76723.27672, (1564, 16, -41.79688)),
        (120e6, (30 * 2560 * 1000, 1001), (1564, 16, 0.0)),
    ):
        print("pio, sys =", pio_clk, sys_clk)
        result = rp2.pio_clock_divisor(pio_clk, sys_clk)
        print(result)
        assert result[0] == expected[0]  # idiv
        assert result[1] == expected[1]  # fdiv
        assert abs(result[2] - expected[2]) < 0.001  # correction
        print()


test_float_ratio()
test_pio_clock_divisor()
