include("$(PORT_DIR)/boards/manifest.py")
include("$(MPY_DIR)/extmod/webrepl")
require("aioble", client=True, central=True, l2cap=True, security=True)
