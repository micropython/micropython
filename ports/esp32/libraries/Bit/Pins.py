from micropython import const


class Pins:
    BL: int = const(12)
    BATT: int = const(10)

    TFT_SCK: int = const(13)
    TFT_MOSI: int = const(14)
    TFT_DC: int = const(15)
    TFT_RST: int = const(16)

    BUZZ: int = const(11)

    I2C_SDA: int = const(1)
    I2C_SCL: int = const(2)


class Buttons:
    Up: int = const(0)
    Down: int = const(1)
    Left: int = const(2)
    Right: int = const(3)
    A: int = const(4)
    B: int = const(5)
    C: int = const(6)

    # Maps Buttons [0-3] to their respective GPIO pins
    Pins: [int] = const([
        const(5),
        const(4),
        const(3),
        const(6),
        const(7),
        const(8),
        const(9)
    ])
