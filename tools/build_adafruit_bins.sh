rm -rf ports/atmel-samd/build*
rm -rf ports/esp8266/build*
rm -rf ports/nrf/build*

ATMEL_BOARDS="arduino_zero circuitplayground_express feather_m0_basic feather_m0_adalogger itsybitsy_m0_express itsybitsy_m4_express feather_m0_rfm69 feather_m0_rfm9x feather_m0_express feather_m4_express metro_m0_express metro_m4_express pirkey_m0 trinket_m0 gemma_m0 feather52"
ROSIE_SETUPS="rosie-ci"

PARALLEL="-j 5"
if [ "$TRAVIS" == "true" ]; then
    PARALLEL="-j 2"
fi

if [ -z "$TRAVIS_BOARD" ]; then
    boards=$ATMEL_BOARDS
else
    boards=$TRAVIS_BOARD
fi

for board in $boards; do
    if [ $board == "feather52" ]; then
        make $PARALLEL -C ports/nrf BOARD=feather52
        (( exit_status = exit_status || $? ))
    else
        make $PARALLEL -C ports/atmel-samd BOARD=$board
        (( exit_status = exit_status || $? ))
    fi
done
if [ -z "$TRAVIS" ]; then
    make $PARALLEL -C ports/esp8266 BOARD=feather_huzzah
    (( exit_status = exit_status || $? ))
fi

version=`git describe --tags --exact-match`
if [ $? -ne 0 ]; then
    version=`date +%Y%m%d`-`git rev-parse --short HEAD`
fi


if [ "$TRAVIS" == "true" ]; then
    sha=$TRAVIS_COMMIT
    if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
       version=`date +%Y%m%d`-`echo $TRAVIS_PULL_REQUEST_SHA | cut -c1-7`
       sha=$TRAVIS_PULL_REQUEST_SHA
    fi
fi

for board in $boards; do
    mkdir -p bin/$board/
    if [ $board == "feather52" ]; then
        cp ports/nrf/build-$board-s132/firmware.bin bin/$board/adafruit-circuitpython-$board-$version.bin
        (( exit_status = exit_status || $? ))
    else
        cp ports/atmel-samd/build-$board/firmware.bin bin/$board/adafruit-circuitpython-$board-$version.bin
        (( exit_status = exit_status || $? ))
        cp ports/atmel-samd/build-$board/firmware.uf2 bin/$board/adafruit-circuitpython-$board-$version.uf2
        (( exit_status = exit_status || $? ))
    fi
    # Only upload to Rosie if its a pull request.
    if [ "$TRAVIS" == "true" ]; then
        for rosie in $ROSIE_SETUPS; do
            echo "Uploading to https://$rosie.ngrok.io/upload/$sha"
            curl -F "file=@bin/$board/adafruit-circuitpython-$board-$version.uf2" https://$rosie.ngrok.io/upload/$sha
        done
    fi
done

# Skip ESP8266 on Travis
if [ -z "$TRAVIS" ]; then
    mkdir -p bin/esp8266/
    cp ports/esp8266/build/firmware-combined.bin bin/esp8266/adafruit-circuitpython-feather_huzzah-$version.bin
    (( exit_status = exit_status || $? ))
fi

exit $exit_status
