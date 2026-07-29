TYPE_CHECKING = False
if TYPE_CHECKING:
    from manifestfile import *

include("$(PORT_DIR)/variants/manifest.py")

include("$(MPY_DIR)/extmod/asyncio")

require("typing", opt_level=3)
# require("typing_extensions", opt_level=3)
