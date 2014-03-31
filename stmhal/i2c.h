extern I2C_HandleTypeDef I2cHandle_X;
extern I2C_HandleTypeDef I2cHandle_Y;
extern const mp_obj_type_t pyb_i2c_type;

void i2c_init(void);
void i2c_start(I2C_HandleTypeDef *i2c_handle);
