// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "lib/oofatfs/ff.h"

extern bool supervisor_workflow_connecting(void);

// File system helpers for workflow code.
FRESULT supervisor_workflow_move(const char *old_path, const char *new_path);
FRESULT supervisor_workflow_mkdir(DWORD fattime, const char *full_path);
FRESULT supervisor_workflow_mkdir_parents(DWORD fattime, char *path);
FRESULT supervisor_workflow_delete_recursive(const char *full_path);
