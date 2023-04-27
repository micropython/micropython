from micropython import const


class Pins:
    BL: int = const(18)
    BATT: int = const(34)
    CHARGE: int = const(35)

    TFT_SCK: int = const(16)
    TFT_MOSI: int = const(4)
    TFT_DC: int = const(17)
    TFT_RST: int = const(5)

    I2C_SDA: int = const(33)
    I2C_SCL: int = const(25)

    LED_R: int = const(21)
    LED_G: int = const(22)
    LED_B: int = const(23)

    BUZZ: int = const(19)


class Buttons:
    Left: int = const(0)
    Right: int = const(1)
    A: int = const(2)
    B: int = const(3)

    # Maps Buttons [0-3] to their respective GPIO pins
    Pins: [int] = const([
        const(32),
        const(27),
        const(26),
        const(14)
    ])
