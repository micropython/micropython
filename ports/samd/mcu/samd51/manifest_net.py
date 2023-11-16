include("$(PORT_DIR)/boards/manifest.py")
include("$(MPY_DIR)/extmod/asyncio")
# Drivers
require("onewire")
require("ds18x20")
require("dht")
# Networking
require("bundle-networking")
# BLE
include(
    "$(MPY_LIB_DIR)/micropython/bluetooth/aioble/manifest.py",
    client=True,
    central=True,
    l2cap=True,
    security=True,
)
