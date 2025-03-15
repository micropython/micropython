/* generated HAL source file - do not edit */
#include "hal_data.h"
/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_tx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi1_tx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_tx_dmac_callback(&g_spi1_ctrl);
}
#endif

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_rx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi1_rx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_rx_dmac_callback(&g_spi1_ctrl);
}
#endif
#undef RA_NOT_DEFINED

spi_instance_ctrl_t g_spi1_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t g_spi1_ext_cfg =
{ .spi_clksyn = SPI_SSL_MODE_CLK_SYN,
  .spi_comm = SPI_COMMUNICATION_FULL_DUPLEX,
  .ssl_polarity = SPI_SSLP_HIGH,
  .ssl_select = SPI_SSL_SELECT_SSL1,
  .mosi_idle = SPI_MOSI_IDLE_VALUE_FIXING_DISABLE,
  .parity = SPI_PARITY_MODE_DISABLE,
  .byte_swap = SPI_BYTE_SWAP_DISABLE,
  .spck_div =
  {
      /* Actual calculated bitrate: 10000000. */ .spbr = 4,
      .brdv = 0
  },
  .spck_delay = SPI_DELAY_COUNT_1,
  .ssl_negation_delay = SPI_DELAY_COUNT_1,
  .next_access_delay = SPI_DELAY_COUNT_1 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t g_spi1_cfg =
{ .channel = 1,

  #if defined(VECTOR_NUMBER_SPI1_RXI)
  .rxi_irq = VECTOR_NUMBER_SPI1_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI1_TXI)
  .txi_irq = VECTOR_NUMBER_SPI1_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI1_TEI)
  .tei_irq = VECTOR_NUMBER_SPI1_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SPI1_ERI)
  .eri_irq = VECTOR_NUMBER_SPI1_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif

  .rxi_ipl = (12),
  .txi_ipl = (12),
  .tei_ipl = (12),
  .eri_ipl = (12),

  .operating_mode = SPI_MODE_MASTER,

  .clk_phase = SPI_CLK_PHASE_EDGE_ODD,
  .clk_polarity = SPI_CLK_POLARITY_HIGH,

  .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE,
  .bit_order = SPI_BIT_ORDER_MSB_FIRST,
  .p_transfer_tx = g_spi1_P_TRANSFER_TX,
  .p_transfer_rx = g_spi1_P_TRANSFER_RX,
  .p_callback = spi_callback,

  .p_context = NULL,
  .p_extend = (void *)&g_spi1_ext_cfg, };

/* Instance structure to use this module. */
const spi_instance_t g_spi1 =
{ .p_ctrl = &g_spi1_ctrl, .p_cfg = &g_spi1_cfg, .p_api = &g_spi_on_spi };
sci_uart_instance_ctrl_t g_uart8_ctrl;

baud_setting_t g_uart8_baud_setting =
{
    /* Baud rate calculated with 0.469% error. */ .semr_baudrate_bits_b.abcse = 1,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 0, .cks = 0, .brr = 17, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart8_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
      SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart8_baud_setting, .flow_control =
      SCI_UART_FLOW_CONTROL_HARDWARE_CTSRTS,
  #if 0xFF != 0xFF
  .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
  #else
  .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
  #endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
    #if 0xFF != 0xFF
    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
  }, };

/** UART interface configuration */
const uart_cfg_t g_uart8_cfg =
{ .channel = 8, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
      user_uart_callback,
  .p_context = NULL, .p_extend = &g_uart8_cfg_extend,
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
  #if defined(VECTOR_NUMBER_SCI8_RXI)
  .rxi_irq = VECTOR_NUMBER_SCI8_RXI,
  #else
  .rxi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI8_TXI)
  .txi_irq = VECTOR_NUMBER_SCI8_TXI,
  #else
  .txi_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI8_TEI)
  .tei_irq = VECTOR_NUMBER_SCI8_TEI,
  #else
  .tei_irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_SCI8_ERI)
  .eri_irq = VECTOR_NUMBER_SCI8_ERI,
  #else
  .eri_irq = FSP_INVALID_VECTOR,
  #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart8 =
{ .p_ctrl = &g_uart8_ctrl, .p_cfg = &g_uart8_cfg, .p_api = &g_uart_on_sci };
agt_instance_ctrl_t g_timer1_ctrl;
const agt_extended_cfg_t g_timer1_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.00131072 seconds. Actual duty: 50%. */ .period_counts = (uint32_t)0x10000,
  .duty_cycle_counts = 0x8000, .source_div = (timer_source_div_t)0, .channel = 1, .p_callback = NULL,
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
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.00131072 seconds. Actual duty: 50%. */ .period_counts = (uint32_t)0x10000,
  .duty_cycle_counts = 0x8000, .source_div = (timer_source_div_t)0, .channel = 0, .p_callback = NULL,
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
dac_instance_ctrl_t g_dac1_ctrl;
const dac_extended_cfg_t g_dac1_ext_cfg =
{ .enable_charge_pump = 0,
  .data_format = DAC_DATA_FORMAT_FLUSH_RIGHT,
  .output_amplifier_enabled = 0,
  .internal_output_enabled = false, };
const dac_cfg_t g_dac1_cfg =
{ .channel = 1, .ad_da_synchronized = false, .p_extend = &g_dac1_ext_cfg };
/* Instance structure to use this module. */
const dac_instance_t g_dac1 =
{ .p_ctrl = &g_dac1_ctrl, .p_cfg = &g_dac1_cfg, .p_api = &g_dac_on_dac };
dac_instance_ctrl_t g_dac0_ctrl;
const dac_extended_cfg_t g_dac0_ext_cfg =
{ .enable_charge_pump = 0,
  .data_format = DAC_DATA_FORMAT_FLUSH_RIGHT,
  .output_amplifier_enabled = 0,
  .internal_output_enabled = false, };
const dac_cfg_t g_dac0_cfg =
{ .channel = 0, .ad_da_synchronized = false, .p_extend = &g_dac0_ext_cfg };
/* Instance structure to use this module. */
const dac_instance_t g_dac0 =
{ .p_ctrl = &g_dac0_ctrl, .p_cfg = &g_dac0_cfg, .p_api = &g_dac_on_dac };
adc_instance_ctrl_t g_adc0_ctrl;
const adc_extended_cfg_t g_adc0_cfg_extend =
{ .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
  .adc_vref_control = ADC_VREF_CONTROL_VREFH,
  .enable_adbuf = 0,
  #if defined(VECTOR_NUMBER_ADC0_WINDOW_A)
  .window_a_irq = VECTOR_NUMBER_ADC0_WINDOW_A,
  #else
  .window_a_irq = FSP_INVALID_VECTOR,
  #endif
  .window_a_ipl = (BSP_IRQ_DISABLED),
  #if defined(VECTOR_NUMBER_ADC0_WINDOW_B)
  .window_b_irq = VECTOR_NUMBER_ADC0_WINDOW_B,
  #else
  .window_b_irq = FSP_INVALID_VECTOR,
  #endif
  .window_b_ipl = (BSP_IRQ_DISABLED), };
const adc_cfg_t g_adc0_cfg =
{ .unit = 0, .mode = ADC_MODE_SINGLE_SCAN, .resolution = ADC_RESOLUTION_12_BIT, .alignment =
      (adc_alignment_t)ADC_ALIGNMENT_RIGHT,
  .trigger = ADC_TRIGGER_SOFTWARE, .p_callback = NULL,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = &g_adc0_cfg_extend,
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
#if ((0) | (0))
const adc_window_cfg_t g_adc0_window_cfg =
{
    .compare_mask = 0,
    .compare_mode_mask = 0,
    .compare_cfg = (0) | (0) | (0) | (ADC_COMPARE_CFG_EVENT_OUTPUT_OR),
    .compare_ref_low = 0,
    .compare_ref_high = 0,
    .compare_b_channel = (ADC_WINDOW_B_CHANNEL_0),
    .compare_b_mode = (ADC_WINDOW_B_MODE_LESS_THAN_OR_OUTSIDE),
    .compare_b_ref_low = 0,
    .compare_b_ref_high = 0,
};
#endif
const adc_channel_cfg_t g_adc0_channel_cfg =
{ .scan_mask = 0,
  .scan_mask_group_b = 0,
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
  .add_mask = 0,
  .sample_hold_mask = 0,
  .sample_hold_states = 24,
  #if ((0) | (0))
  .p_window_cfg = (adc_window_cfg_t *)&g_adc0_window_cfg,
  #else
  .p_window_cfg = NULL,
  #endif
};
/* Instance structure to use this module. */
const adc_instance_t g_adc0 =
{ .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg, .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
iic_master_instance_ctrl_t g_i2c_master0_ctrl;
const iic_master_extended_cfg_t g_i2c_master0_extend =
{ .timeout_mode = IIC_MASTER_TIMEOUT_MODE_SHORT, .timeout_scl_low = IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
/* Actual calculated bitrate: 98425. Actual calculated duty cycle: 50%. */ .clock_settings.brl_value = 28,
  .clock_settings.brh_value = 28, .clock_settings.cks_value = 3, };
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
  .p_callback = NULL,
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
usb_instance_ctrl_t g_basic0_ctrl;

#if !defined(g_usb_descriptor)
extern usb_descriptor_t g_usb_descriptor;
#endif
#define RA_NOT_DEFINED (1)
const usb_cfg_t g_basic0_cfg =
{ .usb_mode = USB_MODE_PERI, .usb_speed = USB_SPEED_FS, .module_number = 0, .type = USB_CLASS_PCDC,
  #if defined(g_usb_descriptor)
  .p_usb_reg = g_usb_descriptor,
  #else
  .p_usb_reg = &g_usb_descriptor,
  #endif
  .usb_complience_cb = NULL,
  #if defined(VECTOR_NUMBER_USBFS_INT)
  .irq = VECTOR_NUMBER_USBFS_INT,
  #else
  .irq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBFS_RESUME)
  .irq_r = VECTOR_NUMBER_USBFS_RESUME,
  #else
  .irq_r = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBFS_FIFO_0)
  .irq_d0 = VECTOR_NUMBER_USBFS_FIFO_0,
  #else
  .irq_d0 = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBFS_FIFO_1)
  .irq_d1 = VECTOR_NUMBER_USBFS_FIFO_1,
  #else
  .irq_d1 = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBHS_USB_INT_RESUME)
  .hsirq = VECTOR_NUMBER_USBHS_USB_INT_RESUME,
  #else
  .hsirq = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBHS_FIFO_0)
  .hsirq_d0 = VECTOR_NUMBER_USBHS_FIFO_0,
  #else
  .hsirq_d0 = FSP_INVALID_VECTOR,
  #endif
  #if defined(VECTOR_NUMBER_USBHS_FIFO_1)
  .hsirq_d1 = VECTOR_NUMBER_USBHS_FIFO_1,
  #else
  .hsirq_d1 = FSP_INVALID_VECTOR,
  #endif
  .ipl = (12),
  .ipl_r = (12), .ipl_d0 = (12), .ipl_d1 = (12), .hsipl = (12), .hsipl_d0 = (12), .hsipl_d1 = (12),
  #if (BSP_CFG_RTOS != 0)
  .p_usb_apl_callback = NULL,
  #else
  .p_usb_apl_callback = NULL,
  #endif
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  #else
  .p_transfer_tx = &RA_NOT_DEFINED,
  #endif
  #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  #else
  .p_transfer_rx = &RA_NOT_DEFINED,
  #endif
};
#undef RA_NOT_DEFINED

/* Instance structure to use this module. */
const usb_instance_t g_basic0 =
{ .p_ctrl = &g_basic0_ctrl, .p_cfg = &g_basic0_cfg, .p_api = &g_usb_on_usb, };

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
    /* Baud rate calculated with 0.469% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 53, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart9_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
      SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart9_baud_setting, .flow_control =
      SCI_UART_FLOW_CONTROL_RTS,
  #if 0xFF != 0xFF
  .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
  #else
  .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
  #endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
    #if 0xFF != 0xFF
    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
  }, };

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
void g_hal_init(void) {
    g_common_init();
}
