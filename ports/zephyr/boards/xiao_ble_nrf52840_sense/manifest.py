include("$(MPY_DIR)/extmod/asyncio")

freeze("$(PORT_DIR)/modules")

require("upysh")
require("aioble")
require("aiorepl")
