include("$(PORT_DIR)/boards/manifest.py")

# Networking
require("bundle-networking")

# Drivers
require("lsm6dsox")
require("espflash")

# Utils
require("logging")

# Bluetooth
require("aioble", client=True, central=True, l2cap=True, security=True)
