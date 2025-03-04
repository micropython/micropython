# Build MicroPython-LVGL firmware for: Generic ESP32 boards

source env-variables-micropython.sh
source env-variables-esp32.sh

BOARD=ESP32_GENERIC
VARIANT=SPIRAM

cd $MICROPYTHON
make -C mpy-cross

cd $MICROPYTHON/ports/esp32
make submodules
make BOARD=$BOARD VARIANT=$VARIANT
