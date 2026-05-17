#ifndef R_SPI_CFG_H_
#define R_SPI_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define SPI_DMA_SUPPORT_ENABLE (1)
#define SPI_TRANSMIT_FROM_RXI_ISR (0)

/* DEPRECATED: Use SPI_DMA_SUPPORT_ENABLE instead. */
#define SPI_DTC_SUPPORT_ENABLE (SPI_DMA_SUPPORT_ENABLE)

#ifdef __cplusplus
}
#endif
#endif /* R_SPI_CFG_H_ */
