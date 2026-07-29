TYPE_CHECKING = False
if TYPE_CHECKING:
    from manifestfile import *


include("$(PORT_DIR)/boards/manifest.py")
include("$(PORT_DIR)/boards/manifest_pyboard.py")

# Typing
require("bundle-typing", extensions=True)
