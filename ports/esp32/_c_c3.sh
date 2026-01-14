#! /bin/bash
make BOARD=ESP32_GENERIC_C3
cp build-ESP32_GENERIC_C3/micropython.bin /mnt/d/jb/s1k/esp/mp_c3_jbg.bin
cp build-ESP32_GENERIC_C3/micropython.map /mnt/d/jb/s1k/esp/mp_c3_jbg.map
cp build-ESP32_GENERIC_C3/micropython.bin /mnt/c/jb/dev/esp/c3/mp_c3_jbg.bin
cp build-ESP32_GENERIC_C3/micropython.map /mnt/c/jb/dev/esp/c3/mp_c3_jbg.map
#
