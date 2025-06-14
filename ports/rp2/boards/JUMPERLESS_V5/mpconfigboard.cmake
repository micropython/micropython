# cmake file for Jumperless v5
set(PICO_BOARD "jumperless-v5")
set(PICO_PLATFORM "rp2350")

# This was copied from SparkFun IoT RedBoard RP350, not sure if needed
# TODO: DELETE THIS LINE WHEN SUBMODULED PICO-SDK INCLUDES THIS BOARD
set(PICO_BOARD_HEADER_DIRS ${MICROPY_PORT_DIR}/boards/${MICROPY_BOARD})

set(PICO_NUM_GPIOS 48)