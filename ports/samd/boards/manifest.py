freeze("$(PORT_DIR)/modules")
try:
    include("$(BOARD_DIR)/manifest.py")
except FileNotFoundError:
    pass
