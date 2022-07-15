# base modules
include("$(PORT_DIR)/boards/manifest.py")

# uasyncio
include("$(MPY_DIR)/extmod/uasyncio")

# drivers
include("$(MPY_DIR)/drivers/display", ssd1306=True)

# file utilities
require("upysh")

# requests
require("urequests")
require("urllib.urequest")

# umqtt
require("umqtt.simple")
require("umqtt.robust")
