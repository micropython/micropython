for PORT in stm32 qemu-arm unix-coverage unix-standard unix-nanobox unix-stackless windows esp32 esp8266 nrf bare-arm cc3200 samd teensy
do
  docker run -v $PWD:$PWD -w $PWD $IMAGE ci-scripts/$PORT.sh > logs || cat logs
done
