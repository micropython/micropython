# Manifest for testing the build.

# Include standard manifest.
include("manifest.py")

# Test freezing @micropython.native code.
freeze("$(MPY_DIR)/tests/micropython", "native_misc.py")

# Test freezing @micropython.viper code.
freeze("$(MPY_DIR)/tests/micropython", "viper_misc.py")
