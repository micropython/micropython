// machine_i2s_port.c - machine.I2S for GD32F4xx
//
// Included by extmod/machine_i2s.c via MICROPY_PY_MACHINE_I2S_INCLUDEFILE.
//
// GD32F4 implements I2S as an operating mode of the SPI peripherals (SPI1 ->
// I2S1, SPI2 -> I2S2).  Unlike the STM32 port there is no separate I2S clock
// tree, the audio clock is derived from the SPI kernel clock through I2SPSC.
//
// Sample flow uses the DMA controller in circular mode with the half-transfer
// and full-transfer interrupts, which gives gapless playback/recording.  The
// ISR refills one half of the ping-pong buffer while the other half is being
// shifted out, exactly like the STM32 port.

#include <stdlib.h>
#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "gd32f4xx.h"
#include "pin_defs.h"

// Ping-pong buffer: one half is filled by software while the other is drained
// by DMA.  Sized to keep the interrupt rate modest without wasting RAM.
#define SIZEOF_DMA_BUFFER_IN_BYTES      (256)
#define SIZEOF_HALF_DMA_BUFFER_IN_BYTES (SIZEOF_DMA_BUFFER_IN_BYTES / 2)

// Non-blocking mode moves samples between the user ring buffer and the DMA
// half-buffer faster than the audio rate to avoid underruns.
#define NON_BLOCKING_RATE_MULTIPLIER    (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES \
    (SIZEOF_HALF_DMA_BUFFER_IN_BYTES * NON_BLOCKING_RATE_MULTIPLIER)

typedef enum {
    TOP_HALF,
    BOTTOM_HALF
} ping_pong_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    uint8_t i2s_id;         // 1 or 2 (I2S1 = SPI1, I2S2 = SPI2)
    uint32_t spi;           // SPI1 / SPI2 base
    uint32_t rcu;           // RCU_SPI1 / RCU_SPI2
    mp_hal_pin_obj_t sck;   // CK
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    uint16_t mode;          // I2S_MODE_MASTERTX etc.
    int8_t bits;            // 16/24/32
    format_t format;        // MONO / STEREO
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    uint8_t dma_buffer[SIZEOF_DMA_BUFFER_IN_BYTES];
    ring_buf_t ring_buffer;
    uint8_t *ring_buffer_storage;
    non_blocking_descriptor_t non_blocking_descriptor;
    io_mode_t io_mode;
    // DMA bookkeeping
    uint32_t dma_periph;
    dma_channel_enum dma_channel_tx;
    dma_channel_enum dma_channel_rx;
    dma_subperipheral_enum dma_subperi_tx;
    dma_subperipheral_enum dma_subperi_rx;
    IRQn_Type irqn;
    bool dma_tx_active;
    bool dma_rx_active;
} machine_i2s_obj_t;

static machine_i2s_obj_t *i2s_obj_by_id[3] = { NULL, NULL, NULL };


// Frame map used by readinto(): transform the 32-bit stereo DMA layout into
// the user-selected (bits, format) layout.  -1 terminates the map.
static const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    { 0,  1, -1, -1, -1, -1, -1, -1 },  // MONO,   16-bit
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // MONO,   32-bit
    { 0,  1,  2,  3,  4,  5, -1, -1 },  // STEREO, 16-bit
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // STEREO, 32-bit
};

static int8_t get_frame_mapping_index(int8_t bits, format_t format) {
    if (format == MONO) {
        return (bits == 16) ? 0 : 1;
    }
    return (bits == 16) ? 2 : 3;
}

// ---- GD32 SPI/I2S instance tables ----
// I2S1 -> SPI1 (+I2S_ADD for full duplex), I2S2 -> SPI2.
static const struct {
    uint32_t spi;
    uint32_t rcu;
    IRQn_Type irqn;
    uint32_t dma_periph;
    dma_channel_enum tx_ch;
    dma_subperipheral_enum tx_subperi;
    dma_channel_enum rx_ch;
    dma_subperipheral_enum rx_subperi;
} i2s_hw[3] = {
    { 0, 0, (IRQn_Type)0, 0, (dma_channel_enum)0, (dma_subperipheral_enum)0,
      (dma_channel_enum)0, (dma_subperipheral_enum)0 },
    // I2S1: DMA1 CH4 (TX) / CH3 (RX), subperipheral 2 / 3
    { SPI1, RCU_SPI1, DMA1_Channel4_IRQn, DMA1, DMA_CH4,
      DMA_SUBPERI2, DMA_CH3, DMA_SUBPERI3 },
    // I2S2: DMA1 CH5 (TX) / CH0 (RX), subperipheral 2 / 3
    { SPI2, RCU_SPI2, DMA1_Channel5_IRQn, DMA1, DMA_CH5,
      DMA_SUBPERI2, DMA_CH0, DMA_SUBPERI3 },
};

// ---- helpers ----
static int8_t get_dma_bits(uint16_t mode, int8_t bits) {
    // The DMA moves whole 16-bit words for 16-bit audio, 32-bit otherwise.
    (void)mode;
    return (bits == 16) ? 16 : 32;
}

static uint32_t i2s_audio_sample_enum(int32_t rate) {
    switch (rate) {
        case 8000:  return I2S_AUDIOSAMPLE_8K;
        case 11025: return I2S_AUDIOSAMPLE_11K;
        case 16000: return I2S_AUDIOSAMPLE_16K;
        case 22050: return I2S_AUDIOSAMPLE_22K;
        case 32000: return I2S_AUDIOSAMPLE_32K;
        case 44100: return I2S_AUDIOSAMPLE_44K;
        case 48000: return I2S_AUDIOSAMPLE_48K;
        case 96000: return I2S_AUDIOSAMPLE_96K;
        case 192000: return I2S_AUDIOSAMPLE_192K;
        default:    return 0;
    }
}

static uint32_t i2s_frame_format_enum(int8_t bits) {
    switch (bits) {
        case 16: return I2S_FRAMEFORMAT_DT16B_CH16B;
        case 24: return I2S_FRAMEFORMAT_DT24B_CH32B;
        default: return I2S_FRAMEFORMAT_DT32B_CH32B;
    }
}

static void i2s_pin_config(mp_hal_pin_obj_t pin) {
    uint8_t port = PIN_PORT(pin.pin);
    uint8_t num = PIN_NUM(pin.pin);
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_af_set(pin_port_base(port), GPIO_AF_5, pin_hal_mask(num));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_AF, GPIO_PUPD_NONE, pin_hal_mask(num));
    gpio_output_options_set(pin_port_base(port), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin_hal_mask(num));
}

// ---- DMA setup ----
static void i2s_dma_config(machine_i2s_obj_t *self, bool tx) {
    uint32_t dma = self->dma_periph;
    dma_channel_enum ch = tx ? self->dma_channel_tx : self->dma_channel_rx;

    dma_deinit(dma, ch);
    dma_channel_subperipheral_select(dma, ch, tx ? self->dma_subperi_tx : self->dma_subperi_rx);

    dma_single_data_parameter_struct p;
    dma_single_data_para_struct_init(&p);
    p.periph_addr = (uint32_t)(&SPI_DATA(self->spi));
    p.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    p.memory0_addr = (uint32_t)self->dma_buffer;
    p.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    p.periph_memory_width = (get_dma_bits(self->mode, self->bits) == 16)
        ? DMA_PERIPH_WIDTH_16BIT : DMA_PERIPH_WIDTH_32BIT;
    p.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    p.direction = tx ? DMA_MEMORY_TO_PERIPH : DMA_PERIPH_TO_MEMORY;
    p.number = SIZEOF_DMA_BUFFER_IN_BYTES / (get_dma_bits(self->mode, self->bits) / 8);
    p.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(dma, ch, &p);

    // Half and full transfer interrupts drive the ping-pong refill.
    dma_interrupt_enable(dma, ch, DMA_INT_HTF | DMA_INT_FTF);

    if (tx) {
        spi_dma_enable(self->spi, SPI_DMA_TRANSMIT);
        self->dma_tx_active = true;
    } else {
        spi_dma_enable(self->spi, SPI_DMA_RECEIVE);
        self->dma_rx_active = true;
    }

    dma_channel_enable(dma, ch);
}

static void i2s_dma_stop(machine_i2s_obj_t *self, bool tx) {
    uint32_t dma = self->dma_periph;
    dma_channel_enum ch = tx ? self->dma_channel_tx : self->dma_channel_rx;
    dma_interrupt_disable(dma, ch, DMA_INT_HTF | DMA_INT_FTF);
    dma_channel_disable(dma, ch);
    if (tx) {
        spi_dma_disable(self->spi, SPI_DMA_TRANSMIT);
        self->dma_tx_active = false;
    } else {
        spi_dma_disable(self->spi, SPI_DMA_RECEIVE);
        self->dma_rx_active = false;
    }
}

// ---- ping-pong data movement ----
// Refill one half of the ping-pong buffer for transmission.
// In NON_BLOCKING mode the extmod ring-buffer helper moves application data
// into the ring buffer; we then drain that ring into the DMA half-buffer.
static void fill_dma_buffer_tx(machine_i2s_obj_t *self, ping_pong_t half) {
    uint8_t *base = self->dma_buffer + (half == TOP_HALF ? 0 : SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
    size_t len = SIZEOF_HALF_DMA_BUFFER_IN_BYTES;

    #if MICROPY_PY_MACHINE_I2S_RING_BUF
    if (self->io_mode == NON_BLOCKING && self->ring_buffer.buffer != NULL) {
        // Let extmod top up the ring buffer from the pending application buffer.
        copy_appbuf_to_ringbuf_non_blocking(self);
        size_t n = ringbuf_available_data(&self->ring_buffer);
        if (n > len) {
            n = len;
        }
        for (size_t i = 0; i < n; i++) {
            uint8_t b = 0;
            ringbuf_pop(&self->ring_buffer, &b);
            base[i] = b;
        }
        for (size_t i = n; i < len; i++) {
            base[i] = 0;   // silence on underrun
        }
        return;
    }
    #endif

    if (self->callback_for_non_blocking != mp_const_none) {
        mp_obj_t dest[2];
        mp_load_method(self->callback_for_non_blocking, MP_QSTR_, dest);
        dest[1] = mp_obj_new_bytearray_by_ref(len, base);
        mp_call_method_n_kw(1, 0, dest);
    } else {
        memset(base, 0, len);
    }
}

// Drain one half of the ping-pong buffer after reception.
static void empty_dma_buffer_rx(machine_i2s_obj_t *self, ping_pong_t half) {
    uint8_t *base = self->dma_buffer + (half == TOP_HALF ? 0 : SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
    size_t len = SIZEOF_HALF_DMA_BUFFER_IN_BYTES;

    #if MICROPY_PY_MACHINE_I2S_RING_BUF
    if (self->io_mode == NON_BLOCKING && self->ring_buffer.buffer != NULL) {
        for (size_t i = 0; i < len; i++) {
            if (!ringbuf_push(&self->ring_buffer, base[i])) {
                break;
            }
        }
        // Hand buffered samples to the application buffer.
        fill_appbuf_from_ringbuf_non_blocking(self);
        return;
    }
    #endif

    if (self->callback_for_non_blocking != mp_const_none) {
        mp_obj_t dest[2];
        mp_load_method(self->callback_for_non_blocking, MP_QSTR_, dest);
        dest[1] = mp_obj_new_bytearray_by_ref(len, base);
        mp_call_method_n_kw(1, 0, dest);
    }
}

// ---- I2S init ----
static bool machine_i2s_hw_init(machine_i2s_obj_t *self) {
    uint32_t sample_enum = i2s_audio_sample_enum(self->rate);
    if (sample_enum == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported sample rate"));
    }

    rcu_periph_clock_enable(self->rcu);
    rcu_periph_clock_enable(RCU_DMA1);

    i2s_pin_config(self->sck);
    i2s_pin_config(self->ws);
    i2s_pin_config(self->sd);

    spi_i2s_deinit(self->spi);

    // Master TX/RX half-duplex; Philips standard, low clock polarity.
    i2s_init(self->spi, self->mode, I2S_STD_PHILIPS, I2S_CKPL_LOW);
    i2s_psc_config(self->spi, sample_enum,
                   i2s_frame_format_enum(self->bits), I2S_MCKOUT_DISABLE);

    if (self->mode == I2S_MODE_MASTERTX) {
        i2s_dma_config(self, true);
    } else {
        i2s_dma_config(self, false);
    }

    i2s_enable(self->spi);
    return true;
}

// ---- extmod hooks ----
static void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {
    self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    self->ws = mp_hal_get_pin_obj(args[ARG_ws].u_obj);
    self->sd = mp_hal_get_pin_obj(args[ARG_sd].u_obj);
    self->mode = (uint16_t)args[ARG_mode].u_int;
    self->bits = (int8_t)args[ARG_bits].u_int;
    self->format = (format_t)args[ARG_format].u_int;
    self->rate = args[ARG_rate].u_int;
    self->ibuf = args[ARG_ibuf].u_int;

    self->io_mode = BLOCKING;
    self->callback_for_non_blocking = mp_const_none;

    #if MICROPY_PY_MACHINE_I2S_RING_BUF
    // Allocate the non-blocking ring buffer using the requested ibuf size.
    size_t ring_buffer_len = (self->ibuf > 0) ? (size_t)self->ibuf : SIZEOF_NON_BLOCKING_COPY_IN_BYTES;
    self->ring_buffer_storage = m_new(uint8_t, ring_buffer_len);
    ringbuf_init(&self->ring_buffer, self->ring_buffer_storage, ring_buffer_len);
    #else
    self->ring_buffer.buffer = NULL;
    #endif

    machine_i2s_hw_init(self);
}

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
    if (i2s_id < 1 || i2s_id > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid I2S id"));
    }
    machine_i2s_obj_t *self = m_new_obj(machine_i2s_obj_t);
    self->base.type = &machine_i2s_type;
    self->i2s_id = (uint8_t)i2s_id;
    self->spi = i2s_hw[i2s_id].spi;
    self->rcu = i2s_hw[i2s_id].rcu;
    self->dma_periph = i2s_hw[i2s_id].dma_periph;
    self->dma_channel_tx = i2s_hw[i2s_id].tx_ch;
    self->dma_channel_rx = i2s_hw[i2s_id].rx_ch;
    self->dma_subperi_tx = i2s_hw[i2s_id].tx_subperi;
    self->dma_subperi_rx = i2s_hw[i2s_id].rx_subperi;
    self->irqn = i2s_hw[i2s_id].irqn;
    self->dma_tx_active = false;
    self->dma_rx_active = false;
    i2s_obj_by_id[i2s_id] = self;
    return self;
}

static void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
    if (self->dma_tx_active) {
        i2s_dma_stop(self, true);
    }
    if (self->dma_rx_active) {
        i2s_dma_stop(self, false);
    }
    i2s_disable(self->spi);
    spi_i2s_deinit(self->spi);
    rcu_periph_clock_disable(self->rcu);
    if (self->ring_buffer_storage != NULL) {
        m_free(self->ring_buffer_storage);
        self->ring_buffer_storage = NULL;
        self->ring_buffer.buffer = NULL;
    }
    i2s_obj_by_id[self->i2s_id] = NULL;
    m_del_obj(machine_i2s_obj_t, self);
}

static void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    if (self->dma_tx_active || self->dma_rx_active) {
        NVIC_SetPriority(self->irqn, 2);
        NVIC_EnableIRQ(self->irqn);
    }
}

// ---- DMA interrupt handlers ----
static void i2s_dma_irq(machine_i2s_obj_t *self, bool tx, dma_channel_enum ch) {
    uint32_t dma = self->dma_periph;

    if (dma_interrupt_flag_get(dma, ch, DMA_INT_FLAG_HTF) != RESET) {
        dma_interrupt_flag_clear(dma, ch, DMA_INT_FLAG_HTF);
        if (tx) {
            fill_dma_buffer_tx(self, TOP_HALF);
        } else {
            empty_dma_buffer_rx(self, TOP_HALF);
        }
    }

    if (dma_interrupt_flag_get(dma, ch, DMA_INT_FLAG_FTF) != RESET) {
        dma_interrupt_flag_clear(dma, ch, DMA_INT_FLAG_FTF);
        if (tx) {
            fill_dma_buffer_tx(self, BOTTOM_HALF);
        } else {
            empty_dma_buffer_rx(self, BOTTOM_HALF);
        }
    }
}

// GD32F4 gives every DMA channel its own interrupt vector, so TX and RX need
// separate handlers.
void DMA1_Channel4_IRQHandler(void) {           // I2S1 TX
    machine_i2s_obj_t *self = i2s_obj_by_id[1];
    if (self != NULL && self->dma_tx_active) {
        i2s_dma_irq(self, true, self->dma_channel_tx);
    }
}

void DMA1_Channel3_IRQHandler(void) {           // I2S1 RX
    machine_i2s_obj_t *self = i2s_obj_by_id[1];
    if (self != NULL && self->dma_rx_active) {
        i2s_dma_irq(self, false, self->dma_channel_rx);
    }
}

void DMA1_Channel5_IRQHandler(void) {           // I2S2 TX
    machine_i2s_obj_t *self = i2s_obj_by_id[2];
    if (self != NULL && self->dma_tx_active) {
        i2s_dma_irq(self, true, self->dma_channel_tx);
    }
}

void DMA1_Channel0_IRQHandler(void) {           // I2S2 RX
    machine_i2s_obj_t *self = i2s_obj_by_id[2];
    if (self != NULL && self->dma_rx_active) {
        i2s_dma_irq(self, false, self->dma_channel_rx);
    }
}
