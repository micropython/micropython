Micropython on ESP8266
======================

Build Instructions
------------------

Build the docker image of the master branch.

```bash
  docker build -t micropython .
```

To specify a particular version of micropython provide it through the `build-arg`. Otherwise the HEAD of the master branch will be used.

```bash
  docker build -t micropython --build-arg VERSION=v1.8.7 .
```


Once the container is built successfuly create a container from the image

```bash
  docker create --name micropython micropython
```

Then copy the the built firmware into the host machine.

```bash
  docker cp micropython:/micropython/esp8266/build/firmware-combined.bin firmware-combined.bin
```

The firmware can then be uploaded with the esptool

```bash
  esptool.py --port ${SERIAL_PORT} --baud 115200 write_flash --verify --flash_size=8m 0 firmware-combined.bin
```

Here `${SERIAL_PORT}` is the path to the serial device on which the board is connected.

Flash from within Container
---------------------------

If you have built the image directly on your host (Linux), you also can flash your ESP directly by running a container from the image.
I prefereably **erase** flash memory of ESP8266 before starting flash a new firmware

```bash
docker run --rm -it --device ${SERIAL_PORT} --user root --workdir /micropython/esp8266 micropython make PORT=${SERIAL_PORT} erase deploy
```

Here `${SERIAL_PORT}` is the path to the serial device on which the board is connected, generally is equal to /dev/ttyUSB0.


Freeze personal script files in the build
-----------------------------------------

If you want to add personal python scripts to include in the build flash image, you have to add them to the folder **/micropython/esp8266/modules**.
The building process will precompiles your scripts with **MPY** and will inserts in the flash image this option will save you more memory of the microcontroller.

To obtain this within the docker container, create a copy of the original micropython folder **/micropython/esp8266/modules** in your working directrory
add here your scripts and link them into the container (with the -v docker option) overriding the default **modules** folder, when you run the container.


```bash
docker run --rm -it -v $(pwd)/modules:/micropython/esp8266/modules --device ${SERIAL_PORT} --user root --workdir /micropython/esp8266 esp /bin/bash
make clean
make
make PORT=/dev/ttyUSB0 erase deploy
```

Here `${SERIAL_PORT}` is the path to the serial device on which the board is connected, generally is equal to /dev/ttyUSB0.

Trash

Делаем мапинг папки в Виртуальную машину.
/mnt/hgfs/docker/micropython/

```mpy-cross

Запускаем контейнер из image.
docker run -it --rm -u root -v /mnt/hgfs/docker/micropython:/mnt/dev micropython bash

Компилируем питон файлы что не влезают в RAM.
mpy-cross simple.py


Пробрасываем в Хост USB.

Запускаем докер со всеми USB
docker run -it --rm -u root  --privileged -v /dev/bus/usb:/dev/bus/usb -v /mnt/hgfs/docker/micropython:/mnt/dev micropython bash

Запускаем докер с конкретным USB
docker run -it --rm -u root  --device=/dev/ttyUSB0 -v /mnt/hgfs/docker/micropython:/mnt/dev micropython bash

Проверяем USB
lsusb


```Flash
	```WEMOS

	esptool.py --port /dev/ttyUSB0 erase_flash
	esptool.py --baud 230400 --port /dev/ttyUSB0 write_flash -fm dio -fs 32m -ff 40m 0x00000 firmware-combined.bin

	```Esp-07

	Вначале USB-TTL потом ESP
		esptool.py --port /dev/ttyUSB0 erase_flash
		ESP - power cycle
    esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash -fm dio -fs 32m -ff 40m 0x00000 firmware-combined.bin

  ```Electrodragon http://www.electrodragon.com/w/index.php?title=ESP_Relay_Board_Series
    1. Connect USB-TTL and connect to PC.
    2. Power On Dragon with BTN2
    3. esptool.py --port /dev/ttyUSB0 erase_flash
    4. ESP - power cycle with BTN2
    5. Flash - esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash -fm dio -fs 32m -ff 40m 0x00000 firmware-combined.bin


  ```Com port Terminal

      Connect to com
      picocom /dev/ttyUSB0 -b115200

      Exit from picocom CTR+A+X



First Setup:

  1. picocom /dev/ttyUSB0 -b115200
  2. Настройка webrepl    
      import webrepl_setup

import network
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('<your ESSID>', '<your password>')
sta_if.ifconfig()


Broker:

    CONFIG['broker'] = '192.168.2.138'
    save_config()
