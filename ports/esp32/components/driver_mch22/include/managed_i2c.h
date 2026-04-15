#ifndef __MANAGED_I2C_H__
#define __MANAGED_I2C_H__
#include "buses.h"

#define i2c_read_bytes  driver_i2c_read_bytes
#define i2c_read_reg    driver_i2c_read_reg
#define i2c_read_event  driver_i2c_read_event
#define i2c_write_byte  driver_i2c_write_byte
#define i2c_write_reg_n driver_i2c_write_reg_n

#endif