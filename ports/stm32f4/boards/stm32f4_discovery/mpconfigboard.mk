USB_VID = 0x239A
USB_PID = 0x808A
USB_PRODUCT = "STM32F407VG Discovery Board - CPy"
USB_MANUFACTURER = "STMicroelectronics"
USB_DEVICES = "CDC,MSC"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE

# This is technically a F407 but there's no difference
# other than the camera and ethernet, which aren't supported. 
MCU_SERIES = m4
MCU_VARIANT = stm32f4
MCU_SUB_VARIANT = stm32f407xx
MCU_PACKAGE = 100
CMSIS_MCU = STM32F407xx
LD_FILE = boards/STM32F407_fs.ld

