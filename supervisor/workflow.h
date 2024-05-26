// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

void supervisor_workflow_reset(void);

// True when the user could be actively iterating on their code.
bool supervisor_workflow_active(void);

void supervisor_workflow_request_background(void);

void supervisor_workflow_start(void);
