/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "systick.h"
#include "dma.h"
#include "irq.h"

#define DMA_IDLE_ENABLED()  (dma_idle.enabled != 0)
#define DMA_SYSTICK_LOG2    (3)
#define DMA_SYSTICK_MASK    ((1 << DMA_SYSTICK_LOG2) - 1)
#define DMA_IDLE_TICK_MAX   (8) // 8*8 = 64 msec
#define DMA_IDLE_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & DMA_SYSTICK_MASK) == 0)

typedef enum {
    dma_id_not_defined=-1,
    dma_id_0,
    dma_id_1,
    dma_id_2,
    dma_id_3,
    dma_id_4,
    dma_id_5,
    dma_id_6,
    dma_id_7,
    dma_id_8,
    dma_id_9,
    dma_id_10,
    dma_id_11,
} dma_id_t;

typedef union {
    uint16_t enabled; // Used to test if both counters are == 0
    uint8_t counter[2];
} dma_idle_count_t;

struct _dma_descr_t {    
    DMA_Channel_TypeDef *instance; /* dma通道实例 */
    uint32_t sub_instance;         /* DMA要用于什么外设的哪个通道 */
    dma_id_t id;                   /* 中断号的索引 */
    const DMA_InitTypeDef *init;
};

// Default parameters to dma_init() shared by spi and i2c; Channel and Direction
// vary depending on the peripheral instance so they get passed separately
static const DMA_InitTypeDef dma_init_struct_spi_i2c = {
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_LOW,
};

#if MICROPY_HW_ENABLE_SDCARD
// Parameters to dma_init() for SDIO tx and rx.
static const DMA_InitTypeDef dma_init_struct_sdio = {
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .MemDataAlignment    = DMA_MDATAALIGN_WORD,
    .Mode                = DMA_NORMAL, /* fixme */
    .Priority            = DMA_PRIORITY_VERY_HIGH,
};
#endif

#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
// Default parameters to dma_init() for DAC tx
static const DMA_InitTypeDef dma_init_struct_dac = {
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_HIGH,
};
#endif

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (12)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_channel) ((dma_channel) >> ((dma_channel >> 28) * 4))

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponding to DMA1 (7 channels)
#define DMA2_ENABLE_MASK (0x0f80) // Bits in dma_enable_mask corresponding to DMA2 (5 channels)

// These descriptors are ordered by DMAx_Stream number, and within a stream by channel
// number. The duplicate channels are ok as long as they aren't used at the same time.
//
// Currently I2C and SPI are synchronous and they call dma_init/dma_deinit
// around each transfer.
// DMA1 channels
const dma_descr_t dma_SPI_1_RX = { DMA1_Channel2, MP_DMA1_CH2_SPI1_RX, dma_id_1, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA1_Channel3, MP_DMA1_CH3_SPI1_TX, dma_id_2, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel4, MP_DMA1_CH4_SPI2_RX, dma_id_3, &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Channel4, MP_DMA1_CH4_I2C2_TX, dma_id_3, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel5, MP_DMA1_CH5_SPI2_TX, dma_id_4, &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Channel5, MP_DMA1_CH5_I2C2_RX, dma_id_4, &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Channel6, MP_DMA1_CH6_I2C1_TX, dma_id_5, &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_RX = { DMA1_Channel7, MP_DMA1_CH7_I2C1_RX, dma_id_6, &dma_init_struct_spi_i2c };
// DMA2 channels
const dma_descr_t dma_SPI_3_RX = { DMA2_Channel1, MP_DMA2_CH1_SPI3_RX, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_TX = { DMA2_Channel2, MP_DMA2_CH2_SPI3_TX, dma_id_8,   &dma_init_struct_spi_i2c };
#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA2_Channel3, MP_DMA2_CH3_DAC_Channel1, dma_id_9,   &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA2_Channel4, MP_DMA2_CH4_DAC_Channel2, dma_id_10,  &dma_init_struct_dac };
#endif
#if MICROPY_HW_ENABLE_SDCARD
const dma_descr_t dma_SDIO_0 = { DMA2_Channel4, MP_DMA2_CH4_SDIO, dma_id_10,  &dma_init_struct_sdio };
#endif

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Channel1_IRQn,
    DMA1_Channel2_IRQn,
    DMA1_Channel3_IRQn,
    DMA1_Channel4_IRQn,
    DMA1_Channel5_IRQn,
    DMA1_Channel6_IRQn,
    DMA1_Channel7_IRQn,
    DMA2_Channel1_IRQn,
    DMA2_Channel2_IRQn,
    DMA2_Channel3_IRQn,
    DMA2_Channel4_IRQn,
    DMA2_Channel4_5_IRQn,
};

static DMA_HandleTypeDef *dma_handle[NSTREAM] = {NULL};
static uint8_t dma_last_sub_instance[NSTREAM];
static volatile uint32_t dma_enable_mask = 0;
volatile dma_idle_count_t dma_idle;

#define DMA_INVALID_CHANNEL 0xff    // Value stored in dma_last_channel which means invalid
#define DMA1_IS_CLK_ENABLED()   ((RCC->AHBENR & RCC_AHBENR_DMA1EN) != 0)
#define DMA2_IS_CLK_ENABLED()   ((RCC->AHBENR & RCC_AHBENR_DMA2EN) != 0)


void DMA1_Channel1_IRQHandler(void) { IRQ_ENTER(DMA1_Channel1_IRQn); if (dma_handle[dma_id_0] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_0]); } IRQ_EXIT(DMA1_Channel1_IRQn); }
void DMA1_Channel2_IRQHandler(void) { IRQ_ENTER(DMA1_Channel2_IRQn); if (dma_handle[dma_id_1] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_1]); } IRQ_EXIT(DMA1_Channel2_IRQn); }
void DMA1_Channel3_IRQHandler(void) { IRQ_ENTER(DMA1_Channel3_IRQn); if (dma_handle[dma_id_2] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_2]); } IRQ_EXIT(DMA1_Channel3_IRQn); }
void DMA1_Channel4_IRQHandler(void) { IRQ_ENTER(DMA1_Channel4_IRQn); if (dma_handle[dma_id_3] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_3]); } IRQ_EXIT(DMA1_Channel4_IRQn); }
void DMA1_Channel5_IRQHandler(void) { IRQ_ENTER(DMA1_Channel5_IRQn); if (dma_handle[dma_id_4] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_4]); } IRQ_EXIT(DMA1_Channel5_IRQn); }
void DMA1_Channel6_IRQHandler(void) { IRQ_ENTER(DMA1_Channel6_IRQn); if (dma_handle[dma_id_5] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_5]); } IRQ_EXIT(DMA1_Channel6_IRQn); }
void DMA1_Channel7_IRQHandler(void) { IRQ_ENTER(DMA1_Channel7_IRQn); if (dma_handle[dma_id_6] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_6]); } IRQ_EXIT(DMA1_Channel7_IRQn); }
void DMA2_Channel1_IRQHandler(void) { IRQ_ENTER(DMA2_Channel1_IRQn); if (dma_handle[dma_id_7] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_7]); } IRQ_EXIT(DMA2_Channel1_IRQn); }
void DMA2_Channel2_IRQHandler(void) { IRQ_ENTER(DMA2_Channel2_IRQn); if (dma_handle[dma_id_8] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_8]); } IRQ_EXIT(DMA2_Channel2_IRQn); }
void DMA2_Channel3_IRQHandler(void) { IRQ_ENTER(DMA2_Channel3_IRQn); if (dma_handle[dma_id_9] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_9]); } IRQ_EXIT(DMA2_Channel3_IRQn); }
void DMA2_Channel4_5_IRQHandler(void) { IRQ_ENTER(DMA2_Channel4_5_IRQn); if (dma_handle[dma_id_10] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_10]);} IRQ_EXIT(DMA2_Channel4_5_IRQn); }

static void dma_idle_handler(uint32_t tick);

// Resets the idle counter for the DMA controller associated with dma_id.
static void dma_tickle(dma_id_t dma_id) {
    dma_idle.counter[(dma_id < NSTREAMS_PER_CONTROLLER) ? 0 : 1] = 1;
    systick_enable_dispatch(SYSTICK_DISPATCH_DMA, dma_idle_handler);
}

// 打开DMA 时钟, 初始化 dma最后使用记录
static void dma_enable_clock(dma_id_t dma_id) {
    // We don't want dma_tick_handler() to turn off the clock right after we
    // enable it, so we need to mark the channel in use in an atomic fashion.
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t old_enable_mask = dma_enable_mask;
    dma_enable_mask |= (1 << dma_id);
    MICROPY_END_ATOMIC_SECTION(irq_state);

    if (dma_id < NSTREAMS_PER_CONTROLLER) {
        if (((old_enable_mask & DMA1_ENABLE_MASK) == 0) && !DMA1_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA1_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = 0; channel < NSTREAMS_PER_CONTROLLER; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    } else {
        if (((old_enable_mask & DMA2_ENABLE_MASK) == 0) && !DMA2_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA2_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA2) needs to be invalidated.
            for (int channel = NSTREAMS_PER_CONTROLLER; channel < NSTREAM; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    }
}

static void dma_disable_clock(dma_id_t dma_id) {
    // We just mark the clock as disabled here, but we don't actually disable it.
    // We wait for the timer to expire first, which means that back-to-back
    // transfers don't have to initialize as much.
    dma_tickle(dma_id);
    dma_enable_mask &= ~(1 << dma_id);
}

void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data) {
    // initialise parameters
    dma->Instance = dma_descr->instance;
    dma->Init = *dma_descr->init;
    dma->Init.Direction = dir;
    // half of __HAL_LINKDMA(data, xxx, *dma)
    // caller must implement other half by doing: data->xxx = dma
    dma->Parent = data;
}

void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data){
    // Some drivers allocate the DMA_HandleTypeDef from the stack
    // (i.e. dac, i2c, spi) and for those cases we need to clear the
    // structure so we don't get random values from the stack)
    memset(dma, 0, sizeof(*dma));

    if (dma_descr != NULL) {
        dma_id_t dma_id  = dma_descr->id;
		// 初始化dma句柄结构体
        dma_init_handle(dma, dma_descr, dir, data);

        // set global pointer for IRQ handler
        dma_handle[dma_id] = dma;

		// 开启对应通道时钟
        dma_enable_clock(dma_id);

        // 如果当前通道与前一个通道配置相同, 则跳过大量配置
        uint8_t sub_inst = DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance) | (dir == DMA_PERIPH_TO_MEMORY) << 7;
        if (dma_last_sub_instance[dma_id] != sub_inst) {
            dma_last_sub_instance[dma_id] = sub_inst;

            // reset and configure DMA peripheral
            // (dma->State is set to HAL_DMA_STATE_RESET by memset above)
            HAL_DMA_DeInit(dma);
            HAL_DMA_Init(dma);
            NVIC_SetPriority(IRQn_NONNEG(dma_irqn[dma_id]), IRQ_PRI_DMA);
        } else {
            // only necessary initialization
            dma->State = HAL_DMA_STATE_READY;

			#if defined(DMA2)
            // These variables are used to access the relevant 4 bits in ISR and IFCR
			if (dma_id < NSTREAMS_PER_CONTROLLER) {
				dma->DmaBaseAddress = DMA1;
				dma->ChannelIndex = dma_id * 4;
			} else {
				dma->DmaBaseAddress = DMA2;
    			dma->ChannelIndex = (dma_id - NSTREAMS_PER_CONTROLLER) * 4;
			}
			#else
				dma->DmaBaseAddress = DMA1;
				dma->ChannelIndex = dma_id * 4;
			#endif
        }

        HAL_NVIC_EnableIRQ(dma_irqn[dma_id]);
    }
}

void dma_deinit(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        dma_handle[dma_descr->id] = NULL;
        dma_disable_clock(dma_descr->id);
    }
}

void dma_invalidate_channel(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        dma_id_t dma_id = dma_descr->id;
        // Only compare the sub-instance, not the direction bit (MSB)
        if ((dma_last_sub_instance[dma_id] & 0x7f) == DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance) ) {
            dma_last_sub_instance[dma_id] = DMA_INVALID_CHANNEL;
        }
    }
}

// Called from the SysTick handler
// We use LSB of tick to select which controller to process
static void dma_idle_handler(uint32_t tick) {
    if (!DMA_IDLE_ENABLED() || !DMA_IDLE_TICK(tick)) {
        return;
    }

    static const uint32_t   controller_mask[] = {
        DMA1_ENABLE_MASK, DMA2_ENABLE_MASK
    };
    {
        int controller = (tick >> DMA_SYSTICK_LOG2) & 1;
        if (dma_idle.counter[controller] == 0) {
            return;
        }
        if (++dma_idle.counter[controller] > DMA_IDLE_TICK_MAX) {
            if ((dma_enable_mask & controller_mask[controller]) == 0) {
                // Nothing is active and we've reached our idle timeout,
                // Now we'll really disable the clock.
                dma_idle.counter[controller] = 0;
                if (controller == 0) {
                    __HAL_RCC_DMA1_CLK_DISABLE();
                } else {
                    __HAL_RCC_DMA2_CLK_DISABLE();
                }
            } else {
                // Something is still active, but the counter never got
                // reset, so we'll reset the counter here.
                dma_idle.counter[controller] = 1;
            }
        }
    }
}


void dma_nohal_init(const dma_descr_t *descr, uint32_t config) {
    DMA_Channel_TypeDef *dma = descr->instance;

    // Enable the DMA peripheral
    dma_enable_clock(descr->id);

    // Set main configuration register
    dma->CCR =
        descr->init->Priority       // PL
        | descr->init->MemInc       // MINC
        | descr->init->PeriphInc    // PINC
        | config                    // MSIZE | PSIZE | CIRC | DIR
        ;

    // Select channel that the DMA stream uses: BUG
	// AFIO_REMAP_ENABLE(descr->sub_instance);
}

void dma_nohal_deinit(const dma_descr_t *descr) {
    DMA_Channel_TypeDef *dma = descr->instance;
    dma->CCR &= ~DMA_CCR_EN;
    dma->CCR = 0;
    dma->CNDTR = 0;
    dma_deinit(descr);
}

void dma_nohal_start(const dma_descr_t *descr, uint32_t src_addr, uint32_t dst_addr, uint16_t len) {
    DMA_Channel_TypeDef *dma = descr->instance;
    dma->CNDTR = len;
    dma->CPAR = dst_addr;
    dma->CMAR = src_addr;
    dma->CCR |= DMA_CCR_EN;
}
