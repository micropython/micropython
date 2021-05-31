SDKCONFIG += boards/sdkconfig.base
SDKCONFIG += boards/sdkconfig.spiram
SDKCONFIG += boards/sdkconfig.240mhz
SDKCONFIG += boards/M5CORE2/sdkconfig.board

FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py

PART_SRC = $(BOARD_DIR)/partitions.csv
LV_CFLAGS = -DLV_COLOR_DEPTH=16 -DLV_COLOR_16_SWAP=1
