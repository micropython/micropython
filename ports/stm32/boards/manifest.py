freeze(".", "_boot.py", opt=3)
include("$(MPY_DIR)/extmod/asyncio")

require("dht")
require("onewire")
