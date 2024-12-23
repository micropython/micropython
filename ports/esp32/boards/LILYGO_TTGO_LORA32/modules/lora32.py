"""LILYGO TTGO LoRa32 MicroPython Helper Library."""

from machine import Pin, SoftI2C, Signal

from lilygo_oled import OLED

from micropython import const


class Lora32Base:
    """Base class defining common pins."""

    def __init__(self, define_helpers=True):
        # LORA
        self.LORA_MOSI = const(27)
        self.LORA_MISO = const(19)
        self.LORA_SCLK = const(5)
        self.LORA_CS = const(18)
        self.LORA_DIO = const(26)
        self.LORA_RST = const(23)

        # DAC
        self.DAC1 = const(26)

        # LED
        self.LED = const(25)

        # OLED
        self.OLED_SDA = const(21)
        self.OLED_SCL = const(22)
        self.OLED_RST = None

        if define_helpers:
            self.create_helpers()

    def create_helpers(self):
        self.led = Pin(self.LED, Pin.OUT)
        self.i2c = SoftI2C(scl=Pin(self.OLED_SCL), sda=Pin(self.OLED_SDA))
        rstpin = self.OLED_RST is not None and Pin(self.OLED_RST, Pin.OUT) or None
        self.oled = OLED(self.i2c, rstpin)


class Lora32v1_0(Lora32Base):
    """Device Support for LILYGO TTGO LoRa32 v1.0."""

    def __init__(self):
        super().__init__(define_helpers=False)

        # v1.0 has different pins for the following
        self.LORA_RST = const(14)
        self.OLED_SDA = const(4)
        self.OLED_SCL = const(15)

        # Also has a reset for the OLED that the others don't have
        self.OLED_RST = const(16)

        super().create_helpers()


class Lora32v1_2(Lora32Base):
    """Device Support for LILYGO TTGO LoRa32 v1.2 (T-Fox)."""

    def __init__(self):
        super().__init__()

        # v1.2 Has a DS3231 RTC
        self.DS3231_SDA = const(21)
        self.DS3231_SCL = const(22)


class Lora32(Lora32Base):
    """Device Support for LILYGO TTGO LoRa32 v1.6 and v2.0."""

    def __init__(self):
        super().__init__()

        # v1.6 and v2.0 support an SDCard
        self.SD_CS = const(13)
        self.SD_MOSI = const(15)
        self.SD_MISO = const(2)
        self.SD_SCLK = const(14)
