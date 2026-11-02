// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/async_spi/SPI.h"
#include "supervisor/shared/awaitable.h"

//| """SPI transfers for asyncio
//|
//| `async_spi.SPI` is an SPI bus whose transfers are awaited. While a transfer runs, other
//| asyncio tasks run too.
//|
//| .. code-block:: python
//|
//|     import asyncio
//|     import board
//|     import async_spi
//|
//|     async def main():
//|         spi = async_spi.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
//|         data = bytearray(512)
//|         async with spi.lock:
//|             spi.configure(baudrate=8_000_000)
//|             await spi.readinto(data)
//|
//|     asyncio.run(main())
//| """
//|
//| class Lock:
//|     """A bus lock for asyncio code, used as ``async with bus.lock:``. Tasks waiting for it
//|     get it in the order they asked; a task cancelled while waiting leaves the line."""
//|
//|     async def __aenter__(self) -> Lock:
//|         """Waits for the lock and takes it."""
//|         ...
//|
//|     def __aexit__(self) -> None:
//|         """Hands the lock to the next task waiting, or releases it."""
//|         ...
//|

static const mp_rom_map_elem_t async_spi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_async_spi) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&async_spi_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_Lock), MP_ROM_PTR(&circuitpy_async_lock_type) },
};

static MP_DEFINE_CONST_DICT(async_spi_module_globals, async_spi_module_globals_table);

const mp_obj_module_t async_spi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&async_spi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_async_spi, async_spi_module);
