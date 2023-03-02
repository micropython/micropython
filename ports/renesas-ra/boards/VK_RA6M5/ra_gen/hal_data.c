/* generated HAL source file - do not edit */
#include "hal_data.h"
/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
dtc_instance_ctrl_t g_transfer2_ctrl;

transfer_info_t g_transfer2_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_4_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 128, };

const dtc_extended_cfg_t g_transfer2_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SDHIMMC0_DMA_REQ, };
const transfer_cfg_t g_transfer2_cfg = { .p_info = &g_transfer2_info,
                                         .p_extend = &g_transfer2_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer2 = { .p_ctrl = &g_transfer2_ctrl, .p_cfg =
                                              &g_transfer2_cfg, .p_api = &g_transfer_on_dtc };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void r_sdhi_transfer_callback(sdhi_instance_ctrl_t *p_ctrl);

void g_sdmmc0_dmac_callback(dmac_callback_args_t *p_args) {
    r_sdhi_transfer_callback((sdhi_instance_ctrl_t *)p_args->p_context);
}
#endif
#undef RA_NOT_DEFINED

sdhi_instance_ctrl_t g_sdmmc0_ctrl;
sdmmc_cfg_t g_sdmmc0_cfg = { .bus_width = SDMMC_BUS_WIDTH_4_BITS, .channel = 0,
                             .p_callback = sdhi_ISR, .p_context = NULL, .block_size = 512,
                             .card_detect = SDMMC_CARD_DETECT_CD, .write_protect =
                                 SDMMC_WRITE_PROTECT_WP,

                             .p_extend = NULL, .p_lower_lvl_transfer = &g_transfer2,

                             .access_ipl = (12), .sdio_ipl = BSP_IRQ_DISABLED, .card_ipl = (12),
                             .dma_req_ipl = (12),
                             #if defined(VECTOR_NUMBER_SDHIMMC0_ACCS)
                             .access_irq = VECTOR_NUMBER_SDHIMMC0_ACCS,
                             #else
                             .access_irq = FSP_INVALID_VECTOR,
                             #endif
                             #if defined(VECTOR_NUMBER_SDHIMMC0_CARD)
                             .card_irq = VECTOR_NUMBER_SDHIMMC0_CARD,
                             #else
                             .card_irq = FSP_INVALID_VECTOR,
                             #endif
                             .sdio_irq = FSP_INVALID_VECTOR,
                             #if defined(VECTOR_NUMBER_SDHIMMC0_DMA_REQ)
                             .dma_req_irq = VECTOR_NUMBER_SDHIMMC0_DMA_REQ,
                             #else
                             .dma_req_irq = FSP_INVALID_VECTOR,
                             #endif
};
/* Instance structure to use this module. */
const sdmmc_instance_t g_sdmmc0 = { .p_ctrl = &g_sdmmc0_ctrl, .p_cfg =
                                        &g_sdmmc0_cfg, .p_api = &g_sdmmc_on_sdhi };
qspi_instance_ctrl_t g_qspi0_ctrl;

static const spi_flash_erase_command_t g_qspi0_erase_command_list[] = {
    #if 4096 > 0
    { .command = 0x20, .size = 4096 },
    #endif
    #if 32768 > 0
    { .command = 0x52, .size = 32768 },
    #endif
    #if 65536 > 0
    { .command = 0xD8, .size = 65536 },
    #endif
    #if 0xC7 > 0
    { .command = 0xC7, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
    #endif
};
static const qspi_extended_cfg_t g_qspi0_extended_cfg = {
    .min_qssl_deselect_cycles = QSPI_QSSL_MIN_HIGH_LEVEL_9_QSPCLK,
    .qspclk_div = QSPI_QSPCLK_DIV_2,
};
const spi_flash_cfg_t g_qspi0_cfg = { .spi_protocol =
                                          SPI_FLASH_PROTOCOL_EXTENDED_SPI, .read_mode =
                                          SPI_FLASH_READ_MODE_FAST_READ_QUAD_IO, .address_bytes =
                                          SPI_FLASH_ADDRESS_BYTES_3, .dummy_clocks =
                                          SPI_FLASH_DUMMY_CLOCKS_DEFAULT, .page_program_address_lines =
                                          SPI_FLASH_DATA_LINES_1, .page_size_bytes = 256, .page_program_command =
                                          0x32, .write_enable_command = 0x06, .status_command = 0x05,
                                      .write_status_bit = 0, .xip_enter_command = 0x20, .xip_exit_command =
                                          0xFF, .p_erase_command_list = &g_qspi0_erase_command_list[0],
                                      .erase_command_list_length = sizeof(g_qspi0_erase_command_list)
                                          / sizeof(g_qspi0_erase_command_list[0]), .p_extend =
                                          &g_qspi0_extended_cfg, };
/** This structure encompasses everything that is needed to use an instance of this interface. */
const spi_flash_instance_t g_qspi0 = { .p_ctrl = &g_qspi0_ctrl, .p_cfg =
                                           &g_qspi0_cfg, .p_api = &g_qspi_on_spi_flash, };
iic_master_instance_ctrl_t g_i2c_master2_ctrl;
const iic_master_extended_cfg_t g_i2c_master2_extend =
{ .timeout_mode = IIC_MASTER_TIMEOUT_MODE_SHORT, .timeout_scl_low =
      IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
  /* Actual calculated bitrate: 98425. Actual calculated duty cycle: 50%. */ .clock_settings.brl_value =
      28, .clock_settings.brh_value = 28,
  .clock_settings.cks_value = 3, };
const i2c_master_cfg_t g_i2c_master2_cfg = { .channel = 2, .rate =
                                                 I2C_MASTER_RATE_STANDARD, .slave = 0x00, .addr_mode =
                                                 I2C_MASTER_ADDR_MODE_7BIT,
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
                                             .p_callback = callback_iic, .p_context = NULL,
                                             #if defined(VECTOR_NUMBER_IIC2_RXI)
                                             .rxi_irq = VECTOR_NUMBER_IIC2_RXI,
                                             #else
                                             .rxi_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC2_TXI)
                                             .txi_irq = VECTOR_NUMBER_IIC2_TXI,
                                             #else
                                             .txi_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC2_TEI)
                                             .tei_irq = VECTOR_NUMBER_IIC2_TEI,
                                             #else
                                             .tei_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC2_ERI)
                                             .eri_irq = VECTOR_NUMBER_IIC2_ERI,
                                             #else
                                             .eri_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             .ipl = (12), .p_extend = &g_i2c_master2_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_master2 = { .p_ctrl = &g_i2c_master2_ctrl,
                                              .p_cfg = &g_i2c_master2_cfg, .p_api = &g_i2c_master_on_iic };
adc_instance_ctrl_t g_adc1_ctrl;
const adc_extended_cfg_t g_adc1_cfg_extend = { .add_average_count = ADC_ADD_OFF,
                                               .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger_group_b =
                                                   ADC_TRIGGER_SYNC_ELC, .double_trigger_mode =
                                                   ADC_DOUBLE_TRIGGER_DISABLED, .adc_vref_control =
                                                   ADC_VREF_CONTROL_VREFH, .enable_adbuf = 0,
                                               #if defined(VECTOR_NUMBER_ADC1_WINDOW_A)
                                               .window_a_irq = VECTOR_NUMBER_ADC1_WINDOW_A,
                                               #else
                                               .window_a_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_a_ipl = (BSP_IRQ_DISABLED),
                                               #if defined(VECTOR_NUMBER_ADC1_WINDOW_B)
                                               .window_b_irq = VECTOR_NUMBER_ADC1_WINDOW_B,
                                               #else
                                               .window_b_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_b_ipl = (BSP_IRQ_DISABLED), };
const adc_cfg_t g_adc1_cfg = { .unit = 1, .mode = ADC_MODE_SINGLE_SCAN,
                               .resolution = ADC_RESOLUTION_12_BIT, .alignment =
                                   (adc_alignment_t)ADC_ALIGNMENT_RIGHT, .trigger =
                                   ADC_TRIGGER_SOFTWARE, .p_callback = NULL,
                               /** If NULL then do not add & */
                               #if defined(NULL)
                               .p_context = NULL,
                               #else
                               .p_context = &NULL,
                               #endif
                               .p_extend = &g_adc1_cfg_extend,
                               #if defined(VECTOR_NUMBER_ADC1_SCAN_END)
                               .scan_end_irq = VECTOR_NUMBER_ADC1_SCAN_END,
                               #else
                               .scan_end_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_ipl = (BSP_IRQ_DISABLED),
                               #if defined(VECTOR_NUMBER_ADC1_SCAN_END_B)
                               .scan_end_b_irq = VECTOR_NUMBER_ADC1_SCAN_END_B,
                               #else
                               .scan_end_b_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
#if ((0) | (0))
const adc_window_cfg_t g_adc1_window_cfg =
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
const adc_channel_cfg_t g_adc1_channel_cfg = { .scan_mask = 0,
                                               .scan_mask_group_b = 0, .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
                                               .add_mask = 0, .sample_hold_mask = 0, .sample_hold_states = 24,
                                               #if ((0) | (0))
                                               .p_window_cfg = (adc_window_cfg_t *)&g_adc1_window_cfg,
                                               #else
                                               .p_window_cfg = NULL,
                                               #endif
};
/* Instance structure to use this module. */
const adc_instance_t g_adc1 = { .p_ctrl = &g_adc1_ctrl, .p_cfg = &g_adc1_cfg,
                                .p_channel_cfg = &g_adc1_channel_cfg, .p_api = &g_adc_on_adc };
adc_instance_ctrl_t g_adc0_ctrl;
const adc_extended_cfg_t g_adc0_cfg_extend = { .add_average_count = ADC_ADD_OFF,
                                               .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger_group_b =
                                                   ADC_TRIGGER_SYNC_ELC, .double_trigger_mode =
                                                   ADC_DOUBLE_TRIGGER_DISABLED, .adc_vref_control =
                                                   ADC_VREF_CONTROL_VREFH, .enable_adbuf = 0,
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
const adc_cfg_t g_adc0_cfg = { .unit = 0, .mode = ADC_MODE_SINGLE_SCAN,
                               .resolution = ADC_RESOLUTION_12_BIT, .alignment =
                                   (adc_alignment_t)ADC_ALIGNMENT_RIGHT, .trigger =
                                   ADC_TRIGGER_SOFTWARE, .p_callback = NULL,
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
const adc_channel_cfg_t g_adc0_channel_cfg = { .scan_mask = 0,
                                               .scan_mask_group_b = 0, .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
                                               .add_mask = 0, .sample_hold_mask = 0, .sample_hold_states = 24,
                                               #if ((0) | (0))
                                               .p_window_cfg = (adc_window_cfg_t *)&g_adc0_window_cfg,
                                               #else
                                               .p_window_cfg = NULL,
                                               #endif
};
/* Instance structure to use this module. */
const adc_instance_t g_adc0 = { .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg,
                                .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
lpm_instance_ctrl_t g_lpm0_ctrl;

const lpm_cfg_t g_lpm0_cfg = { .low_power_mode = LPM_MODE_SLEEP,
                               .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_RTCALM
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

const lpm_instance_t g_lpm0 = { .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm0_ctrl,
                                .p_cfg = &g_lpm0_cfg };
dtc_instance_ctrl_t g_transfer1_ctrl;

transfer_info_t g_transfer1_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_DESTINATION, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_2_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer1_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SPI0_RXI, };
const transfer_cfg_t g_transfer1_cfg = { .p_info = &g_transfer1_info,
                                         .p_extend = &g_transfer1_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 = { .p_ctrl = &g_transfer1_ctrl, .p_cfg =
                                              &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer0_ctrl;

transfer_info_t g_transfer0_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_2_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer0_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SPI0_TXI, };
const transfer_cfg_t g_transfer0_cfg = { .p_info = &g_transfer0_info,
                                         .p_extend = &g_transfer0_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 = { .p_ctrl = &g_transfer0_ctrl, .p_cfg =
                                              &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_tx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi0_tx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_tx_dmac_callback(&g_spi0_ctrl);
}
#endif

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_rx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi0_rx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_rx_dmac_callback(&g_spi0_ctrl);
}
#endif
#undef RA_NOT_DEFINED

spi_instance_ctrl_t g_spi0_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t g_spi0_ext_cfg = { .spi_clksyn = SPI_SSL_MODE_CLK_SYN,
                                            .spi_comm = SPI_COMMUNICATION_FULL_DUPLEX, .ssl_polarity = SPI_SSLP_LOW,
                                            .ssl_select = SPI_SSL_SELECT_SSL0, .mosi_idle =
                                                SPI_MOSI_IDLE_VALUE_FIXING_DISABLE, .parity =
                                                SPI_PARITY_MODE_DISABLE, .byte_swap = SPI_BYTE_SWAP_DISABLE,
                                            .spck_div = {
                                                /* Actual calculated bitrate: 12500000. */ .spbr = 3, .brdv = 0
                                            },
                                            .spck_delay = SPI_DELAY_COUNT_1,
                                            .ssl_negation_delay = SPI_DELAY_COUNT_1, .next_access_delay =
                                                SPI_DELAY_COUNT_1 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t g_spi0_cfg = { .channel = 0,

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

                               .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),

                               .operating_mode = SPI_MODE_MASTER,

                               .clk_phase = SPI_CLK_PHASE_EDGE_ODD, .clk_polarity =
                                   SPI_CLK_POLARITY_LOW,

                               .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE, .bit_order =
                                   SPI_BIT_ORDER_MSB_FIRST, .p_transfer_tx = g_spi0_P_TRANSFER_TX,
                               .p_transfer_rx = g_spi0_P_TRANSFER_RX, .p_callback = spi_callback,

                               .p_context = NULL, .p_extend = (void *)&g_spi0_ext_cfg, };

/* Instance structure to use this module. */
const spi_instance_t g_spi0 = { .p_ctrl = &g_spi0_ctrl, .p_cfg = &g_spi0_cfg,
                                .p_api = &g_spi_on_spi };
agt_instance_ctrl_t g_timer1_ctrl;
const agt_extended_cfg_t g_timer1_extend =
{ .count_source = AGT_CLOCK_PCLKB, .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED, .agtio_filter =
      AGT_AGTIO_FILTER_NONE, .enable_pin =
      AGT_ENABLE_PIN_NOT_USED, .trigger_edge =
      AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer1_cfg = { .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.00131072 seconds. Actual duty: 50%. */ .period_counts =
                                       (uint32_t)0x10000, .duty_cycle_counts = 0x8000, .source_div =
                                       (timer_source_div_t)0, .channel = 1, .p_callback = callback_agt,
/** If NULL then do not add & */
                                   #if defined(NULL)
                                   .p_context = NULL,
                                   #else
                                   .p_context = &NULL,
                                   #endif
                                   .p_extend = &g_timer1_extend, .cycle_end_ipl = (5),
                                   #if defined(VECTOR_NUMBER_AGT1_INT)
                                   .cycle_end_irq = VECTOR_NUMBER_AGT1_INT,
                                   #else
                                   .cycle_end_irq = FSP_INVALID_VECTOR,
                                   #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer1 = { .p_ctrl = &g_timer1_ctrl, .p_cfg =
                                        &g_timer1_cfg, .p_api = &g_timer_on_agt };
agt_instance_ctrl_t g_timer0_ctrl;
const agt_extended_cfg_t g_timer0_extend =
{ .count_source = AGT_CLOCK_PCLKB, .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED, .agtio_filter =
      AGT_AGTIO_FILTER_NONE, .enable_pin =
      AGT_ENABLE_PIN_NOT_USED, .trigger_edge =
      AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer0_cfg = { .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.00131072 seconds. Actual duty: 50%. */ .period_counts =
                                       (uint32_t)0x10000, .duty_cycle_counts = 0x8000, .source_div =
                                       (timer_source_div_t)0, .channel = 0, .p_callback = callback_agt,
/** If NULL then do not add & */
                                   #if defined(NULL)
                                   .p_context = NULL,
                                   #else
                                   .p_context = &NULL,
                                   #endif
                                   .p_extend = &g_timer0_extend, .cycle_end_ipl = (5),
                                   #if defined(VECTOR_NUMBER_AGT0_INT)
                                   .cycle_end_irq = VECTOR_NUMBER_AGT0_INT,
                                   #else
                                   .cycle_end_irq = FSP_INVALID_VECTOR,
                                   #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer0 = { .p_ctrl = &g_timer0_ctrl, .p_cfg =
                                        &g_timer0_cfg, .p_api = &g_timer_on_agt };
flash_hp_instance_ctrl_t g_flash0_ctrl;
const flash_cfg_t g_flash0_cfg = { .data_flash_bgo = false, .p_callback = NULL,
                                   .p_context = NULL,
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
                                   .err_ipl = (BSP_IRQ_DISABLED), .ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const flash_instance_t g_flash0 = { .p_ctrl = &g_flash0_ctrl, .p_cfg =
                                        &g_flash0_cfg, .p_api = &g_flash_on_flash_hp };
rtc_instance_ctrl_t g_rtc0_ctrl;
const rtc_error_adjustment_cfg_t g_rtc0_err_cfg = { .adjustment_mode =
                                                        RTC_ERROR_ADJUSTMENT_MODE_AUTOMATIC, .adjustment_period =
                                                        RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND, .adjustment_type =
                                                        RTC_ERROR_ADJUSTMENT_NONE, .adjustment_value = 0, };
const rtc_cfg_t g_rtc0_cfg = { .clock_source = RTC_CLOCK_SOURCE_LOCO,
                               .freq_compare_value_loco = 255, .p_err_cfg = &g_rtc0_err_cfg,
                               .p_callback = NULL, .p_context = NULL, .alarm_ipl = (14),
                               .periodic_ipl = (14), .carry_ipl = (14),
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
const rtc_instance_t g_rtc0 = { .p_ctrl = &g_rtc0_ctrl, .p_cfg = &g_rtc0_cfg,
                                .p_api = &g_rtc_on_rtc };
sci_uart_instance_ctrl_t g_uart9_ctrl;

baud_setting_t g_uart9_baud_setting = {
/* Baud rate calculated with 0.469% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 53, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart9_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart9_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart9_cfg = { .channel = 9, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart9_cfg_extend,
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
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
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
const uart_instance_t g_uart9 = { .p_ctrl = &g_uart9_ctrl,
                                  .p_cfg = &g_uart9_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart7_ctrl;

baud_setting_t g_uart7_baud_setting = {
/* Baud rate calculated with 0.469% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 53, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart7_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart7_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart7_cfg = { .channel = 7, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart7_cfg_extend,
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
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
                                 #if defined(VECTOR_NUMBER_SCI7_RXI)
                                 .rxi_irq = VECTOR_NUMBER_SCI7_RXI,
                                 #else
                                 .rxi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_TXI)
                                 .txi_irq = VECTOR_NUMBER_SCI7_TXI,
                                 #else
                                 .txi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_TEI)
                                 .tei_irq = VECTOR_NUMBER_SCI7_TEI,
                                 #else
                                 .tei_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_ERI)
                                 .eri_irq = VECTOR_NUMBER_SCI7_ERI,
                                 #else
                                 .eri_irq = FSP_INVALID_VECTOR,
                                 #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart7 = { .p_ctrl = &g_uart7_ctrl,
                                  .p_cfg = &g_uart7_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart6_ctrl;

baud_setting_t g_uart6_baud_setting = {
/* Baud rate calculated with 0.469% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 53, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart6_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart6_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart6_cfg = { .channel = 6, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart6_cfg_extend,
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
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
                                 #if defined(VECTOR_NUMBER_SCI6_RXI)
                                 .rxi_irq = VECTOR_NUMBER_SCI6_RXI,
                                 #else
                                 .rxi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI6_TXI)
                                 .txi_irq = VECTOR_NUMBER_SCI6_TXI,
                                 #else
                                 .txi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI6_TEI)
                                 .tei_irq = VECTOR_NUMBER_SCI6_TEI,
                                 #else
                                 .tei_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI6_ERI)
                                 .eri_irq = VECTOR_NUMBER_SCI6_ERI,
                                 #else
                                 .eri_irq = FSP_INVALID_VECTOR,
                                 #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart6 = { .p_ctrl = &g_uart6_ctrl,
                                  .p_cfg = &g_uart6_cfg, .p_api = &g_uart_on_sci };
void g_hal_init(void) {
    g_common_init();
}
