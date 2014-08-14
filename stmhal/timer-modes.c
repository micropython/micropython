// mode = IC        HAL_TIM_IC_ConfigChannel - triggers irq
// 
// channel          1..4
// ic_polarity      rising, falling, both
// ic_selection     direct, indirect, trc
// ic_prescaler     div 1, 2, 4, 8
// ic_filter        0..15
//
// mode = OC        HAL_TIM_OC_ConfigChannel
// 
// channel          1..4
// oc_mode          Timing, Active, Inactive, Toggle, ForcedActie, ForcedInactive
// pulse            0x0000 .. 0xffff
// oc_polarity      high, low
// oc_npolarity     high, low
// oc_fastmode      disable, enable
// oc_idlestate     set, reset
// oc_nidlestate    set, reset
// 
// mode = PWM       HAL_TIM_PWM_ConfigChannel
// 
// channel          1..4
// pwm_mode         PWM1, PWM2
// pulse            0x0000 .. 0xffff
// oc_polarity      high, low
// oc_npolarity     high, low
// oc_fastmode      disable, enable
// oc_idlestate     set, reset
// oc_nidlestate    set, reset
// 
// mode = OP        HAL_TIM_OnePulse_ConfigChannel
// 
// input channel    1..4
// ouput channel    1..4
// opm_mode         single, repetitive
// pulse            0x0000 .. 0xfffff
// oc_polarity      high, low
// oc_npolarity     high, low
// oc_idlestate     set, reset
// oc_nidlestate    set, reset
// ic_polarity      rising, falling, both
// ic_selection     div 1, 2, 4, 8
// ic_filter        0..15
//
// mode = ENC
// 
// enc_mode         TI1, TI2, TI12
// ic1_polarity     rising, falling, both 
// ic1_selection    direct, indirect, trc 
// ic1_prescaler    div 1, 2, 4, 8        
// ic1_filter       0..15                 
// ic2_polarity     rising, falling, both 
// ic2_selection    direct, indirect, trc 
// ic2_prescaler    div 1, 2, 4, 8        
// ic2_filter       0..15                 

HAL_TIM_Base_Init
HAL_TIM_Base_Start

HAL_TIM_PWM_Init
HAL_TIM_PWM_ConfigChannel
HAL_TIM_PWM_Start

HAL_TIM_OC_Init
HAL_TIM_OC_ConfigChannel
HAL_TIM_OC_Start

HAL_TIM_IC_Init
HAL_TIM_IC_ConfigChannel
HAL_TIM_IC_Start

HAL_TIM_OnePulse_Init
HAL_TIM_OnePulse_ConfigChannel
HAL_TIM_OnePulse_Start

HAL_TIM_Encoder_Init
HAL_TIM_Encoder_Start
