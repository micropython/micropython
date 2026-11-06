# D2WD variant manifest: the 2MiB build has no room for the full frozen set
# alongside BLE, so the packages dropped relative to boards/manifest.py
# can be installed from micropython-lib via mip when needed.
freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

# Require some micropython-lib modules.
require("neopixel")
require("mip")
require("ntptime")
require("ssl")
require("requests")
