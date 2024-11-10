
# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pico & Typing.py + USB Mass Storage"

    MICROPY_HW_USB_MANUFACTURER_STRING="MicroPython"
    MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING="Verlinde"
    # make sure the C version is not also included to avoid conflicts
    MICROPY_PY_TYPING=0
    MICROPY_PY_TYPING_EXTRA_MODULES=0

    #Enable USB Mass Storage with FatFS filesystem.
    MICROPY_HW_USB_MSC=1
    
)
# include the correct manifest file
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest_TYPING.py)
