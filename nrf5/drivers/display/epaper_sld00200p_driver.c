/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epaper_sld00200p_driver.h"

#if MICROPY_PY_DISPLAY_EPAPER_SLD00200P

#include "py/mphal.h"

#include "epaper_sld00200p_driver.h"
#include "hal_spi.h"
#include "hal_time.h"

#define BYTE_ARRAY(...) ((uint8_t[]){ __VA_ARGS__})
#define DATA_WRITE(...) (data_write_buffer(BYTE_ARRAY(__VA_ARGS__), sizeof(BYTE_ARRAY(__VA_ARGS__))))

static NRF_SPI_Type * mp_spi_instance;
static NRF_PWM_Type * mp_pwm_instance;
static pin_obj_t * mp_pin_cs;
static pin_obj_t * mp_pin_panel_on;
static pin_obj_t * mp_pin_border;
static pin_obj_t * mp_pin_busy;
static pin_obj_t * mp_pin_reset;
static pin_obj_t * mp_pin_discharge;

#if 0
static pin_obj_t * mp_pin_temp_sensor;
#endif

static void wait_for_busy_release(void) {
    while (mp_hal_pin_read(mp_pin_busy) == 1) {
        ;
    }
}

static void data_write_buffer(uint8_t * p_bytes, uint16_t num_of_bytes) {
    mp_hal_pin_low(mp_pin_cs);

    hal_spi_master_tx_rx(mp_spi_instance, num_of_bytes, p_bytes, NULL);

    mp_hal_pin_high(mp_pin_cs);
}

static void raw_write(uint8_t value)
{
    hal_spi_master_tx_rx(mp_spi_instance, 1, &value, NULL);
}

void driver_sld00200p_init(NRF_SPI_Type * p_spi_instance,
                           NRF_PWM_Type * p_pwm_instance,
                           pin_obj_t * p_pin_cs,
                           pin_obj_t * p_pin_panel_on,
                           pin_obj_t * p_pin_border,
                           pin_obj_t * p_pin_busy,
                           pin_obj_t * p_pin_reset,
                           pin_obj_t * p_pin_discharge) {

    mp_spi_instance  = p_spi_instance;
    mp_pwm_instance  = p_pwm_instance;
    mp_pin_cs        = p_pin_cs;
    mp_pin_panel_on  = p_pin_panel_on;
    mp_pin_border    = p_pin_border;
    mp_pin_busy      = p_pin_busy;
    mp_pin_reset     = p_pin_reset;
    mp_pin_discharge = p_pin_discharge;

    driver_sld00200p_reinit();
}

void driver_sld00200p_reinit(void) {
    mp_hal_pin_low(mp_pin_reset);
    mp_hal_pin_low(mp_pin_panel_on);
    mp_hal_pin_low(mp_pin_discharge);
    mp_hal_pin_low(mp_pin_border);

    // start the pwm
    hal_pwm_start(mp_pwm_instance);

    mp_hal_delay_ms(5);

    mp_hal_pin_high(mp_pin_panel_on);

    mp_hal_delay_ms(10);

    mp_hal_pin_high(mp_pin_reset);
    mp_hal_pin_high(mp_pin_border);
    mp_hal_pin_high(mp_pin_cs);

    // make reset square wave
    mp_hal_delay_ms(5);
    mp_hal_pin_low(mp_pin_reset);
    mp_hal_delay_ms(5);
    mp_hal_pin_high(mp_pin_reset);
    mp_hal_delay_ms(5);

    wait_for_busy_release();

    // channel select
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x01);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00);

    // DC/DC frequency
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x06);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00);

    // high power mode osc
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x07);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x9d);

    // disable ADC
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x08);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00);

    // Vcom level
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x09);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0xd0, 0x00);

    // gate and source voltage levels
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x04);

    // GS
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00);

    mp_hal_delay_ms(5);

    // driver latch on
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x03);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x01);

    // driver latch off
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x03);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00);

    mp_hal_delay_ms(5);

    // charge pump positive voltage on
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x01);

    // final delay before PWM off
    mp_hal_delay_us(30);

    // stop the pwm
    hal_pwm_stop(mp_pwm_instance);

    // charge pump negative voltage on
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x03);

    mp_hal_delay_us(30);

    // Vcom driver on
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x0f);

    mp_hal_delay_ms(30);

    // output enable to disable
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x02);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x24);
}

static void epaper_sld00200p_line(uint16_t line, const uint8_t * data, uint8_t fixed_value, epd_stage_t stage)
{
    mp_hal_delay_ms(10);

    DATA_WRITE(0x70, 0x04);
    wait_for_busy_release();

    // gate source
    DATA_WRITE(0x72, 0x00);
    wait_for_busy_release();

    DATA_WRITE(0x70, 0x0a);
    wait_for_busy_release();

    mp_hal_pin_low(mp_pin_cs);
    raw_write(0x72);
    wait_for_busy_release();

    uint16_t bytes_per_line = 264 / 8;

    // even pixels
    for (uint16_t i = bytes_per_line; i > 0; --i) {
        if (data != NULL) {
            uint8_t pixels = data[i - 1] & 0xaa;

            switch (stage) {
                case EPD_COMP:
                    // B -> W, W -> B (current image)
                    pixels = 0xaa | ((pixels ^ 0xaa) >> 1);
                    break;

                case EPD_WHITE:
                    // B -> N, W -> W (current image)
                    pixels = 0x55 + ((pixels ^ 0xaa) >> 1);
                    break;

                case EPD_INV:
                    // B -> N, W -> B (new image)
                    pixels = 0x55 | (pixels ^ 0xaa);
                    break;

                case EPD_NORM:
                    // B -> B, W -> W (new image)
                    pixels = 0xaa | (pixels >> 1);
                    break;

                default:
                    break;
            }

            raw_write(pixels);
            wait_for_busy_release();
        } else {
            raw_write(fixed_value);
            wait_for_busy_release();
        }
    }

    uint16_t bytes_per_scan = 176 / 4;
    // scan line
    for (uint16_t i = 0; i < bytes_per_scan; i++) {
        if (line / 4 == i) {
            raw_write(0xc0 >> (2 * (line & 0x03)));
            wait_for_busy_release();
        } else {
            raw_write(0x00);
            wait_for_busy_release();
        }
    }

    // odd pixels
    for (uint16_t i = 0; i < bytes_per_line; i++) {
        if (data != NULL) {
            uint8_t pixels;
            pixels = data[i] & 0x55;

            switch (stage) {
                case EPD_COMP:
                    pixels = 0xaa | (pixels ^ 0x55);
                    break;

                case EPD_WHITE:
                    pixels = 0x55 + (pixels ^ 0x55);
                    break;

                case EPD_INV:
                    pixels = 0x55 | ((pixels ^ 0x55) << 1);
                    break;

                case EPD_NORM:
                    pixels = 0xaa | pixels;
                    break;

                default:
                    break;
            }

            uint8_t p1 = (pixels >> 6) & 0x03;
            uint8_t p2 = (pixels >> 4) & 0x03;
            uint8_t p3 = (pixels >> 2) & 0x03;
            uint8_t p4 = (pixels >> 0) & 0x03;
            pixels = (p1 << 0) | (p2 << 2) | (p3 << 4) | (p4 << 6);

            raw_write(pixels);
            wait_for_busy_release();
        } else {
            raw_write(fixed_value);
            wait_for_busy_release();
        }
    }

    // Complete line
    raw_write(0x00);
    wait_for_busy_release();

    mp_hal_pin_high(mp_pin_cs);
    wait_for_busy_release();

    DATA_WRITE(0x70, 0x02);
    wait_for_busy_release();

    DATA_WRITE(0x72, 0x2f);
}

void driver_sld00200p_deinit(void) {
    epaper_sld00200p_line(0x7fffu, 0, 0x55, EPD_NORM);
    mp_hal_delay_ms(25);
    mp_hal_pin_low(mp_pin_border);
    mp_hal_delay_ms(250);
    mp_hal_pin_high(mp_pin_border);

    // latch reset turn on
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x03);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x01);

    // output enable off
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x02);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x05);

    // Vcom power off
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x0e);

    // power off negative charge pump
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x02);

    // discharge
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x04);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x0c);
    mp_hal_delay_us(120);

    // all charge pumps off
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x05);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00);

    // turn of osc
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x07);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x0d);

    // discharge internal - 1
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x04);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x50);
    mp_hal_delay_us(40);

    // discharge internal - 2
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x04);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0xA0);
    mp_hal_delay_us(40);

    // discharge internal - 3
    mp_hal_delay_us(10);
    DATA_WRITE(0x70, 0x04);
    mp_hal_delay_us(10);
    DATA_WRITE(0x72, 0x00);

    // turn of power and all signals
    mp_hal_delay_ms(10);
    mp_hal_pin_low(mp_pin_reset);
    mp_hal_pin_low(mp_pin_panel_on);
    mp_hal_pin_low(mp_pin_border);

    // discharge pulse
    mp_hal_pin_high(mp_pin_discharge);
    mp_hal_delay_us(250);
    mp_hal_pin_low(mp_pin_discharge);
    mp_hal_pin_high(mp_pin_cs);
}

void driver_sld00200p_clear(uint16_t color) {
    uint16_t line_count = 176;
    for (uint16_t i = 0; i < line_count; i++) {
        epaper_sld00200p_line(i, NULL, 0xFF, EPD_COMP);
    }
    mp_hal_delay_ms(100);

    for (uint16_t i = 0; i < line_count; i++) {
        epaper_sld00200p_line(i, NULL, 0xAA, EPD_WHITE);
    }
    mp_hal_delay_ms(100);

    for (uint16_t i = 0; i < line_count; i++) {
        epaper_sld00200p_line(i, NULL, 0xFF, EPD_INV);
    }
    mp_hal_delay_ms(100);

    for (uint16_t i = 0; i < line_count; i++) {
        epaper_sld00200p_line(i, NULL, 0xAA, EPD_NORM);
    }
    mp_hal_delay_ms(100);
}

void driver_sld00200p_update_line(uint16_t line, framebuffer_byte_t * p_bytes, framebuffer_byte_t * p_old, uint16_t len) {
    epaper_sld00200p_line(line, (uint8_t *)p_old, 0x00, EPD_COMP);
    epaper_sld00200p_line(line, (uint8_t *)p_old, 0xAA, EPD_WHITE);
    epaper_sld00200p_line(line, (uint8_t *)p_bytes, 0xAA, EPD_INV);
    epaper_sld00200p_line(line, (uint8_t *)p_bytes, 0xFF, EPD_NORM);
}

#endif
