// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/i2ctarget/I2CTarget.h"

#include "py/runtime.h"

//| """Two wire serial protocol target
//|
//| In many cases, i2c is used by a controller to retrieve (or send) to a peripheral (target). It is also possible
//| for a device to act as a target for another controller.  However, a device can only be a controller or a target on
//| an I2C bus (although many devices now support multiple I2C busses).
//|
//| .. note::
//|    `I2CTarget` takes a list of addresses, but not all devices support this feature
//|
//| Example of emulating a simple device that can only handle single writes and reads::
//|
//|    import board
//|    from i2ctarget import I2CTarget
//|
//|    import adafruit_logging as logging
//|
//|    logger = logging.getLogger('i2ctarget')
//|    logger.setLevel(logging.INFO)
//|    logger.addHandler(logging.StreamHandler())
//|
//|    logger.info("\\n\\ncode starting...")
//|
//|    # initialize an I2C target with a device address of 0x40
//|    with I2CTarget(board.SCL, board.SDA, (0x40,)) as device:
//|
//|        while True:
//|            # check if there's a pending device request
//|            i2c_target_request = device.request()
//|
//|            if not i2c_target_request:
//|                # no request is pending
//|                continue
//|
//|            # `with` invokes I2CTargetRequest's functions to handle the necessary opening and closing of a request
//|            with i2c_target_request:
//|
//|                # the address associated with the request
//|                address = i2c_target_request.address
//|
//|                if i2c_target_request.is_read:
//|                    logger.info(f"read request to address '0x{address:02x}'")
//|
//|                    # for our emulated device, return a fixed value for the request
//|                    buffer = bytes([0xaa])
//|                    i2c_target_request.write(buffer)
//|                else:
//|                    # transaction is a write request
//|                    data = i2c_target_request.read(1)
//|                    logger.info(f"write request to address 0x{address:02x}: {data}")
//|                    # for our emulated device, writes have no effect
//|
//| This example creates an I2C target device that can be accessed via another device as an I2C controller::
//|
//|        import busio
//|        import board
//|        i2c = busio.I2C(board.SCL, board.SDA)
//|
//|        # perform a single read
//|        while not i2c.try_lock():
//|            pass
//|        buffer = bytearray(1)
//|        i2c.readfrom_into(0x40, buffer)
//|        print(f"device responded with {buffer}")
//|        i2c.unlock()
//|
//|        # perform a single write
//|        while not i2c.try_lock():
//|            pass
//|        buffer = bytearray(1)
//|        buffer[0] = 0x12
//|        i2c.writeto(0x40, buffer)
//|        print(f"wrote {buffer} to device")
//|        i2c.unlock()
//|
//| Typically, i2c devices support writes and reads to/from multiple register indices as in this example    ::
//|
//|    import board
//|    from i2ctarget import I2CTarget
//|
//|    import adafruit_logging as logging
//|
//|    logger = logging.getLogger('i2ctarget')
//|    logger.setLevel(logging.INFO)
//|    logger.addHandler(logging.StreamHandler())
//|
//|    # emulate a target with 16 registers
//|    regs = [0] * 16
//|    register_index = None
//|
//|    logger.info("\\n\\ncode starting...")
//|
//|    # initialize an I2C target with a device address of 0x40
//|    with I2CTarget(board.SCL, board.SDA, (0x40,)) as device:
//|
//|        while True:
//|            # check if there's a pending device request
//|            i2c_target_request = device.request()
//|
//|            if not i2c_target_request:
//|                # no request is pending
//|                continue
//|
//|            # work with the i2c request
//|            with i2c_target_request:
//|
//|                if not i2c_target_request.is_read:
//|                    # a write request
//|
//|                    # bytearray contains the request's first byte, the register's index
//|                    index = i2c_target_request.read(1)[0]
//|
//|                    # bytearray containing the request's second byte, the data
//|                    data = i2c_target_request.read(1)
//|
//|                    # if the request doesn't have a second byte, this is read transaction
//|                    if not data:
//|
//|                        # since we're only emulating 16 registers, read from a larger address is an error
//|                        if index > 15:
//|                            logger.error(f"write portion of read transaction has invalid index {index}")
//|                            continue
//|
//|                        logger.info(f"write portion of read transaction, set index to {index}'")
//|                        register_index = index
//|                        continue
//|
//|                    # since we're only emulating 16 registers, writing to a larger address is an error
//|                    if index > 15:
//|                        logger.error(f"write request to incorrect index {index}")
//|                        continue
//|
//|                    logger.info(f"write request to index {index}: {data}")
//|                    regs[index] = data[0]
//|                else:
//|                    # our emulated device requires a read to be part of a full write-then-read transaction
//|                    if not i2c_target_request.is_restart:
//|                        logger.warning(f"read request without first writing is not supported")
//|                        # still need to respond, but result data is not defined
//|                        i2c_target_request.write(bytes([0xff]))
//|                        register_index = None
//|                        continue
//|
//|                    # the single read transaction case is covered above, so we should always have a valid index
//|                    assert(register_index is not None)
//|
//|                    # the write-then-read to an invalid address is covered above,
//|                    #   but if this is a restarted read, index might be out of bounds so need to check
//|                    if register_index > 16:
//|                        logger.error(f"restarted read yielded an unsupported index")
//|                        i2c_target_request.write(bytes([0xff]))
//|                        register_index = None
//|                        continue
//|
//|                    # retrieve the data from our register file and respond
//|                    data = regs[register_index]
//|                    logger.info(f"read request from index {register_index}: {data}")
//|                    i2c_target_request.write(bytes([data]))
//|
//|                    # in our emulated device, a single read transaction is covered above
//|                    #   so any subsequent restarted read gets the value at the next index
//|                    assert(i2c_target_request.is_restart is True)
//|                    register_index += 1
//|
//| This second example creates I2C target device that can be accessed via another device as an I2C controller::
//|
//|    import busio
//|    import board
//|    i2c = busio.I2C(board.SCL, board.SDA)
//|
//|    # perform a write transaction
//|    while not i2c.try_lock():
//|        pass
//|    buffer = bytearray(2)
//|    buffer[0] = 0x0b  # the register index
//|    buffer[1] = 0xa1  # the value
//|    i2c.writeto(0x40, buffer)
//|    print(f"wrote {buffer} to device")
//|    i2c.unlock()
//|
//|    # perform a full read transaction (write-then-read)
//|    while not i2c.try_lock():
//|        pass
//|    index_buffer = bytearray(1)
//|    index_buffer[0] = 0x0b
//|    read_buffer = bytearray(1)
//|    i2c.writeto_then_readfrom(0x40, index_buffer, read_buffer)
//|    print(f"read from device index {index_buffer}: {read_buffer}")
//|    i2c.unlock()
//|
//| Or accessed from Linux like this::
//|
//|    $ i2cget -y 1 0x40 0x0b
//|    0xff
//|    $ i2cset -y 1 0x40 0x0b 0xa1
//|    $ i2cget -y 1 0x40 0x01
//|    0xa1
//|
//| .. warning::
//|    I2CTarget makes use of clock stretching in order to slow down the host.
//|    Make sure the I2C host supports this.
//|
//|    Raspberry Pi 3 and below, in particular, do not support this with its I2C hw block.
//|    This can be worked around by using the ``i2c-gpio`` bit banging driver.
//|    Since the RPi firmware uses the hw i2c, it's not possible to emulate a HAT eeprom."""

static const mp_rom_map_elem_t i2ctarget_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2ctarget) },
    { MP_ROM_QSTR(MP_QSTR_I2CTarget), MP_ROM_PTR(&i2ctarget_i2c_target_type) },
};

static MP_DEFINE_CONST_DICT(i2ctarget_module_globals, i2ctarget_module_globals_table);

const mp_obj_module_t i2ctarget_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&i2ctarget_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_i2ctarget, i2ctarget_module);
