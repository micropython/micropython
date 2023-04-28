from micropython import const


class Pins:
    I2C_SDA = const(26)
    I2C_SCL = const(27)

    BL = const(25)

    SPI_SCK = const(18)
    SPI_MISO = const(19)
    SPI_MOSI = const(23)

    SD_CS = const(22)

    TFT_CS = const(32)
    TFT_DC = const(33)
    TFT_RST = const(2)

    NUVO_RESET: int = const(13)


class Buttons:
    Left = 0
    Right = 1
    Enc_Mid = const(2)
    Enc_Left1 = const(3)
    Enc_Left2 = const(8)
    Enc_Left3 = const(7)
    Enc_Right1 = const(6)
    Enc_Right2 = const(5)
    Enc_Right3 = const(4)


class Encoders:
    Mid = 0
    Left1 = const(1)
    Left2 = const(6)
    Left3 = const(5)
    Right1 = const(4)
    Right2 = const(3)
    Right3 = const(2)


class Sliders:
    Mid = const(0)
    Left = const(1)
    Right = const(2)
