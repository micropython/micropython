# base modules
include("$(PORT_DIR)/boards/manifest.py")

# uasyncio
include("$(MPY_DIR)/extmod/uasyncio")

# drivers
include("$(MPY_DIR)/drivers/display", ssd1306=True)

# micropython-lib: file utilities
require("upysh")

# micropython-lib: requests
require("urequests")
require("urllib.urequest")

# micropython-lib: umqtt
require("umqtt.simple")
require("umqtt.robust")
