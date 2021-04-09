USB_VID = 0x239A
USB_PID = 0x80DB
USB_PRODUCT = "TG-Watch"
USB_MANUFACTURER = "TG-Techie"

MCU_CHIP = nrf52840

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "GD25Q16C, W25Q128JVxQ"

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ST7789
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Shapes
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ProgressBar
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LSM6DS
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_FocalTouch
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DS3231
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LC709203F
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DRV2605
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BLE
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BLE_Apple_Notification_Center
