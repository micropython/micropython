rm -rf atmel-samd/build*
rm -rf esp8266/build*

ATMEL_BOARDS="arduino_zero circuitplayground_express feather_m0_basic feather_m0_adalogger feather_m0_express metro_m0_express trinket_m0 gemma_m0"

for board in $ATMEL_BOARDS; do
    make -C atmel-samd BOARD=$board
    (( exit_status = exit_status || $? ))
done
if [ -z "$TRAVIS" ]; then
    make -C esp8266 BOARD=feather_huzzah
    (( exit_status = exit_status || $? ))
fi

version=`git describe --tags --exact-match`
if [ $? -ne 0 ]; then
    version=`date +%Y%m%d`-`git rev-parse --short HEAD`
fi

for board in $ATMEL_BOARDS; do
    mkdir -p bin/$board/
    cp atmel-samd/build-$board/firmware.bin bin/$board/adafruit-circuitpython-$board-$version.bin
    (( exit_status = exit_status || $? ))
    cp atmel-samd/build-$board/firmware.uf2 bin/$board/adafruit-circuitpython-$board-$version.uf2
    (( exit_status = exit_status || $? ))
done

# Skip ESP8266 on Travis
if [ -z "$TRAVIS" ]; then
    mkdir -p bin/esp8266/
    cp esp8266/build/firmware-combined.bin bin/esp8266/adafruit-circuitpython-feather_huzzah-$version.bin
    (( exit_status = exit_status || $? ))
fi

exit $exit_status
