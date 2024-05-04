make \
    BOARD=ESP32_GENERIC_S3 \
    BOARD_VARIANT=SPIRAM \
    USER_C_MODULES=../../../../s3lcd/src/micropython.cmake \
    FROZEN_MANIFEST=../../../../s3lcd/manifest.py \
