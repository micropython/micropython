# Deploy firmware to device: Generic ESP32 boards

source env-variables-micropython.sh
source env-variables-esp32.sh

BOARD=ESP32_GENERIC
VARIANT=SPIRAM

cd $MICROPYTHON/ports/esp32
make deploy BOARD=$BOARD VARIANT=$VARIANT
