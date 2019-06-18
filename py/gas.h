//   Copyright (C) 2018 TASChain
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef GAS_H
#define GAS_H

#include "py/mpconfig.h"
#include "py/misc.h"


void setGas(int value);

int getGas();

bool FireGas_Mem(size_t len);

bool FireGas_DB(size_t len);

bool CheckGas(byte* op);

void Gas_Report();

#endif //GAS_H
