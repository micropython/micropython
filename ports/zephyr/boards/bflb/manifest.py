freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

require("bundle-networking")

require("upysh")
require("aioble")
require("aiorepl")
require("utop")
