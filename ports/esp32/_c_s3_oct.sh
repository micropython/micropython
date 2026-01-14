#! /bin/bash
make BOARD=ESP32_GENERIC_S3 BOARD_VARIANT=SPIRAM_OCT 	
cp build-ESP32_GENERIC_S3-SPIRAM_OCT/micropython.bin /mnt/d/jb/s1k/esp/mp_s3_oct_jbg.bin
cp build-ESP32_GENERIC_S3-SPIRAM_OCT/micropython.map /mnt/d/jb/s1k/esp/mp_s3_oct_jbg.map
cp build-ESP32_GENERIC_S3-SPIRAM_OCT/micropython.bin /mnt/c/jb/dev/esp/s3/mp_s3_oct_jbg.bin
cp build-ESP32_GENERIC_S3-SPIRAM_OCT/micropython.map /mnt/c/jb/dev/esp/s3/mp_s3_oct_jbg.map
#
