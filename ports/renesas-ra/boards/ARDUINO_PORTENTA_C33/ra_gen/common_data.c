/* generated common source file - do not edit */
#include "common_data.h"
icu_instance_ctrl_t g_external_irq1_ctrl;
const external_irq_cfg_t g_external_irq1_cfg =
{ .channel = 2,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = NULL,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ2)
  .irq = VECTOR_NUMBER_ICU_IRQ2,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq1 =
{ .p_ctrl = &g_external_irq1_ctrl, .p_cfg = &g_external_irq1_cfg, .p_api = &g_external_irq_on_icu };
sce_instance_ctrl_t sce_ctrl;
const sce_cfg_t sce_cfg =
{ .lifecycle = SCE_SSD };
#if SCE_USER_SHA_384_ENABLED
uint32_t SCE_USER_SHA_384_FUNCTION(uint8_t *message, uint8_t *digest, uint32_t message_length);
#endif
icu_instance_ctrl_t g_external_irq0_ctrl;
const external_irq_cfg_t g_external_irq0_cfg =
{ .channel = 0,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = NULL,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ0)
  .irq = VECTOR_NUMBER_ICU_IRQ0,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq0 =
{ .p_ctrl = &g_external_irq0_ctrl, .p_cfg = &g_external_irq0_cfg, .p_api = &g_external_irq_on_icu };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_ctrl = &g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
void g_common_init(void) {
}
