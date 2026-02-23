// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Cytron MOTION 2350 Pro"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1024 * 1024)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO16)

// Enable USB Mass Storage with FatFS filesystem.
#define MICROPY_HW_USB_MSC              (1)
#define MICROPY_HW_USB_VID              (0x2e8a)
#define MICROPY_HW_USB_PID              (0x0005)
