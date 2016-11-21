rm -rf atmel-samd/build*
rm -rf esp8266/build*

make -C atmel-samd BOARD=feather_m0_basic
make -C atmel-samd BOARD=feather_m0_flash
make -C atmel-samd BOARD=metro_m0_flash
make -C esp8266 BOARD=feather_huzzah

version=`git describe --tags --exact-match`
if [ $? -ne 0 ]; then
    version=`git rev-parse --short HEAD`
fi

cp atmel-samd/build-feather_m0_basic/firmware.bin bin/adafruit-micropython-feather_m0_basic-$version.bin
cp atmel-samd/build-feather_m0_flash/firmware.bin bin/adafruit-micropython-feather_m0_flash-$version.bin
cp atmel-samd/build-metro_m0_flash/firmware.bin bin/adafruit-micropython-metro_m0_flash-$version.bin
cp esp8266/build/firmware-combined.bin bin/adafruit-micropython-feather_huzzah-$version.bin
