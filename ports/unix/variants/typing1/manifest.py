include("$(PORT_DIR)/variants/manifest.py")

include("$(MPY_DIR)/extmod/asyncio")


# Freeze typing modules from local micropython-stubs repo
require("__future__", opt_level=3)
module("typing.py", base_path="/home/jos/micropython-stubs/mip", opt=3)
