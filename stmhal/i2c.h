// use this for OwnAddress1 to configure I2C in master mode
#define PYB_I2C_MASTER_ADDRESS (0xfe)

extern I2C_HandleTypeDef I2CHandle1;
extern I2C_HandleTypeDef I2CHandle2;
extern const mp_obj_type_t pyb_i2c_type;

void i2c_init0(void);
void i2c_init(I2C_HandleTypeDef *i2c);
