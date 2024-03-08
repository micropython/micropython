from micropython import const


class Pins:
    BL: int = const(37)
    BATT: int = const(6)
    CHARGE: int = const(47)

    TFT_SCK: int = const(41)
    TFT_MOSI: int = const(40)
    TFT_DC: int = const(39)
    TFT_RST: int = const(38)

    I2C_SDA: int = const(35)
    I2C_SCL: int = const(36)

    RGB_R: int = const(33)
    RGB_G: int = const(34)
    RGB_B: int = const(48)

    BUZZ: int = const(11)

    LED_1: int = const(46)
    LED_2: int = const(45)
    LED_3: int = const(44)
    LED_4: int = const(43)
    LED_5: int = const(18)
    LED_6: int = const(17)

class Buttons:
    Up: int = const(0)
    Down: int = const(1)
    Select: int = const(2)
    Back: int = const(3)

    # Maps Buttons [0-3] to their respective GPIO pins
    Pins: [int] = const([
        const(9),
        const(10),
        const(8),
        const(21)
    ])
