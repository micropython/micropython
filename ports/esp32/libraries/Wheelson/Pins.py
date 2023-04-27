from micropython import const


class Pins:
    TFT_SCK: int = const(2)
    TFT_MOSI: int = const(13)
    TFT_DC: int = const(12)
    TFT_RST: int = const(0)
    TFT_CS: int = const(32)

    I2C_SDA: int = const(14)
    I2C_SCL: int = const(15)

    NUVO_RESET: int = const(33)


class Buttons:
    Up: int = const(3)
    Down: int = const(1)
    Left: int = const(0)
    Right: int = const(2)
    A: int = const(4)
    B: int = const(5)
