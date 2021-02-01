CPPEXAMPLE_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(CPPEXAMPLE_MOD_DIR)/examplemodule.c
SRC_USERMOD_CXX += $(CPPEXAMPLE_MOD_DIR)/example.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(CPPEXAMPLE_MOD_DIR)
CXXFLAGS_USERMOD += -I$(CPPEXAMPLE_MOD_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++
