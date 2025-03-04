# Deploy firmware to device: M5Stack Core2 ESP32

source env-variables-micropython.sh
source env-variables-esp32.sh

BOARD=M5STACK_CORE2

cd $MICROPYTHON/ports/esp32
make deploy BOARD=$BOARD
