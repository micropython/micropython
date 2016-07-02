#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# OSX_SDK	The location of the SDK eg. OSX_SDK  = /Developer/SDKs/MacOSX10.7.sdk
# OSX_ARCH	The architecture flags eg.  OSX_ARCH = -mmacosx-version-min=10.3
#

SRCFLAGS	+= -isysroot $(OSX_SDK) $(OSX_ARCH)
LDFLAGS		+= -pthread -Wl,-syslibroot,$(OSX_SDK) $(OSX_ARCH)
OPT_CPU = x86
