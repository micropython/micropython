
// std includes


// mpy includes


// MTB includes
#include "cyhal.h"


// port-specific includes
#include "psoc6_i2c.h"


cy_rslt_t i2c_init(machine_i2c_obj_t *machine_i2c_obj) {
    // free first in case object has been initialized before
    cyhal_i2c_free(&machine_i2c_obj->i2c_obj);

    // Define the I2C master configuration structure
    cyhal_i2c_cfg_t i2c_master_config =
    {
        CYHAL_I2C_MODE_MASTER,
        0, // address is not used for master mode
        machine_i2c_obj->freq
    };

    // Initialize I2C master, set the SDA and SCL pins and assign a new clock
    cy_rslt_t result = cyhal_i2c_init(&machine_i2c_obj->i2c_obj, machine_i2c_obj->sda, machine_i2c_obj->scl, NULL);

    return result == CY_RSLT_SUCCESS ? cyhal_i2c_configure(&machine_i2c_obj->i2c_obj, &i2c_master_config)
                                     : result;
}


cy_rslt_t i2c_read(cyhal_i2c_t *i2c_obj, uint16_t addr, uint8_t *buf, size_t len, uint32_t timeout, bool send_stop) {
    return cyhal_i2c_master_read(i2c_obj, addr, buf, len, timeout, send_stop);
}


cy_rslt_t i2c_write(cyhal_i2c_t *i2c_obj, uint16_t addr, uint8_t *buf, size_t len, uint32_t timeout, bool send_stop) {
    return cyhal_i2c_master_write(i2c_obj, addr, buf, len, timeout, send_stop);
}
