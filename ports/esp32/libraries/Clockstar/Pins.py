from micropython import const


class Pins:
    BL: int = const(4)
    BATT: int = const(36)
    CHARGE: int = const(19)

    TFT_SCK: int = const(16)
    TFT_MOSI: int = const(17)
    TFT_DC: int = const(5)
    TFT_RST: int = const(18)

    I2C_SDA: int = const(26)
    I2C_SCL: int = const(25)

    LED_R: int = const(21)
    LED_G: int = const(22)
    LED_B: int = const(23)

    BUZZ: int = const(2)


class Buttons:
    Up: int = const(0)
    Down: int = const(1)
    Select: int = const(2)
    Back: int = const(3)

    # Maps Buttons [0-3] to their respective GPIO pins
    Pins: [int] = const([
        const(15),
        const(14),
        const(27),
        const(39)
    ])
