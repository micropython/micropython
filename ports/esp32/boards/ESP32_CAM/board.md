The following files are firmware that work on the ESP32 CAM module using 
an OV2640 camera.

ESP32 CAM has PSRAM (aka spiram) but the ESP32-S chip on it does not support bluetooth

To build this from source, load your env vars:-

    . $HOME/esp/esp-idf/export.sh

Open a shell in the folder micropython/ports/esp32

and run this command:-

    git clone https://github.com/lemariva/micropython-camera-driver.git
    git clone https://github.com/espressif/esp32-camera
    make USER_C_MODULES=../micropython-camera-driver/src/micropython.cmake BOARD=ESP32_CAM all

-or- (for ota support):-

    make USER_C_MODULES=../micropython-camera-driver/src/micropython.cmake BOARD=ESP32_CAM MICROPY_BOARD_VARIANT=OTA
