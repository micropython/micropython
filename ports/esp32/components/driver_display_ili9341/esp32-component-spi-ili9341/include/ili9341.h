/**
 * Copyright (c) 2022 Nicolai Electronics
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define ILI9341_WIDTH       320
#define ILI9341_HEIGHT      240
#define ILI9341_BUFFER_SIZE ILI9341_WIDTH * ILI9341_HEIGHT * 2 // Each pixel takes 16 bits
#define ILI9341_BPP         16

// Registers
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01 // Software Reset
#define ILI9341_RDDID       0x04 // Read Display ID
#define ILI9341_RDDST       0x09 // Read Display Status
#define ILI9341_RDDPM       0x0A // Read Display Power
#define ILI9341_RDDMADCTL   0x0B // Read Display Memory Data Access Mode
#define ILI9341_RDDCOLMOD   0x0C // Read Display Pixel
#define ILI9341_RDDIM       0x0D // Read Display Image
#define ILI9341_RDDSM       0x0E // Read Display Signal
#define ILI9341_RDDSDR      0x0F // Read Display Self Diagnostics
#define ILI9341_SLPIN       0x10 // Sleep In
#define ILI9341_SLPOUT      0x11 // Sleep Out
#define ILI9341_PTLON       0x12 // Partial Mode On
#define ILI9341_NORON       0x13 // Partial Mode Off
#define ILI9341_INVOFF      0x20 // Display Invert Off
#define ILI9341_INVON       0x21 // Display Invert On
#define ILI9341_GAMSET      0x26 // Display Invert On Gamma
#define ILI9341_DISPOFF     0x28 // Display Off
#define ILI9341_DISPON      0x29 // Display On
#define ILI9341_CASET       0x2A // Column Address Set
#define ILI9341_RASET       0x2B // Row Address Set
#define ILI9341_RAMWR       0x2C // Memory Write
#define ILI9341_RAMRD       0x2E // Memory Read
#define ILI9341_PTLAR       0x30 // Partial Start/End Address Set
#define ILI9341_VSCRDEF     0x33 // Vertical Scrolling Definition
#define ILI9341_TEOFF       0x34 // Tearing Effect Line Off
#define ILI9341_TEON        0x35 // Tearing Effect Line On
#define ILI9341_MADCTL      0x36 // Memory Data Access Control
#define ILI9341_VSCRSADD    0x37 // Vertical Scrolling Start Address
#define ILI9341_IDMOFF      0x38 // Idle Mode Off
#define ILI9341_IDMON       0x39 // Idle Mode On
#define ILI9341_COLMOD      0x3A // Interface Pixel Format
#define ILI9341_RAMWRC      0x3C // Memory Write Continue
#define ILI9341_RAMRDC      0x3E // Memory Read Continue
#define ILI9341_TESCAN      0x44 // Set Tear Scan Line
#define ILI9341_RDTESCAN    0x45 // Get Tear Scan Line
#define ILI9341_WRDISBV     0x51 // Set Display Brightness
#define ILI9341_RDDISBV     0x52 // Get Display Brightness
#define ILI9341_WRCTRLD     0x53 // Set Display Control
#define ILI9341_RDCTRLD     0x54 // Get Display Control
#define ILI9341_WRCACE      0x55 // Write content adaptive brightness control and Color enhancement
#define ILI9341_RDCABC      0x56 // Read content adaptive brightness control and Color enhancement
#define ILI9341_WRCABCMB    0x5E // Write CABC minimum brightness
#define ILI9341_RDCABCMB    0x5F // Read CABC minimum brightness
#define ILI9341_RDABCSDR    0x68 // Read Automatic Brightness Control Self-Diagnostic Result
#define ILI9341_PORCTRK     0xB2 // Porch setting
#define ILI9341_GCTRL       0xB7 // Gate Control
#define ILI9341_VCOMS       0xBB // VCOM setting
#define ILI9341_LCMCTRL     0xC0 // LCM Control
#define ILI9341_VDVVRHEN    0xC2 // VDV and VRH Command Enable
#define ILI9341_VRHS        0xC3 // VRH Set
#define ILI9341_VDVS        0xC4 // VDV Set
#define ILI9341_FRCTRL2     0xC6 // Frame Rate control in normal mode
#define ILI9341_PWCTRL1     0xD0 // Power Control 1
#define ILI9341_RDID1       0xDA // Read ID1
#define ILI9341_RDID2       0xDB // Read ID2
#define ILI9341_RDID3       0xDC // Read ID3
#define ILI9341_PVGAMCTRL   0xE0 // Positive Voltage Gamma control
#define ILI9341_NVGAMCTRL   0xE1 // Negative Voltage Gamma control

// Extended command set
#define ILI9341_RGB_INTERFACE 0xB0 // RGB Interface Signal Control
#define ILI9341_FRMCTR1       0xB1 // Frame Rate Control (In Normal Mode)
#define ILI9341_FRMCTR2       0xB2 // Frame Rate Control (In Idle Mode)
#define ILI9341_FRMCTR3       0xB3 // Frame Rate Control (In Partial Mode)
#define ILI9341_INVTR         0xB4 // Display Inversion Control
#define ILI9341_BPC           0xB5 // Blanking Porch Control register
#define ILI9341_DFC           0xB6 // Display Function Control register
#define ILI9341_ETMOD         0xB7 // Entry Mode Set
#define ILI9341_BACKLIGHT1    0xB8 // Backlight Control 1
#define ILI9341_BACKLIGHT2    0xB9 // Backlight Control 2
#define ILI9341_BACKLIGHT3    0xBA // Backlight Control 3
#define ILI9341_BACKLIGHT4    0xBB // Backlight Control 4
#define ILI9341_BACKLIGHT5    0xBC // Backlight Control 5
#define ILI9341_BACKLIGHT7    0xBE // Backlight Control 7
#define ILI9341_BACKLIGHT8    0xBF // Backlight Control 8
#define ILI9341_POWER1        0xC0 // Power Control 1 register
#define ILI9341_POWER2        0xC1 // Power Control 2 register
#define ILI9341_VCOM1         0xC5 // VCOM Control 1 register
#define ILI9341_VCOM2         0xC7 // VCOM Control 2 register
#define ILI9341_NVMWR         0xD0 // NV Memory Write
#define ILI9341_NVMPKEY       0xD1 // NV Memory Protection Key
#define ILI9341_RDNVM         0xD2 // NV Memory Status Read
#define ILI9341_READ_ID4      0xD3 // Read ID4
#define ILI9341_PGAMMA        0xE0 // Positive Gamma Correction register
#define ILI9341_NGAMMA        0xE1 // Negative Gamma Correction register
#define ILI9341_DGAMCTRL1     0xE2 // Digital Gamma Control 1
#define ILI9341_DGAMCTRL2     0xE3 // Digital Gamma Control 2
#define ILI9341_INTERFACE     0xF6 // Interface control register

// Extend register commands
#define ILI9341_POWERA        0xCB // Power control A register
#define ILI9341_POWERB        0xCF // Power control B register
#define ILI9341_DTCA          0xE8 // Driver timing control A
#define ILI9341_DTCB          0xEA // Driver timing control B
#define ILI9341_POWER_SEQ     0xED // Power on sequence register
#define ILI9341_3GAMMA_EN     0xF2 // 3 Gamma enable register
#define ILI9341_PRC           0xF7 // Pump ratio control register

typedef void (*ili9341_cb_t)(bool); // Callback for init / deinit

typedef struct ILI9341 {
    // Pins
    int spi_bus;
    int pin_cs;
    int pin_dcx;
    int pin_reset;
    // Configuration
    uint8_t rotation;
    bool color_mode;
    uint32_t spi_speed;
    uint32_t spi_max_transfer_size;
    ili9341_cb_t callback;
    // Internal state
    spi_device_handle_t spi_device;
    bool dc_level;
    // Mutex
    SemaphoreHandle_t mutex;
} ILI9341;

esp_err_t ili9341_init(ILI9341* device);
esp_err_t ili9341_deinit(ILI9341* device);

esp_err_t ili9341_set_sleep(ILI9341* device, const bool state);
esp_err_t ili9341_set_display(ILI9341* device, const bool state);
esp_err_t ili9341_set_invert(ILI9341* device, const bool state);
esp_err_t ili9341_set_partial_scanning(ILI9341* device, const uint16_t start, const uint16_t end);
esp_err_t ili9341_set_tearing_effect_line(ILI9341* device, const bool state);
esp_err_t ili9341_set_idle_mode(ILI9341* device, const bool state);

esp_err_t ili9341_write(ILI9341* device, const uint8_t *data);
esp_err_t ili9341_write_partial_direct(ILI9341* device, const uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
esp_err_t ili9341_write_partial(ILI9341* device, const uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

esp_err_t ili9341_select(ILI9341* device, const bool mode);

#ifdef __cplusplus
}
#endif //__cplusplus
