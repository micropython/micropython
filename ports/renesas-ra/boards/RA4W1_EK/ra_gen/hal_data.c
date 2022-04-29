/* generated HAL source file - do not edit */
#include "hal_data.h"
/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
adc_instance_ctrl_t g_adc0_ctrl;
const adc_extended_cfg_t g_adc0_cfg_extend =
{ .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
  .adc_vref_control = ADC_VREF_CONTROL_VREFH, };
const adc_cfg_t g_adc0_cfg =
{ .unit = 0, .mode = ADC_MODE_SINGLE_SCAN, .resolution = ADC_RESOLUTION_14_BIT, .alignment =
      (adc_alignment_t)ADC_ALIGNMENT_RIGHT,
  .trigger = ADC_TRIGGER_SOFTWARE, .p_callback = NULL, .p_context = NULL, .p_extend = &g_adc0_cfg_extend,
  #if defined(VECTOR_NUMBER_ADC0_SCAN_END)
  .scan_end_irq = VECTOR_NUMBER_ADC0_SCAN_END,
  #else
  .scan_end_irq = FSP_INVALID_VECTOR,
  #endif
  .scan_end_ipl = (BSP_IRQ_DISABLED),
  #if defined(VECTOR_NUMBER_ADC0_SCAN_END_B)
  .scan_end_b_irq = VECTOR_NUMBER_ADC0_SCAN_END_B,
  #else
  .scan_end_b_irq = FSP_INVALID_VECTOR,
  #endif
  .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
const adc_channel_cfg_t g_adc0_channel_cfg =
{ .scan_mask = 0,
  .scan_mask_group_b = 0,
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
  .add_mask = 0,
  .sample_hold_mask = 0,
  .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0 =
{ .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg, .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
iic_master_instance_ctrl_t g_i2c_master0_ctrl;
const iic_master_extended_cfg_t g_i2c_master0_extend =
{ .timeout_mode = IIC_MASTER_TIMEOUT_MODE_SHORT,
/* Actual calculated bitrate: 99272. Actual calculated duty cycle: 49%. */ .clock_settings.brl_value = 27,
  .clock_settings.brh_value = 26, .clock_settings.cks_value = 2, };
const i2c_master_cfg_t g_i2c_master0_cfg =
{ .channel = 0, .rate = I2C_MASTER_RATE_STANDARD, .slave = 0x00, .addr_mode = I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
  #else
  .p_transfer_tx = &RA_NOT_DEFINED,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
  #else
  .p_transfer_rx = &RA_NOT_DEFINED,
  #endif
#undef RA_NOT_DEFINED
  .p_callback = callback_iic,
  .p_context = NULL,
  #if defined(VECTOR_NUMBER_IIC0_RXI)
  .rxi_irq = VECTOR_NUMBER_IIC0_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_IIC0_TXI)
  .txi_irq = VECTOR_NUMBER_IIC0_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_IIC0_TEI)
  .tei_irq = VECTOR_NUMBER_IIC0_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_IIC0_ERI)
  .eri_irq = VECTOR_NUMBER_IIC0_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif
  .ipl = (12),
  .p_extend = &g_i2c_master0_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_master0 =
{ .p_ctrl = &g_i2c_master0_ctrl, .p_cfg = &g_i2c_master0_cfg, .p_api = &g_i2c_master_on_iic };
lpm_instance_ctrl_t g_lpm0_ctrl;

const lpm_cfg_t g_lpm0_cfg =
{ .low_power_mode = LPM_MODE_SLEEP,
  .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
  .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_RTCALM | (lpm_standby_wake_source_t)0,
  .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
  .snooze_end_sources = (lpm_snooze_end_t)0,
  .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
  #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
  .output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN,
  #endif
  #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
  .io_port_state = 0,
  .power_supply_state = 0,
  .deep_standby_cancel_source = (lpm_deep_standby_cancel_source_t)0,
  .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
  #endif
  .p_extend = NULL, };

const lpm_instance_t g_lpm0 =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm0_ctrl, .p_cfg = &g_lpm0_cfg };
dtc_instance_ctrl_t g_transfer1_ctrl;

transfer_info_t g_transfer1_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void *)NULL,
  .p_src = (void const *)NULL,
  .num_blocks = 0,
  .length = 0, };
const dtc_extended_cfg_t g_transfer1_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SPI0_RXI, };
const transfer_cfg_t g_transfer1_cfg =
{ .p_info = &g_transfer1_info, .p_extend = &g_transfer1_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer0_ctrl;

transfer_info_t g_transfer0_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void *)NULL,
  .p_src = (void const *)NULL,
  .num_blocks = 0,
  .length = 0, };
const dtc_extended_cfg_t g_transfer0_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SPI0_TXI, };
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info, .p_extend = &g_transfer0_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
spi_instance_ctrl_t g_spi0_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t g_spi0_ext_cfg =
{ .spi_clksyn = SPI_SSL_MODE_CLK_SYN,
  .spi_comm = SPI_COMMUNICATION_FULL_DUPLEX,
  .ssl_polarity = SPI_SSLP_LOW,
  .ssl_select = SPI_SSL_SELECT_SSL0,
  .mosi_idle = SPI_MOSI_IDLE_VALUE_FIXING_DISABLE,
  .parity = SPI_PARITY_MODE_DISABLE,
  .byte_swap = SPI_BYTE_SWAP_DISABLE,
  .spck_div =
  {
      /* Actual calculated bitrate: 12000000. */ .spbr = 1,
      .brdv = 0
  },
  .spck_delay = SPI_DELAY_COUNT_1,
  .ssl_negation_delay = SPI_DELAY_COUNT_1,
  .next_access_delay = SPI_DELAY_COUNT_1 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t g_spi0_cfg =
{ .channel = 0,

  #if defined(VECTOR_NUMBER_SPI0_RXI)
  .rxi_irq = VECTOR_NUMBER_SPI0_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI0_TXI)
  .txi_irq = VECTOR_NUMBER_SPI0_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI0_TEI)
  .tei_irq = VECTOR_NUMBER_SPI0_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI0_ERI)
  .eri_irq = VECTOR_NUMBER_SPI0_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif

  .rxi_ipl = (12),
  .txi_ipl = (12),
  .tei_ipl = (12),
  .eri_ipl = (12),

  .operating_mode = SPI_MODE_MASTER,

  .clk_phase = SPI_CLK_PHASE_EDGE_ODD,
  .clk_polarity = SPI_CLK_POLARITY_LOW,

  .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE,
  .bit_order = SPI_BIT_ORDER_MSB_FIRST,
  .p_transfer_tx = g_spi0_P_TRANSFER_TX,
  .p_transfer_rx = g_spi0_P_TRANSFER_RX,
  .p_callback = spi_callback,

  .p_context = NULL,
  .p_extend = (void *)&g_spi0_ext_cfg, };

/* Instance structure to use this module. */
const spi_instance_t g_spi0 =
{ .p_ctrl = &g_spi0_ctrl, .p_cfg = &g_spi0_cfg, .p_api = &g_spi_on_spi };
icu_instance_ctrl_t g_external_irq4_ctrl;
const external_irq_cfg_t g_external_irq4_cfg =
{ .channel = 4,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = callback_icu,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ4)
  .irq = VECTOR_NUMBER_ICU_IRQ4,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq4 =
{ .p_ctrl = &g_external_irq4_ctrl, .p_cfg = &g_external_irq4_cfg, .p_api = &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq3_ctrl;
const external_irq_cfg_t g_external_irq3_cfg =
{ .channel = 3,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = callback_icu,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ3)
  .irq = VECTOR_NUMBER_ICU_IRQ3,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq3 =
{ .p_ctrl = &g_external_irq3_ctrl, .p_cfg = &g_external_irq3_cfg, .p_api = &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq2_ctrl;
const external_irq_cfg_t g_external_irq2_cfg =
{ .channel = 2,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = callback_icu,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ2)
  .irq = VECTOR_NUMBER_ICU_IRQ2,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq2 =
{ .p_ctrl = &g_external_irq2_ctrl, .p_cfg = &g_external_irq2_cfg, .p_api = &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq1_ctrl;
const external_irq_cfg_t g_external_irq1_cfg =
{ .channel = 1,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = callback_icu,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (12),
  #if defined(VECTOR_NUMBER_ICU_IRQ1)
  .irq = VECTOR_NUMBER_ICU_IRQ1,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq1 =
{ .p_ctrl = &g_external_irq1_ctrl, .p_cfg = &g_external_irq1_cfg, .p_api = &g_external_irq_on_icu };
icu_instance_ctrl_t g_external_irq0_ctrl;
const external_irq_cfg_t g_external_irq0_cfg =
{ .channel = 0,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = callback_icu,
  .p_context = NULL,
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
agt_instance_ctrl_t g_timer1_ctrl;
const agt_extended_cfg_t g_timer1_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoa = AGT_PIN_CFG_DISABLED,
  .agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.002730666666666667 seconds. Actual duty: 50%. */ .period_counts = 0x10000,
  .duty_cycle_counts = 0x8000, .source_div = (timer_source_div_t)0, .channel = 1, .p_callback = callback_agt,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = &g_timer1_extend,
  .cycle_end_ipl = (5),
  #if defined(VECTOR_NUMBER_AGT1_INT)
  .cycle_end_irq = VECTOR_NUMBER_AGT1_INT,
  #else
  .cycle_end_irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer1 =
{ .p_ctrl = &g_timer1_ctrl, .p_cfg = &g_timer1_cfg, .p_api = &g_timer_on_agt };
agt_instance_ctrl_t g_timer0_ctrl;
const agt_extended_cfg_t g_timer0_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoa = AGT_PIN_CFG_DISABLED,
  .agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.002730666666666667 seconds. Actual duty: 50%. */ .period_counts = 0x10000,
  .duty_cycle_counts = 0x8000, .source_div = (timer_source_div_t)0, .channel = 0, .p_callback = callback_agt,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = &g_timer0_extend,
  .cycle_end_ipl = (5),
  #if defined(VECTOR_NUMBER_AGT0_INT)
  .cycle_end_irq = VECTOR_NUMBER_AGT0_INT,
  #else
  .cycle_end_irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_agt };
flash_lp_instance_ctrl_t g_flash0_ctrl;
const flash_cfg_t g_flash0_cfg =
{ .data_flash_bgo = false, .p_callback = NULL, .p_context = NULL, .ipl = (BSP_IRQ_DISABLED),
  #if defined(VECTOR_NUMBER_FCU_FRDYI)
  .irq = VECTOR_NUMBER_FCU_FRDYI,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const flash_instance_t g_flash0 =
{ .p_ctrl = &g_flash0_ctrl, .p_cfg = &g_flash0_cfg, .p_api = &g_flash_on_flash_lp };
rtc_instance_ctrl_t g_rtc0_ctrl;
const rtc_error_adjustment_cfg_t g_rtc0_err_cfg =
{ .adjustment_mode = RTC_ERROR_ADJUSTMENT_MODE_AUTOMATIC,
  .adjustment_period = RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND,
  .adjustment_type = RTC_ERROR_ADJUSTMENT_NONE,
  .adjustment_value = 0, };
const rtc_cfg_t g_rtc0_cfg =
{ .clock_source = RTC_CLOCK_SOURCE_LOCO, .freq_compare_value_loco = 255, .p_err_cfg = &g_rtc0_err_cfg, .p_callback =
      NULL,
  .p_context = NULL, .alarm_ipl = (14), .periodic_ipl = (14), .carry_ipl = (14),
  #if defined(VECTOR_NUMBER_RTC_ALARM)
  .alarm_irq = VECTOR_NUMBER_RTC_ALARM,
  #else
  .alarm_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_RTC_PERIOD)
  .periodic_irq = VECTOR_NUMBER_RTC_PERIOD,
  #else
  .periodic_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_RTC_CARRY)
  .carry_irq = VECTOR_NUMBER_RTC_CARRY,
  #else
  .carry_irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const rtc_instance_t g_rtc0 =
{ .p_ctrl = &g_rtc0_ctrl, .p_cfg = &g_rtc0_cfg, .p_api = &g_rtc_on_rtc };
sci_uart_instance_ctrl_t g_uart9_ctrl;

baud_setting_t g_uart9_baud_setting =
{
/* Baud rate calculated with 0.160% error. */ .abcse = 0,
    .abcs = 0, .bgdm = 1, .cks = 0, .brr = 25, .mddr = (uint8_t)256, .brme = false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart9_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT,
  .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE,
  .noise_cancel = SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
  .p_baud_setting = &g_uart9_baud_setting,
  .uart_mode = UART_MODE_RS232,
  .ctsrts_en = SCI_UART_CTSRTS_RTS_OUTPUT,
  #if 0
  .flow_control_pin = BSP_IO_PORT_00_PIN_00,
  #else
  .flow_control_pin = (bsp_io_port_pin_t)(0xFFFFU),
  #endif
};

/** UART interface configuration */
const uart_cfg_t g_uart9_cfg =
{ .channel = 9, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
      user_uart_callback,
  .p_context = NULL, .p_extend = &g_uart9_cfg_extend,
#define RA_NOT_DEFINED (1)
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
  #else
  .p_transfer_tx = &RA_NOT_DEFINED,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
  #else
  .p_transfer_rx = &RA_NOT_DEFINED,
  #endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
  #if defined(VECTOR_NUMBER_SCI9_RXI)
  .rxi_irq = VECTOR_NUMBER_SCI9_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI9_TXI)
  .txi_irq = VECTOR_NUMBER_SCI9_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI9_TEI)
  .tei_irq = VECTOR_NUMBER_SCI9_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI9_ERI)
  .eri_irq = VECTOR_NUMBER_SCI9_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart9 =
{ .p_ctrl = &g_uart9_ctrl, .p_cfg = &g_uart9_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart4_ctrl;

baud_setting_t g_uart4_baud_setting =
{
/* Baud rate calculated with 0.160% error. */ .abcse = 0,
    .abcs = 0, .bgdm = 1, .cks = 0, .brr = 25, .mddr = (uint8_t)256, .brme = false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart4_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT,
  .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE,
  .noise_cancel = SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
  .p_baud_setting = &g_uart4_baud_setting,
  .uart_mode = UART_MODE_RS232,
  .ctsrts_en = SCI_UART_CTSRTS_RTS_OUTPUT,
  #if 0
  .flow_control_pin = BSP_IO_PORT_00_PIN_00,
  #else
  .flow_control_pin = (bsp_io_port_pin_t)(0xFFFFU),
  #endif
};

/** UART interface configuration */
const uart_cfg_t g_uart4_cfg =
{ .channel = 4, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
      user_uart_callback,
  .p_context = NULL, .p_extend = &g_uart4_cfg_extend,
#define RA_NOT_DEFINED (1)
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
  #else
  .p_transfer_tx = &RA_NOT_DEFINED,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
  #else
  .p_transfer_rx = &RA_NOT_DEFINED,
  #endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
  #if defined(VECTOR_NUMBER_SCI4_RXI)
  .rxi_irq = VECTOR_NUMBER_SCI4_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI4_TXI)
  .txi_irq = VECTOR_NUMBER_SCI4_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI4_TEI)
  .tei_irq = VECTOR_NUMBER_SCI4_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI4_ERI)
  .eri_irq = VECTOR_NUMBER_SCI4_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart4 =
{ .p_ctrl = &g_uart4_ctrl, .p_cfg = &g_uart4_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart1_ctrl;

baud_setting_t g_uart1_baud_setting =
{
/* Baud rate calculated with 0.160% error. */ .abcse = 0,
    .abcs = 0, .bgdm = 1, .cks = 0, .brr = 25, .mddr = (uint8_t)256, .brme = false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart1_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT,
  .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE,
  .noise_cancel = SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
  .p_baud_setting = &g_uart1_baud_setting,
  .uart_mode = UART_MODE_RS232,
  .ctsrts_en = SCI_UART_CTSRTS_RTS_OUTPUT,
  #if 0
  .flow_control_pin = BSP_IO_PORT_00_PIN_00,
  #else
  .flow_control_pin = (bsp_io_port_pin_t)(0xFFFFU),
  #endif
};

/** UART interface configuration */
const uart_cfg_t g_uart1_cfg =
{ .channel = 1, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
      user_uart_callback,
  .p_context = NULL, .p_extend = &g_uart1_cfg_extend,
#define RA_NOT_DEFINED (1)
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
  #else
  .p_transfer_tx = &RA_NOT_DEFINED,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
  #else
  .p_transfer_rx = &RA_NOT_DEFINED,
  #endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
  #if defined(VECTOR_NUMBER_SCI1_RXI)
  .rxi_irq = VECTOR_NUMBER_SCI1_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI1_TXI)
  .txi_irq = VECTOR_NUMBER_SCI1_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI1_TEI)
  .tei_irq = VECTOR_NUMBER_SCI1_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI1_ERI)
  .eri_irq = VECTOR_NUMBER_SCI1_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart1 =
{ .p_ctrl = &g_uart1_ctrl, .p_cfg = &g_uart1_cfg, .p_api = &g_uart_on_sci };
void g_hal_init(void) {
    g_common_init();
}
