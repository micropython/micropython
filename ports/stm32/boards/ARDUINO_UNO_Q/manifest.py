include("$(PORT_DIR)/boards/manifest.py")

# Driver for the on-board LED matrix (pairs with the _ledmatrix C module).
module("ledmatrix.py", opt=3)
