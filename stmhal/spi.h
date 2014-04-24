extern SPI_HandleTypeDef SPIHandle1;
extern SPI_HandleTypeDef SPIHandle2;
extern SPI_HandleTypeDef SPIHandle3;
extern const mp_obj_type_t pyb_spi_type;

void spi_init0(void);
void spi_init(SPI_HandleTypeDef *spi);
