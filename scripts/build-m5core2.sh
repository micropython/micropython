# Build MicroPython-LVGL firmware for: M5Stack Core2 ESP32

source env-variables-micropython.sh
source env-variables-esp32.sh

BOARD=M5STACK_CORE2

cd $MICROPYTHON
make -C mpy-cross

cd $MICROPYTHON/ports/esp32
make BOARD=$BOARD
