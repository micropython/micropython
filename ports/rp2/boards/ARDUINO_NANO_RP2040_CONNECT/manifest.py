include("$(PORT_DIR)/boards/manifest.py")

# Networking
require("bundle-networking")

# Drivers
require("lsm6dsox")
require("espflash")

# Utils
require("time")
require("senml")
require("logging")

# Bluetooth
require("aioble")
