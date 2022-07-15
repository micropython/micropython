include("$(PORT_DIR)/boards/manifest.py")

# Networking
include("$(MPY_DIR)/extmod/webrepl")
require("urequests")

# Drivers
include("$(MPY_DIR)/drivers/lsm6dsox")

# Bluetooth
require("aioble", client=True, central=True, l2cap=True, security=True)
