#ifndef R_SDHI_CFG_H_
#define R_SDHI_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SDHI_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define SDMMC_CFG_UNALIGNED_ACCESS_ENABLE (1)
#ifndef SDHI_CFG_SD_SUPPORT_ENABLE
#define SDHI_CFG_SD_SUPPORT_ENABLE ((1))
#endif
#ifndef SDHI_CFG_EMMC_SUPPORT_ENABLE
#define SDHI_CFG_EMMC_SUPPORT_ENABLE ((0))
#endif

#ifdef __cplusplus
}
#endif
#endif /* R_SDHI_CFG_H_ */
