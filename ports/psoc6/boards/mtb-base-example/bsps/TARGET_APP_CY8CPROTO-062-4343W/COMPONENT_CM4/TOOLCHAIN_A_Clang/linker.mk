################################################################################
# \file cy8c6xxa_cm4_dual.mk
# \version 2.95.1
#
# \brief
# Specifies the starting address and the size of the segments in the output
# file.
#
# \note The section definitions in this file are generic and handle all common
# use cases.
#
################################################################################
# \copyright
# Copyright 2018-2021 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

### CM0P ###
export HEAP_SIZE_CM0P     := 0x400
export VECT_BASE_CM0P     := 0x10000000
export RAM_VECT_BASE_CM0P := 0x08000000
export VECT_SIZE_CM0P     := 0x00000080
export TEXT_BASE_CM0P     := 0x10000080
export TEXT_SIZE_CM0P     := 0x00002000
export RAM_BASE_CM0P      := 0x08000080
export RAM_SIZE_CM0P      := 0x00002000
export CYMETA_BASE_CM0P   := 0x90500000
export STACK_SIZE_CM0P    := 0x2000

STACK_ADDRESS_TOP_CM0P    := $(shell printf "0x%x" $$(($(RAM_VECT_BASE_CM0P) + $(RAM_SIZE_CM0P))))
STACK_ADDRESS_BOTTOM_CM0P := $(shell printf "0x%x" $$(($(STACK_ADDRESS_TOP_CM0P) - $(STACK_SIZE_CM0P))))
TOOLCHAIN_VECT_BASE_CM0   := $(VECT_BASE_CM0P)

SECTIONS_CM0P := \
    -segaddr __VECT $(VECT_BASE_CM0P) \
    -segaddr __TEXT $(TEXT_BASE_CM0P) \
    -segaddr __DATA $(RAM_BASE_CM0P) \
    -segaddr __RAMVECTORS $(RAM_VECT_BASE_CM0P) \
    -segaddr __CYMETA $(CYMETA_BASE_CM0P) \
    -segaddr __STACK $(STACK_ADDRESS_TOP_CM0P)

### CM4 ###
export HEAP_SIZE_CM4     := 0x400
export VECT_BASE_CM4     := 0x10002000
export RAM_VECT_BASE_CM4 := 0x08002000
export VECT_SIZE_CM4     := 0x000002E0
export TEXT_BASE_CM4     := 0x100022E0
export TEXT_SIZE_CM4     := 0x001FE000
export RAM_BASE_CM4      := 0x080022E0
export RAM_SIZE_CM4      := 0x000FD800
export CYMETA_BASE_CM4   := 0x90500000
export STACK_SIZE_CM4    := 0x2000
STACK_ADDRESS_TOP_CM4    := $(shell printf "0x%x" $$(($(RAM_VECT_BASE_CM4) + $(RAM_SIZE_CM4))))
STACK_ADDRESS_BOTTOM_CM4 := $(shell printf "0x%x" $$(($(STACK_ADDRESS_TOP_CM4) - $(STACK_SIZE_CM4))))
TOOLCHAIN_VECT_BASE_CM4  := $(VECT_BASE_CM4)

SECTIONS_CM4 := \
    -segaddr __CY_M0P_IMAGE $(VECT_BASE_CM0P) \
    -segaddr __VECT $(VECT_BASE_CM4) \
    -segaddr __TEXT $(TEXT_BASE_CM4) \
    -segaddr __DATA $(RAM_BASE_CM4) \
    -segaddr __RAMVECTORS $(RAM_VECT_BASE_CM4) \
    -segaddr __CYMETA $(CYMETA_BASE_CM4) \
    -segaddr __STACK $(STACK_ADDRESS_TOP_CM4)

# Pass section addresses to the linker
ifeq ($(MTB_RECIPE__CORE),CM4)
ACLANG_MEM_LDFLAGS += \
    -segalign 4 \
    $(SECTIONS_CM4)
else ifeq ($(MTB_RECIPE__CORE),CM0P)
ACLANG_MEM_LDFLAGS += \
    -segalign 4 \
    $(SECTIONS_CM0P)
endif

# EOF
