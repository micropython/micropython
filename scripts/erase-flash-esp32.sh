# Erase the flash memory of the ESP32 board

source env-variables-micropython.sh
source env-variables-esp32.sh

cd $MICROPYTHON/ports/esp32
make erase BOARD=$BOARD