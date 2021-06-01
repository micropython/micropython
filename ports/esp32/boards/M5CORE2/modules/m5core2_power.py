import axp192
from time import sleep

SDA = 21
SCL = 22
I2C_ADDRESS = 0x34

ESP_POWER       = axp192.DCDC1_VOLTAGE
LCD_BACKLIGHT   = axp192.DCDC3_VOLTAGE
LOGIC_AND_SD    = axp192.LDO2_VOLTAGE
VIBRATOR        = axp192.LDO3_VOLTAGE

class Power():

    def __init__(self, i2c_dev=0, sda=SDA, scl=SCL, freq=400000, i2c_addr=I2C_ADDRESS, skip_init=False):
        self.axp = axp192.AXP192(i2c_dev, sda, scl, freq, i2c_addr)

        if skip_init:
            return

        print("Initialising M5Core2 power management")
        
        self.axp.twiddle(axp192.VBUS_IPSOUT_CHANNEL, 0b11111011, 0x02)
        print("  Vbus limit off")

        self.axp.twiddle(axp192.GPIO2_CONTROL, 0b00000111, 0x00)
        self.speaker(False)
        print("  Speaker amplifier off")
    
        self.axp.twiddle(axp192.BATTERY_CHARGE_CONTROL, 0b11100011, 0b10100010)
        print("  RTC battery charging enabled (3v, 200uA)")
    
        # If you set this too low or turn it off, you will need to do I2C mouth-to-mouth
        # with another device to get your M5Core2 to come alive again. So don't do that.
        self.axp.write(ESP_POWER, 3.35)
        print("  ESP32 power voltage set to 3.35v")

        self.axp.write(LCD_BACKLIGHT, 2.8)
        print("  LCD backlight voltage set to 2.80v")

        self.axp.write(LOGIC_AND_SD, 3.3)
        print("  LCD logic and sdcard voltage set to 3.3v")

        self.axp.twiddle(axp192.GPIO1_CONTROL, 0x07, 0x00)
        self.led(True)
        print("  LED on")

        self.axp.twiddle(axp192.CHARGE_CONTROL_1, 0x0f, 0x00)
        print("  Charge current set to 100 mA")

        print("  Battery voltage now: {}v".format(self.axp.read(axp192.BATTERY_VOLTAGE)))

        self.axp.twiddle(axp192.PEK, 0xff, 0x4c)
        print("  Power key set, 4 seconds for hard shutdown")

        self.axp.twiddle(axp192.ADC_ENABLE_1, 0x00, 0xff)
        print("  Enabled all ADC channels")

        self.int_5v(True)
        print("  USB / battery powered, 5V bus on")

        self.axp.twiddle(axp192.GPIO40_FUNCTION_CONTROL, 0x8d, 0x84)
        self.axp.twiddle(axp192.GPIO40_SIGNAL_STATUS, 0x02, 0x00)
        sleep(0.1)
        self.axp.twiddle(axp192.GPIO40_SIGNAL_STATUS, 0x02, 0x02)
        print("  LCD and touch reset")
        sleep(0.1)

    def led(self, state):
        self.axp.twiddle(axp192.GPIO20_SIGNAL_STATUS, 0x02, 0x00 if state else 0x02)

    def speaker(self, state):
        self.axp.twiddle(axp192.GPIO20_SIGNAL_STATUS, 0x04, 0x04 if state else 0x00)
    
    # True turns on at 2V, False or 0 turns off. Alternatively, specify a voltage in range 1.8 - 3.3 volts.
    def vibrator(self, state):
        if type(state) == bool and state:
            self.axp.write(VIBRATOR, 2.0)
        else:
            self.axp.write(VIBRATOR, state)

    def int_5v(self, state):
        if state:
            self.axp.twiddle(axp192.GPIO0_LDOIO0_VOLTAGE, 0xf0, 0xf0)
            self.axp.twiddle(axp192.GPIO0_CONTROL, 0x07, 0x02)
            self.axp.twiddle(axp192.DCDC13_LDO23_CONTROL, axp192.BIT_EXTEN_ENABLE, axp192.BIT_EXTEN_ENABLE)
        else:
            self.axp.twiddle(axp192.DCDC13_LDO23_CONTROL, axp192.BIT_EXTEN_ENABLE, 0)
            self.axp.twiddle(axp192.GPIO0_CONTROL, 0x07, 0x01)
        
    