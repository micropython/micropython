from micropython import const


class Pins:
    I2C_SDA = const(23)
    I2C_SCL = const(22)

    INP_SCK = const(25)
    INP_DATA = const(27)
    INP_PL = const(19)

    I2S_BCK = const(21)
    I2S_WS = const(4)
    I2S_DO = const(14)
    I2S_DI = const(15)

    ENC_L1 = const(34)
    ENC_L2 = const(35)

    ENC_R1 = const(32)
    ENC_R2 = const(33)

    POT_L = const(39)
    POT_R = const(36)

    RGB_CLK = const(12)
    RGB_DATA = const({ 5, 2, 26, 13 })
class Buttons:
    BTN_5 = const(6)
    BTN_4 = const(3)
    BTN_3 = const(2)
    BTN_2 = const(1)
    BTN_1 = const(0)
    BTN_ENC_L = const(5)
    BTN_ENC_R = const(4)
