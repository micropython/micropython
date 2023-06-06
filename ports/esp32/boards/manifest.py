freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/uasyncio")

# Useful networking-related packages.
require("bundle-networking")

# Require some micropython-lib modules.
# require("aioespnow")
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")
require("umqtt.robust")
require("umqtt.simple")
require("upysh")
