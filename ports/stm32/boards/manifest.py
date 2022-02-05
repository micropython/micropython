options.defaults(extra_drivers=True)

include("$(MPY_DIR)/extmod/uasyncio/manifest.py")

if options.extra_drivers:
    freeze("$(MPY_DIR)/drivers/dht", "dht.py")
    freeze("$(MPY_DIR)/drivers/display", ("lcd160cr.py", "lcd160cr_test.py"))
    freeze("$(MPY_DIR)/drivers/onewire", "onewire.py")
