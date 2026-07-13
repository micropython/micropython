# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# Frozen modules for the MicroPython UEFI port.
# asyncio is an in-tree, pure-Python package (extmod/asyncio); freezing it is the
# way to ship it without a filesystem. It falls back to a pure-Python TaskQueue
# when the optional _asyncio C accelerator is absent.
include("$(MPY_DIR)/extmod/asyncio")

# Port-local pure-Python modules (e.g. gzip, a wrapper over the C deflate module).
freeze("modules")
