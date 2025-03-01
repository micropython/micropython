#include "hal_data.h"
lpm_instance_ctrl_t g_lpm0_ctrl;

const lpm_cfg_t g_lpm0_cfg =
{ .low_power_mode = LPM_MODE_SLEEP, .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_RTCALM
      | (lpm_standby_wake_source_t)0,
  #if BSP_FEATURE_LPM_HAS_SNOOZE
  .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
  .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
  .snooze_end_sources = (lpm_snooze_end_t)0,
  .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
  #endif
  #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
  .output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN,
  #endif
  #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
  .io_port_state = LPM_IO_PORT_NO_CHANGE,
  .power_supply_state = LPM_POWER_SUPPLY_DEEPCUT0,
  .deep_standby_cancel_source = (lpm_deep_standby_cancel_source_t)0,
  .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
  #endif
  #if BSP_FEATURE_LPM_HAS_PDRAMSCR
  .ram_retention_cfg.ram_retention = (uint8_t)(0),
  .ram_retention_cfg.tcm_retention = false,
  #endif
  #if BSP_FEATURE_LPM_HAS_DPSBYCR_SRKEEP
  .ram_retention_cfg.standby_ram_retention = false,
  #endif
  #if BSP_FEATURE_LPM_HAS_LDO_CONTROL
  .ldo_standby_cfg.pll1_ldo = false,
  .ldo_standby_cfg.pll2_ldo = false,
  .ldo_standby_cfg.hoco_ldo = false,
  #endif
  .p_extend = NULL, };

const lpm_instance_t g_lpm0 =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm0_ctrl, .p_cfg = &g_lpm0_cfg };

flash_hp_instance_ctrl_t g_flash0_ctrl;
const flash_cfg_t g_flash0_cfg =
{ .data_flash_bgo = false, .p_callback = NULL, .p_context = NULL,
  #if defined(VECTOR_NUMBER_FCU_FRDYI)
  .irq = VECTOR_NUMBER_FCU_FRDYI,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_FCU_FIFERR)
  .err_irq = VECTOR_NUMBER_FCU_FIFERR,
  #else
  .err_irq = FSP_INVALID_VECTOR,
  #endif
  .err_ipl = (BSP_IRQ_DISABLED),
  .ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const flash_instance_t g_flash0 =
{ .p_ctrl = &g_flash0_ctrl, .p_cfg = &g_flash0_cfg, .p_api = &g_flash_on_flash_hp };
