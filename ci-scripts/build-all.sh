#!/usr/bin/env sh
set -x

PORTS="stm32 qemu-arm unix-coverage unix-standard unix-nanobox unix-stackless windows esp32 esp8266 nrf bare-arm cc3200 samd teensy"

for PORT in $PORTS; do
  docker run -d --name mpy_build_$PORT -v $PWD:$PWD -w $PWD $IMAGE ci-scripts/$PORT.sh
done

for PORT in $PORTS; do
  docker wait $(docker ps --format '{{.Names}}' | grep 'mpy_build')
done
