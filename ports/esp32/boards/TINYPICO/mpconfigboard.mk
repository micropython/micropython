FLASH_FREQ = 80m

SDKCONFIG += boards/sdkconfig.base
SDKCONFIG += boards/sdkconfig.240mhz
SDKCONFIG += boards/sdkconfig.spiram
SDKCONFIG += boards/TINYPICO/sdkconfig.board

FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
