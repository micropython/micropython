from micropython import const


class Pins:
    BL = const(32)
    BUZZ = const(19)
    BATT = const(34)

    TFT_SCK = const(27)
    TFT_MOSI = const(26)
    TFT_DC = const(33)
    TFT_CS = const(0)
    TFT_RST = const(15)

    I2C_SDA = const(13)
    I2C_SCL = const(2)

    LORA_SCK = const(16)
    LORA_MOSI = const(5)
    LORA_MISO = const(17)
    LORA_CS = const(14)
    LORA_DIO1 = const(18)

    INP_SCK = const(22)
    INP_DATA = const(23)
    INP_PL = const(21)

class Buttons:
    Key_0 = const(15)
    Key_1 = const(11)
    Key_2 = const(12)
    Key_3 = const(3)
    Key_4 = const(10)
    Key_5 = const(13)
    Key_6 = const(2)
    Key_7 = const(9)
    Key_8 = const(14)
    Key_9 = const(1)
    Key_L = const(8)
    Key_R = const(0)

    Left = const(4)
    Right = const(5)
    Enter = const(6)
    Back = const(7)
