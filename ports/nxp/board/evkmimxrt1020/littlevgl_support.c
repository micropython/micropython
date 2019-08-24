/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "littlevgl_support.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "board.h"
#include "littlevgl_support.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_ili9341.h"
#include "fsl_ft6x06.h"
#include "fsl_lpspi_cmsis.h"
#include "fsl_lpi2c_cmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Port Me, Start. */
#define BOARD_TOUCH_I2C Driver_I2C4
#define BOARD_TOUCH_I2C_IRQ LPI2C4_IRQn
#define BOARD_TOUCH_I2C_FREQ_FUNC LPI2C4_GetFreq
#define BOARD_TOUCH_I2C_FREQ CLOCK_GetFreq(kCLOCK_OscClk) / LPI2C_CLOCK_SOURCE_DIVIDER

#define BOARD_LCD_SPI Driver_SPI1
#define BOARD_LCD_SPI_BAUDRATE 10000000U
#define BOARD_LCD_SPI_IRQ LPSPI1_IRQn
#define BOARD_LCD_SPI_FREQ_FUNC LPSPI1_GetFreq
#define BOARD_LCD_SPI_FREQ CLOCK_GetFreq(kCLOCK_SysPllClk) / LPSPI_CLOCK_SOURCE_DIVIDER
#define BOARD_LCD_SPI_DMA_TX_IRQ DMA11_DMA27_IRQn
#define BOARD_LCD_SPI_DMA_RX_IRQ DMA10_DMA26_IRQn

/* Port Me, End. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
static ft6x06_handle_t touch_handle;
static volatile uint32_t spi_event;
static volatile bool spi_event_received;
static ft6x06_handle_t touch_handle;
static SemaphoreHandle_t s_transferDone;
static uint8_t s_frameBuffer[2][LCD_VIRTUAL_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL];
/*******************************************************************************
 * Code
 ******************************************************************************/
void lv_port_pre_init(void)
{
    /*
     * Pass in the frame buffer address to LittlevGL manually, the LV_VDB_ADR
     * and LV_VDB2_ADR should be defined to LV_VDB_ADR_INV in lv_conf.h
     */
    memset(s_frameBuffer, 0, sizeof(s_frameBuffer));
    lv_vdb_set_adr(s_frameBuffer[0], s_frameBuffer[1]);
}

uint32_t BOARD_LCD_SPI_FREQ_FUNC(void)
{
    return BOARD_LCD_SPI_FREQ;
}

uint32_t BOARD_TOUCH_I2C_FREQ_FUNC(void)
{
    return BOARD_TOUCH_I2C_FREQ;
}

static void SPI_MasterSignalEvent(uint32_t event)
{
    BaseType_t taskAwake = pdFALSE;

    xSemaphoreGiveFromISR(s_transferDone, &taskAwake);

    portYIELD_FROM_ISR(taskAwake);
}

static void SPI_WaitEvent(void)
{
    if (xSemaphoreTake(s_transferDone, portMAX_DELAY) != pdTRUE)
    {
        PRINTF("LCD SPI transfer error\r\n");
    }
}

static void DEMO_SPI_LCD_WriteCmd(uint8_t Data)
{
    GPIO_PortClear(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 1);
    SPI_WaitEvent();
}

static void DEMO_SPI_LCD_WriteData(uint8_t Data)
{
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 1);
    SPI_WaitEvent();
}

static void DEMO_SPI_LCD_WriteMultiData(const uint8_t *pData, int NumItems)
{
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(pData, NumItems);
    SPI_WaitEvent();
}

static void DEMO_InitLcdSPiClk(void)
{
    /* Set LPSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_LpspiDiv, LPSPI_CLOCK_SOURCE_DIVIDER);
    /* Set Lpspi clock source. */
    CLOCK_SetMux(kCLOCK_LpspiMux, LPSPI_CLOCK_SOURCE_SELECT);
}

void DEMO_InitLcd(void)
{
    /* Define the init structure for the data/command output pin */
    gpio_pin_config_t dc_config = {
        kGPIO_DigitalOutput,
        1,
    };

    DEMO_InitLcdSPiClk();

    /* Init data/command GPIO output . */
    GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PIN, &dc_config);

    s_transferDone = xSemaphoreCreateBinary();
    if (NULL == s_transferDone)
    {
        PRINTF("Semaphore create failed\r\n");
        assert(0);
    }

    /* FreeRTOS kernel API is used in SPI ISR, so need to set proper IRQ priority. */
    NVIC_SetPriority(BOARD_LCD_SPI_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_SetPriority(BOARD_LCD_SPI_DMA_TX_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_SetPriority(BOARD_LCD_SPI_DMA_RX_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

    /* SPI master init */
    BOARD_LCD_SPI.Initialize(SPI_MasterSignalEvent);
    BOARD_LCD_SPI.PowerControl(ARM_POWER_FULL);
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA0 | ARM_SPI_DATA_BITS(8), BOARD_LCD_SPI_BAUDRATE);

    FT9341_Init(DEMO_SPI_LCD_WriteData, DEMO_SPI_LCD_WriteCmd);
    /* Change to landscape view. */
    DEMO_SPI_LCD_WriteCmd(ILI9341_CMD_MAC);
    DEMO_SPI_LCD_WriteData(0x28);
}

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    DEMO_InitLcd();

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;      /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/
    disp_drv.disp_flush = DEMO_FlushDisplay;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

void DEMO_FlushDisplay(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    uint8_t data[4];
    const uint8_t *pdata = (const uint8_t *)color_p;
    uint32_t send_size   = (x2 - x1 + 1) * (y2 - y1 + 1) * LCD_FB_BYTE_PER_PIXEL;

    /*Column addresses*/
    DEMO_SPI_LCD_WriteCmd(ILI9341_CMD_COLADDR);
    data[0] = (x1 >> 8) & 0xFF;
    data[1] = x1 & 0xFF;
    data[2] = (x2 >> 8) & 0xFF;
    data[3] = x2 & 0xFF;
    DEMO_SPI_LCD_WriteMultiData(data, 4);

    /*Page addresses*/
    DEMO_SPI_LCD_WriteCmd(ILI9341_CMD_PAGEADDR);
    data[0] = (y1 >> 8) & 0xFF;
    data[1] = y1 & 0xFF;
    data[2] = (y2 >> 8) & 0xFF;
    data[3] = y2 & 0xFF;
    DEMO_SPI_LCD_WriteMultiData(data, 4);

    /*Memory write*/
    DEMO_SPI_LCD_WriteCmd(ILI9341_CMD_GRAM);
    DEMO_SPI_LCD_WriteMultiData(pdata, send_size);

    lv_flush_ready();
}

void lv_port_indev_init(void)
{
    lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad */
    DEMO_InitTouch();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read = DEMO_ReadTouch;
    lv_indev_drv_register(&indev_drv);
}

static void I2C_MasterSignalEvent(uint32_t event)
{
    /* Notify touch driver about status of the IO operation */
    FT6X06_EventHandler(&touch_handle, event);
}

static void DEMO_InitTouchI2cClk(void)
{
    /* Set LPI2C_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);
    /* Set Lpi2c clock source. */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
}

/*Initialize your touchpad*/
void DEMO_InitTouch(void)
{
    status_t status;

    /* The FreeRTOS kernel APIs are not used in I2C ISR, so the
     * interrupt priority could be set to 0.
     */
    NVIC_SetPriority(BOARD_TOUCH_I2C_IRQ, 0);

    DEMO_InitTouchI2cClk();

    /*Init I2C1 */
    BOARD_TOUCH_I2C.Initialize(I2C_MasterSignalEvent);
    BOARD_TOUCH_I2C.PowerControl(ARM_POWER_FULL);
    BOARD_TOUCH_I2C.Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);

    status = FT6X06_Init(&touch_handle, &BOARD_TOUCH_I2C);

    if (status != kStatus_Success)
    {
        PRINTF("Touch panel init failed\n");
        assert(0);
    }
}

/* Will be called by the library to read the touchpad */
bool DEMO_ReadTouch(lv_indev_data_t *data)
{
    touch_event_t touch_event;
    static int touch_x = 0;
    static int touch_y = 0;

    data->state = LV_INDEV_STATE_REL;

    if (kStatus_Success == FT6X06_GetSingleTouch(&touch_handle, &touch_event, &touch_x, &touch_y))
    {
        if ((touch_event == kTouch_Down) || (touch_event == kTouch_Contact))
        {
            data->state = LV_INDEV_STATE_PR;
        }
    }

    /*Set the last pressed coordinates*/
    data->point.x = LCD_WIDTH - touch_y;
    data->point.y = touch_x;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}
