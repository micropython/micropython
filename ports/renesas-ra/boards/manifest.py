include("$(MPY_DIR)/extmod/uasyncio/manifest.py")
freeze("$(MPY_DIR)/drivers/dht", "dht.py")
freeze("$(MPY_DIR)/drivers/onewire", "onewire.py")
freeze("$(MPY_DIR)/drivers/sdcard", "sdcard.py")
