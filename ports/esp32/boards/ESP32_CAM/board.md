The following files are firmware that work on the ESP32 CAM module using 
an OV2640 camera.

ESP32 CAM has PSRAM (aka spiram) but the ESP32-S chip on it does not support bluetooth

To build this from source, load your env vars:-

    . $HOME/esp/esp-idf/export.sh

Open a shell in the folder micropython/ports/esp32

and run this command:-

    git clone https://github.com/lemariva/micropython-camera-driver.git
    git clone https://github.com/espressif/esp32-camera
    make BOARD=ESP32_CAM submodules
    make USER_C_MODULES=../micropython-camera-driver/src/micropython.cmake BOARD=ESP32_CAM all

-or- (for ota support):-

    make USER_C_MODULES=../micropython-camera-driver/src/micropython.cmake BOARD=ESP32_CAM MICROPY_BOARD_VARIANT=OTA

then flash is like this:-

    esptool.py -p $PORT write_flash --flash_mode dio --flash_size 4MB --flash_freq 40m 0x1000 build-ESP32_CAM/bootloader/bootloader.bin 0x8000 build-ESP32_CAM/partition_table/partition-table.bin 0x10000 build-ESP32_CAM/micropython.bin

Note that these boards wire RTS and DSR to reset and gpio0 pins, so you need the fixed pyboard.py that includes the --hard-reset to talk to them.  For example:-

    pyboard.py --device $PORT --hard-reset -f ls
