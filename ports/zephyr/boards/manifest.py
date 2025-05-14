# This is an example frozen module manifest. Enable this by configuring
# the Zephyr project and enabling the frozen modules config feature.

freeze("$(PORT_DIR)/modules")

# Require a micropython-lib module.
require("upysh")
