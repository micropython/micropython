rm -rf atmel-samd/build*
rm -rf esp8266/build*

ATMEL_BOARDS="arduino_zero circuitplayground_express feather_m0_basic feather_m0_adalogger feather_m0_express metro_m0_flash trinket_m0 gemma_m0"

for board in $ATMEL_BOARDS; do
    make -C atmel-samd BOARD=$board
done
make -C esp8266 BOARD=feather_huzzah

version=`git describe --tags --exact-match`
if [ $? -ne 0 ]; then
    version=`git rev-parse --short HEAD`
fi

for board in $ATMEL_BOARDS; do
    cp atmel-samd/build-$board/firmware.bin bin/adafruit-circuitpython-$board-$version.bin
done
cp esp8266/build/firmware-combined.bin bin/adafruit-circuitpython-feather_huzzah-$version.bin
