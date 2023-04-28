from micropython import const


class Pins:
    LED: int = const(26)
    BTN: int = const(17)
    I2C_SDA: int = const(22)
    I2C_SCL: int = const(21)
