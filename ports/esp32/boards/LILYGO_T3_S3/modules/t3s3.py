# LILYGO T3-S3 MicroPython Board Helper
# Pin assignments from pins.csv, verified against schematic T3S3_V1.2

from machine import Pin
from micropython import const

_pins = Pin.board

_OLED_ADDR = const(0x3C)

# Battery voltage divider: R42=100K / R43=100K (VBAT/2 at ADC input)
_VBAT_DIVIDER_RATIO = 2.0


def get_battery_voltage():
    """Read battery voltage via ADC with voltage divider compensation.

    Returns voltage in volts (e.g. 3.85). Accuracy is approximate.
    """
    from machine import ADC

    adc = ADC(_pins.VBAT_SENSE)
    adc.atten(ADC.ATTN_11DB)
    raw_uv = adc.read_uv()
    return round(raw_uv / 1_000_000 * _VBAT_DIVIDER_RATIO, 2)


def set_led(state):
    """Turn on-board LED on (True) or off (False). LED is active HIGH."""
    Pin(_pins.LED, Pin.OUT).value(1 if state else 0)


def get_button():
    """Read boot button state. Returns True when pressed (active LOW)."""
    return Pin(_pins.BUTTON, Pin.IN).value() == 0


def get_oled():
    """Create and return an SSD1306 128x64 OLED display instance."""
    from machine import I2C
    import ssd1306

    i2c = I2C(0, scl=_pins.OLED_SCL, sda=_pins.OLED_SDA, freq=400000)
    return ssd1306.SSD1306_I2C(128, 64, i2c, addr=_OLED_ADDR)


def get_lora(lora_cfg=None):
    """Create and return a configured SX1262 LoRa modem for the T3-S3.

    Optional lora_cfg dict, e.g.:
        {"freq_khz": 869500, "sf": 7, "bw": "125", "coding_rate": 5, "output_power": 14}
    """
    from machine import SPI
    from lora import SX1262

    spi = SPI(
        1,
        baudrate=2000000,
        polarity=0,
        phase=0,
        sck=_pins.LORA_SCK,
        mosi=_pins.LORA_MOSI,
        miso=_pins.LORA_MISO,
    )
    return SX1262(
        spi=spi,
        cs=_pins.LORA_CS,
        busy=_pins.LORA_BUSY,
        dio1=_pins.LORA_DIO1,
        dio2_rf_sw=True,
        dio3_tcxo_millivolts=1800,
        reset=_pins.LORA_RST,
        lora_cfg=lora_cfg,
    )
