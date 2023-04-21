from micropython import const

pinMap = [0]*11


class Pins:
    SPEAKER = const(25)
    SPEAKER_EN = const(13)
    BL = 0
    BATT = const(36)
    CHARGE = 4
    SD_SENSE = 5

    LED_R = 1
    LED_G = 2
    LED_B = 3

    SPI_SCK = 6
    SPI_MISO = const(5)
    SPI_MOSI = const(32)

    SD_CS = const(2)

    TFT_DC = const(21)
    TFT_RST = const(27)
    TFT_MISO = 7
    TFT_MOSI = 8
    TFT_CS = 9
    TFT_SCK = 10

    I2C_SDA = const(23)
    I2C_SCL = const(22)

    SHIFT_CLK = const(15)
    SHIFT_DAT = const(4)
    SHIFT_PL = const(0)

class Buttons:
    Up = const(0)
    Down = const(3)
    Left = const(1)
    Right = const(2)
    A = const(6)
    B = const(5)
    C = const(4)


def init_pins(variant: int):
    if variant == 0:
        pinMap[0] = const(12)  # BL_PIN

        pinMap[1] = const(14)  # LED_R
        pinMap[2] = const(15)  # LED_G
        pinMap[3] = const(13)  # LED_B
        pinMap[4] = const(10)  # CHARGE_DETECT_PIN
        pinMap[5] = const(8)  # SD_DETECT_PIN

        pinMap[6] = const(26)  # SPI_SCK

        pinMap[7] = const(5)
        pinMap[8] = const(32)
        pinMap[9] = const(33)
        pinMap[10] = -1

    elif variant == 1:
        pinMap[0] = const(18)  # BL_PIN

        pinMap[1] = const(22)  # LED_R
        pinMap[2] = const(23)  # LED_G
        pinMap[3] = const(19)  # LED_B
        pinMap[4] = const(35)  # CHARGE_DETECT_PIN
        pinMap[5] = const(7)  # SD_DETECT_PIN

        pinMap[6] = const(14)  # SPI_SCK

        pinMap[7] = -1  # TFT_MISO
        pinMap[8] = const(33)  # TFT_MOSI
        pinMap[9] = -1  # TFT_CS <- CS isn't connected in this variant
        pinMap[10] = const(26)  # TFT_SCK

        Buttons.Up = const(7)
        Buttons.Down = const(4)
        Buttons.Left = const(5)
        Buttons.Right = const(6)
        Buttons.A = const(3)
        Buttons.B = const(2)
        Buttons.C = const(1)

    Pins.BL = pinMap[0]
    Pins.LED_R = pinMap[1]
    Pins.LED_G = pinMap[2]
    Pins.LED_B = pinMap[3]
    Pins.CHARGE = pinMap[4]
    Pins.SD_SENSE = pinMap[5]
    Pins.SPI_SCK = pinMap[6]
    Pins.TFT_MISO = pinMap[7]
    Pins.TFT_MOSI = pinMap[8]
    Pins.TFT_CS = pinMap[9]
    Pins.TFT_SCK = pinMap[10]
