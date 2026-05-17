/* generated configuration header file - do not edit */
#ifndef BSP_MCU_FAMILY_CFG_H_
#define BSP_MCU_FAMILY_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_mcu_device_pn_cfg.h"
#include "bsp_mcu_device_cfg.h"
#include "../../../ra/fsp/src/bsp/mcu/ra4m1/bsp_mcu_info.h"
#include "bsp_clock_cfg.h"
#define BSP_MCU_GROUP_RA4M1 (1)
#define BSP_LOCO_HZ                 (32768)
#define BSP_MOCO_HZ                 (8000000)
#define BSP_SUB_CLOCK_HZ            (32768)
#if   BSP_CFG_HOCO_FREQUENCY == 0
#define BSP_HOCO_HZ             (24000000)
#elif BSP_CFG_HOCO_FREQUENCY == 2
#define BSP_HOCO_HZ             (32000000)
#elif BSP_CFG_HOCO_FREQUENCY == 4
#define BSP_HOCO_HZ             (48000000)
#elif BSP_CFG_HOCO_FREQUENCY == 5
#define BSP_HOCO_HZ             (64000000)
#else
#error "Invalid HOCO frequency chosen (BSP_CFG_HOCO_FREQUENCY) in bsp_clock_cfg.h"
#endif
#define BSP_CORTEX_VECTOR_TABLE_ENTRIES    (16U)
#define BSP_VECTOR_TABLE_MAX_ENTRIES       (48U)

#define OFS_SEQ1 0xA001A001 | (1 << 1) | (3 << 2)
#define OFS_SEQ2 (15 << 4) | (3 << 8) | (3 << 10)
#define OFS_SEQ3 (1 << 12) | (1 << 14) | (1 << 17)
#define OFS_SEQ4 (3 << 18) | (15 << 20) | (3 << 24) | (3 << 26)
#define OFS_SEQ5 (1 << 28) | (1 << 30)
#define BSP_CFG_ROM_REG_OFS0 (OFS_SEQ1 | OFS_SEQ2 | OFS_SEQ3 | OFS_SEQ4 | OFS_SEQ5)
#define BSP_CFG_ROM_REG_OFS1 (0xFFFFFEC3 | (1 << 2) | (3 << 3) | (0 << 8))
#define BSP_CFG_USE_LOW_VOLTAGE_MODE ((0))
#define BSP_CFG_ROM_REG_MPU_PC0_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_PC0_START (0x00FFFFFC)
#define BSP_CFG_ROM_REG_MPU_PC0_END (0x00FFFFFF)
#define BSP_CFG_ROM_REG_MPU_PC1_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_PC1_START (0x00FFFFFC)
#define BSP_CFG_ROM_REG_MPU_PC1_END (0x00FFFFFF)
#define BSP_CFG_ROM_REG_MPU_REGION0_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_REGION0_START (0x00FFFFFC)
#define BSP_CFG_ROM_REG_MPU_REGION0_END (0x00FFFFFF)
#define BSP_CFG_ROM_REG_MPU_REGION1_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_REGION1_START (0x200FFFFC)
#define BSP_CFG_ROM_REG_MPU_REGION1_END (0x200FFFFF)
#define BSP_CFG_ROM_REG_MPU_REGION2_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_REGION2_START (0x407FFFFC)
#define BSP_CFG_ROM_REG_MPU_REGION2_END (0x407FFFFF)
#define BSP_CFG_ROM_REG_MPU_REGION3_ENABLE (1)
#define BSP_CFG_ROM_REG_MPU_REGION3_START (0x400DFFFC)
#define BSP_CFG_ROM_REG_MPU_REGION3_END (0x400DFFFF)
#ifndef BSP_CLOCK_CFG_MAIN_OSC_WAIT
#define BSP_CLOCK_CFG_MAIN_OSC_WAIT (9)
#endif
/* Used to create IELS values for the interrupt initialization table g_interrupt_event_link_select. */
#define BSP_PRV_IELS_ENUM(vector)    (ELC_##vector)

/*
 ID Code
 Note: To permanently lock and disable the debug interface define the BSP_ID_CODE_PERMANENTLY_LOCKED in the compiler settings.
 WARNING: This will disable debug access to the part and cannot be reversed by a debug probe.
 */
#if defined(BSP_ID_CODE_PERMANENTLY_LOCKED)
#define BSP_CFG_ID_CODE_LONG_1 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_2 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_3 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_4 (0x00000000)
#else
/* ID CODE: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF */
#define BSP_CFG_ID_CODE_LONG_1 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_2 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_3 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_4 (0xffFFFFFF)
#endif

#ifdef __cplusplus
}
#endif
#endif /* BSP_MCU_FAMILY_CFG_H_ */
