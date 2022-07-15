include("$(MPY_DIR)/extmod/uasyncio")

include("$(MPY_DIR)/drivers/dht")
include("$(MPY_DIR)/drivers/display", lcd160cr=True, test=True)
include("$(MPY_DIR)/drivers/onewire", ds18x20=False)
