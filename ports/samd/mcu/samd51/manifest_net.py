include("$(PORT_DIR)/boards/manifest.py")
include("$(MPY_DIR)/extmod/asyncio")
# Drivers
require("onewire")
require("ds18x20")
require("dht")
# Networking
require("bundle-networking")
