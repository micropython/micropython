TYPE_CHECKING = False
if TYPE_CHECKING:
    from manifestfile import *

include("$(PORT_DIR)/variants/manifest.py")

include("$(MPY_DIR)/extmod/asyncio")

# to reduce code size:
#  - default optimization level is 3
#  - extensions are disabled by default
require("bundle-typing", extensions=True)
