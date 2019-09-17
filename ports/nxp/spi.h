#ifndef _SPI_H_
#define _SPI_H_


#if defined(BOARD_SPI_CMSIS_HEADER)
#include BOARD_SPI_CMSIS_HEADER
struct _ARM_DRIVER_SPI *spi_find_index(mp_obj_t id, uint8_t *instance);

ARM_SPI_SignalEvent_t spi_find_callback(uint8_t instance);

void spi_init(void);
#endif /* BOARD_SPI_CMSIS_HEADER */

#endif /* _SPI_H_ */