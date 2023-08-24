ANALOGUE_READINGS_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(ANALOGUE_READINGS_MOD_DIR)/module_config.c
SRC_USERMOD_CXX += $(ANALOGUE_READINGS_MOD_DIR)/analogue_readings.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(ANALOGUE_READINGS_MOD_DIR)
CXXFLAGS_USERMOD += -I$(ANALOGUE_READINGS_MOD_DIR) -std=c++11

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++
