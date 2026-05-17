/* generated common source file - do not edit */
#include "common_data.h"
icu_instance_ctrl_t g_external_irq14_ctrl;
const external_irq_cfg_t g_external_irq14_cfg = { .channel = 14, .trigger =
                                                      EXTERNAL_IRQ_TRIG_RISING, .filter_enable = false, .pclk_div =
                                                      EXTERNAL_IRQ_PCLK_DIV_BY_64, .p_callback = callback_icu,
/** If NULL then do not add & */
                                                  #if defined(NULL)
                                                  .p_context = NULL,
                                                  #else
                                                  .p_context = &NULL,
                                                  #endif
                                                  .p_extend = NULL, .ipl = (12),
                                                  #if defined(VECTOR_NUMBER_ICU_IRQ14)
                                                  .irq = VECTOR_NUMBER_ICU_IRQ14,
                                                  #else
                                                  .irq = FSP_INVALID_VECTOR,
                                                  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq14 = { .p_ctrl =
                                                       &g_external_irq14_ctrl, .p_cfg = &g_external_irq14_cfg, .p_api =
                                                       &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq13_ctrl;
const external_irq_cfg_t g_external_irq13_cfg = { .channel = 13, .trigger =
                                                      EXTERNAL_IRQ_TRIG_RISING, .filter_enable = false, .pclk_div =
                                                      EXTERNAL_IRQ_PCLK_DIV_BY_64, .p_callback = callback_icu,
/** If NULL then do not add & */
                                                  #if defined(NULL)
                                                  .p_context = NULL,
                                                  #else
                                                  .p_context = &NULL,
                                                  #endif
                                                  .p_extend = NULL, .ipl = (12),
                                                  #if defined(VECTOR_NUMBER_ICU_IRQ13)
                                                  .irq = VECTOR_NUMBER_ICU_IRQ13,
                                                  #else
                                                  .irq = FSP_INVALID_VECTOR,
                                                  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq13 = { .p_ctrl =
                                                       &g_external_irq13_ctrl, .p_cfg = &g_external_irq13_cfg, .p_api =
                                                       &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq7_ctrl;
const external_irq_cfg_t g_external_irq7_cfg = { .channel = 7, .trigger =
                                                     EXTERNAL_IRQ_TRIG_RISING, .filter_enable = false, .pclk_div =
                                                     EXTERNAL_IRQ_PCLK_DIV_BY_64, .p_callback = callback_icu,
/** If NULL then do not add & */
                                                 #if defined(NULL)
                                                 .p_context = NULL,
                                                 #else
                                                 .p_context = &NULL,
                                                 #endif
                                                 .p_extend = NULL, .ipl = (12),
                                                 #if defined(VECTOR_NUMBER_ICU_IRQ7)
                                                 .irq = VECTOR_NUMBER_ICU_IRQ7,
                                                 #else
                                                 .irq = FSP_INVALID_VECTOR,
                                                 #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq7 = { .p_ctrl =
                                                      &g_external_irq7_ctrl, .p_cfg = &g_external_irq7_cfg, .p_api =
                                                      &g_external_irq_on_icu };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport = { .p_api = &g_ioport_on_ioport, .p_ctrl =
                                         &g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
void g_common_init(void) {
}
