# This file is only used if cyw43 is enabled
include("$(PORT_DIR)/boards/manifest.py")

require("bundle-networking")

# Bluetooth
require("aioble")
