include("$(PORT_DIR)/boards/manifest.py")
require("webrepl")
require("aioble", client=True, central=True, l2cap=True, security=True)
