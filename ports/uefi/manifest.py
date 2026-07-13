# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# Frozen modules for the MicroPython UEFI port.
# asyncio is an in-tree, pure-Python package (extmod/asyncio); freezing it is the
# way to ship it without a filesystem. It falls back to a pure-Python TaskQueue
# when the optional _asyncio C accelerator is absent.
include("$(MPY_DIR)/extmod/asyncio")

# CPython-compatible `ssl` facade over the built-in `tls` module: the real
# micropython-lib package (unmodified), not a local reimplementation — same
# path tests/run_uefi_tests.py already reaches into for `unittest`.
freeze("$(MPY_DIR)/lib/micropython-lib/python-stdlib/ssl", "ssl.py")

# Port-local pure-Python modules (the uefi.* package and friends).
freeze("modules")
