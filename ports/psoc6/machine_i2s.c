
#include <stdlib.h>

#include "py/runtime.h"
#include "py/mphal.h"

#define i2s_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/* Audio Subsystem Clock. Typical values depends on the desired sample rate:
 * 8KHz / 16 KHz / 32 KHz / 48 KHz    : 98.304  MHz
 * 22.05 KHz / 44.1 KHz               : 90.3168 MHz
 */

/* Clock Settings */
#define AUDIO_SYS_CLOCK_HZ    98304000u   /* in Hz (Ideally 98.304 MHz) */
#define AUDIO_SYS_CLOCK_HZ_1  90300000u   /* in Hz (Ideally 90.304 MHz) */

cyhal_clock_t audio_clock;

// DMA ping-pong buffer size was empirically determined.  It is a tradeoff between:
// 1. memory use (smaller buffer size desirable to reduce memory footprint)
// 2. interrupt frequency (larger buffer size desirable to reduce interrupt frequency)
#define SIZEOF_DMA_BUFFER (256)
#define SIZEOF_HALF_DMA_BUFFER (SIZEOF_DMA_BUFFER / 2)
#define SIZEOF_DMA_BUFFER_IN_BYTES (256 * sizeof(uint32_t))
#define SIZEOF_HALF_DMA_BUFFER_IN_BYTES (SIZEOF_DMA_BUFFER_IN_BYTES / 2)

// TODO: For non-blocking mode, to avoid underflow/overflow, sample data is written/read to/from the ring buffer at a rate faster
// than the DMA transfer rate
#define NON_BLOCKING_RATE_MULTIPLIER (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES (SIZEOF_HALF_DMA_BUFFER * NON_BLOCKING_RATE_MULTIPLIER)

typedef enum {
    RX,
    TX
} i2s_mode_t;

#define MICROPY_HW_MAX_I2S 2 // TODO: Move to per board.

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    uint8_t i2s_id;
    cyhal_i2s_t i2s_obj;
    mp_hal_pin_obj_t sck;   // TODO: These will be pin_phy objects
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    uint16_t mode;
    int8_t bits;
    uint8_t channel_resolution_bits; // TODO: better name?
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    io_mode_t io_mode;
    uint32_t dma_buffer[SIZEOF_DMA_BUFFER];
    uint32_t *dma_active_buf_p;
    uint32_t *dma_idle_buf_p;
    ring_buf_t ring_buffer;
    uint8_t *ring_buffer_storage;
    non_blocking_descriptor_t non_blocking_descriptor;
} machine_i2s_obj_t;

static const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    { 0,  1, -1, -1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    { 0,  1, -1, -1,  2,  3, -1, -1 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

static int8_t get_frame_mapping_index(int8_t bits, format_t format) {
    // TODO!
    if (format == MONO) {
        if (bits == 16) {
            return 0;
        } else { // 32 bits
            return 1;
        }
    } else { // STEREO
        if (bits == 16) {
            return 2;
        } else { // 32 bits
            return 3;
        }
    }
}

void i2s_audio_clock_init(uint32_t audio_clock_freq_hz) {
    cyhal_clock_t clock_pll;
    cy_rslt_t result;

    static bool clock_set = false;
    static uint32_t pll_source_clock_freq_hz = AUDIO_SYS_CLOCK_HZ;

    // TODO: Currently this is not working after initialization.
    // Once initializing, changing the frequency blocks execution of I2S constructor call.
    if (audio_clock_freq_hz != pll_source_clock_freq_hz) {
        clock_set = false;
        pll_source_clock_freq_hz = audio_clock_freq_hz;
    }

    if (!clock_set) {

        clock_set = true;

        /* Initialize, take ownership of PLL0/PLL */
        result = cyhal_clock_reserve(&clock_pll, &CYHAL_CLOCK_PLL[0]);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "Clock reserve fail");
        }

        /* Set the PLL0/PLL frequency to AUDIO_SYS_CLOCK_HZ based on AUDIO_IN_SAMPLE_FREQ */
        result = cyhal_clock_set_frequency(&clock_pll,  pll_source_clock_freq_hz, NULL);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "set_freq fail");
        }

        /* If the PLL0/PLL clock is not already enabled, enable it */
        if (!cyhal_clock_is_enabled(&clock_pll)) {
            result = cyhal_clock_set_enabled(&clock_pll, true, true);
            if (CY_RSLT_SUCCESS != result) {
                mp_printf(&mp_plat_print, "PLL enable fail");
            }
        }

        cyhal_clock_free(&clock_pll);

        /* Initialize, take ownership of CLK_HF1 */
        result = cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "Clock Hf1 fail");
        }

        /* Source the audio subsystem clock (CLK_HF1) from PLL0/PLL */
        result = cyhal_clock_set_source(&audio_clock, &clock_pll);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "sourcing HF1 fail");
        }

        /* Set the divider for audio subsystem clock (CLK_HF1) */
        result = cyhal_clock_set_divider(&audio_clock, 2);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "dividing hf1 fail");
        }

        /* If the audio subsystem clock (CLK_HF1) is not already enabled, enable it */
        if (!cyhal_clock_is_enabled(&audio_clock)) {
            result = cyhal_clock_set_enabled(&audio_clock, true, true);
            if (CY_RSLT_SUCCESS != result) {
                mp_printf(&mp_plat_print, "enable hf1 fail");
            }
        }

        cyhal_clock_free(&audio_clock);
    }

    cyhal_system_delay_ms(1);
}

static inline bool i2s_dma_is_tx_complete(cyhal_i2s_event_t event) {
    return 0u != (event & CYHAL_I2S_ASYNC_TX_COMPLETE);
}

static inline bool i2s_dma_is_rx_complete(cyhal_i2s_event_t event) {
    return 0u != (event & CYHAL_I2S_ASYNC_RX_COMPLETE);
}

uint8_t get_word_byte_size(machine_i2s_obj_t *self) {
    uint8_t res_bits = self->channel_resolution_bits;
    if (res_bits <= 8) {
        return 1;
    } else if (res_bits > 8 && res_bits <= 16) {
        return 2;
    } else if (res_bits > 16 && res_bits <= 24) {
        return 3;
    } else {
        return 4;
    }
}

static inline void i2s_dma_rx(machine_i2s_obj_t *self) {
    uint16_t dma_half_buff_word_size = SIZEOF_HALF_DMA_BUFFER_IN_BYTES / get_word_byte_size(self);
    cy_rslt_t result = cyhal_i2s_read_async(&self->i2s_obj, self->dma_active_buf_p, dma_half_buff_word_size);
    i2s_assert_raise_val("I2S DMA read failed with return code %lx !", result);
}

static inline void i2s_dma_tx(machine_i2s_obj_t *self) {
    uint16_t dma_half_buff_word_size = SIZEOF_HALF_DMA_BUFFER_IN_BYTES / get_word_byte_size(self);
    cy_rslt_t result = cyhal_i2s_write_async(&self->i2s_obj, self->dma_active_buf_p, dma_half_buff_word_size);
    i2s_assert_raise_val("I2S DMA write configure failed with return code %lx !", result);
}

// static inline void toggle_led() {
//     static uint32_t a = 0;
//     if (a++ % sizeof(uint64_t) == 0) {
//         cyhal_gpio_toggle(CYBSP_USER_LED);
//     }
// }

static void i2s_dma_from_dmabuf_to_ringbuf(machine_i2s_obj_t *self) {
    uint8_t dma_sample_size_in_bytes = (self->bits == 16? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint8_t *dma_buff_p = (uint8_t *)self->dma_idle_buf_p;
    uint32_t num_bytes_needed_from_ringbuf = SIZEOF_HALF_DMA_BUFFER_IN_BYTES * (I2S_RX_FRAME_SIZE_IN_BYTES / dma_sample_size_in_bytes);
    // toggle_led();

    // when space exists, copy samples into ring buffer
    if (ringbuf_available_space(&self->ring_buffer) >= num_bytes_needed_from_ringbuf) {
        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);
        uint32_t i = 0;
        while (i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {
            for (uint8_t j = 0; j < I2S_RX_FRAME_SIZE_IN_BYTES; j++) {
                int8_t r_to_a_mapping = i2s_frame_map[f_index][j];
                if (r_to_a_mapping != -1) {
                    ringbuf_push(&self->ring_buffer, dma_buff_p[i]);
                    i++;
                } else { // r_a_mapping == -1
                    ringbuf_push(&self->ring_buffer, -1);
                }
            }
        }
    }
}

static void i2s_dma_from_ringbuf_to_dmabuf(machine_i2s_obj_t *self) {
    uint8_t *dma_buffer_p = (uint8_t *)self->dma_idle_buf_p;
    memset(dma_buffer_p, 0, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
    if (ringbuf_available_data(&self->ring_buffer) >= SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {
        for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES; i++) {
            ringbuf_pop(&self->ring_buffer, &dma_buffer_p[i]);
        }
    }
}

static inline void i2s_dma_swap_active_dmabuf(machine_i2s_obj_t *self) {
    uint32_t *temp = self->dma_active_buf_p;
    self->dma_active_buf_p = self->dma_idle_buf_p;
    self->dma_idle_buf_p = temp;
}

static void i2s_dma_tx_event_process(machine_i2s_obj_t *self, cyhal_i2s_event_t event) {
    if (i2s_dma_is_tx_complete(event)) {

        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            copy_appbuf_to_ringbuf_non_blocking(self);
        }

        i2s_dma_swap_active_dmabuf(self);
        i2s_dma_tx(self);
        i2s_dma_from_ringbuf_to_dmabuf(self);
    }
}

static void i2s_dma_rx_event_process(machine_i2s_obj_t *self, cyhal_i2s_event_t event) {
    if (i2s_dma_is_rx_complete(event)) {

        i2s_dma_swap_active_dmabuf(self);
        i2s_dma_rx(self);
        i2s_dma_from_dmabuf_to_ringbuf(self);

        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            fill_appbuf_from_ringbuf_non_blocking(self);
        }
    }
}

static void i2s_dma_irq_handler(void *arg, cyhal_i2s_event_t event) {
    machine_i2s_obj_t *self = (machine_i2s_obj_t *)arg;

    void (*i2s_dma_event_process_func)(machine_i2s_obj_t *, cyhal_i2s_event_t);
    if (self->mode == TX) {
        i2s_dma_event_process_func = i2s_dma_tx_event_process;
    } else if (self->mode == RX) {
        i2s_dma_event_process_func = i2s_dma_rx_event_process;
    }

    i2s_dma_event_process_func(self, event);
}

static void i2s_init(machine_i2s_obj_t *self, cyhal_clock_t *clock) {
    cyhal_i2s_pins_t pins = { .sck = self->sck, .ws = self->ws, .data = self->sd, .mclk = NC };
    cyhal_i2s_config_t config =
    {
        .is_tx_slave = true,
        .is_rx_slave = false,
        .mclk_hz = 0,
        .channel_length = self->bits,
        .word_length = self->channel_resolution_bits,
        .sample_rate_hz = self->rate
    };

    cyhal_i2s_pins_t *tx_pins, *rx_pins;

    if (self->mode == TX) {
        tx_pins = &pins;
        rx_pins = NULL;
    } else { // Rx
        tx_pins = NULL;
        rx_pins = &pins;
    }

    cy_rslt_t result = cyhal_i2s_init(&self->i2s_obj, tx_pins, rx_pins, &config, clock);
    i2s_assert_raise_val("I2S initialisation failed with return code %lx !", result);
}

static inline void i2s_dma_irq_configure(machine_i2s_obj_t *self) {
    cyhal_i2s_register_callback(&self->i2s_obj, &i2s_dma_irq_handler, self);

    cyhal_i2s_event_t event;
    if (self->mode == TX) {
        event = CYHAL_I2S_ASYNC_TX_COMPLETE;
    } else {
        event = CYHAL_I2S_ASYNC_RX_COMPLETE;
    }

    cyhal_i2s_enable_event(&self->i2s_obj, event, CYHAL_ISR_PRIORITY_DEFAULT, true);

    cy_rslt_t result = cyhal_i2s_set_async_mode(&self->i2s_obj, CYHAL_ASYNC_DMA, CYHAL_DMA_PRIORITY_DEFAULT);
    i2s_assert_raise_val("I2S set DMA mode failed with return code %lx !", result);
}

static inline void i2s_dma_init_buff(machine_i2s_obj_t *self) {
    for (uint32_t i = 0; i < SIZEOF_DMA_BUFFER; i++) {
        self->dma_buffer[i] = 0;
    }

    self->dma_active_buf_p = self->dma_buffer;
    self->dma_idle_buf_p = &self->dma_buffer[SIZEOF_HALF_DMA_BUFFER];
}

static void i2s_dma_tx_init(machine_i2s_obj_t *self) {
    i2s_dma_tx(self);

    cy_rslt_t result = cyhal_i2s_start_tx(&self->i2s_obj);
    i2s_assert_raise_val("I2S tx start failed with return code %lx !", result);
}

static void i2s_dma_rx_init(machine_i2s_obj_t *self) {
    i2s_dma_rx(self);

    cy_rslt_t result = cyhal_i2s_start_rx(&self->i2s_obj);
    i2s_assert_raise_val("I2S rx start failed with return code %lx !", result);
}

static void i2s_dma_init(machine_i2s_obj_t *self) {
    i2s_dma_irq_configure(self);
    i2s_dma_init_buff(self);

    void (*i2s_dma_mode_init_func)(machine_i2s_obj_t *self);

    if (self->mode == TX) {
        i2s_dma_mode_init_func = i2s_dma_tx_init;
    } else { // Rx
        i2s_dma_mode_init_func = i2s_dma_rx_init;
    }

    i2s_dma_mode_init_func(self);
}

static void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {

    // // TODO: with all inputs validated make the objects of the i2s object.
    // // TODO: then make the i2s object
    // TODO: validate all arguments before initializing or creating any object

    // are Pins valid?
    // TODO: Check gpio allocation!
    // i2s_pin_alloc();
    // Allocate physical pins to prevent that there are used for/by other peripherals
    mp_hal_pin_obj_t sck = args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    mp_hal_pin_obj_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_ws].u_obj);
    mp_hal_pin_obj_t sd = args[ARG_sd].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sd].u_obj);

    // is Mode valid?
    // We are not distinguishing between master and slave roles, just RX and TX, which only support master in mpy API.
    i2s_mode_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != RX) &&
        (i2s_mode != TX)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // TODO: proper implementation between channel_bits and words_bits. Refactor proper naming.
    // is Bits valid?
    int8_t i2s_bits_resolution = args[ARG_bits].u_int;
    if (i2s_bits_resolution < 1 || i2s_bits_resolution > 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }

    int8_t i2s_bits;
    if (i2s_bits_resolution <= 16) {
        i2s_bits = 16;
    } else {
        i2s_bits = 32;
    }

    // int8_t i2s_bits = args[ARG_bits].u_int;
    // if ((i2s_bits != 16) &&
    //     (i2s_bits != 32)
    //     ) {
    //     mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    // }

    // is Format valid?
    format_t i2s_format = args[ARG_format].u_int;
    if ((i2s_format != MONO) &&
        (i2s_format != STEREO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    uint32_t audio_clock_freq_hz;
    uint32_t rate = args[ARG_rate].u_int;
    ;
    if (rate == 8000 ||
        rate == 16000 ||
        rate == 32000 ||
        rate == 48000) {
        audio_clock_freq_hz = AUDIO_SYS_CLOCK_HZ;
    } else if (rate == 22050 ||
               rate == 44100) {
        audio_clock_freq_hz = AUDIO_SYS_CLOCK_HZ_1;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("rate not supported"));
    }

    int32_t ring_buffer_len = args[ARG_ibuf].u_int;
    if (ring_buffer_len > 0) {
        self->ring_buffer_storage = m_new(uint8_t, ring_buffer_len);
        ringbuf_init(&self->ring_buffer, self->ring_buffer_storage, ring_buffer_len);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }

    self->sck = sck;
    self->ws = ws;
    self->sd = sd;
    self->mode = i2s_mode;
    self->bits = i2s_bits;
    self->channel_resolution_bits = i2s_bits_resolution;
    self->format = i2s_format;
    self->rate = rate;
    self->ibuf = ring_buffer_len;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->io_mode = BLOCKING;


    // cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
    //     CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    i2s_audio_clock_init(audio_clock_freq_hz);
    i2s_init(self, &audio_clock);
    i2s_dma_init(self);
}

static void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
    cyhal_i2s_free(&self->i2s_obj);
    // TODO: disable irq?
    // TODO: free pin_phy.c
    // TODO: stop i2s? Is that required?
}

static void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    (void)self;
}

static mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
    // TODO: We can skip the id, as this is not a hardware id for i2s,
    // but just an index in the amount of available instances.
    // The id from the main API will be kept as imposed by the extmod, but unused
    // in the psoc6 port.
    // If not instances are available, it should return with error, not deallocate
    // an existing instance.
    if (i2s_id >= MICROPY_HW_MAX_I2S) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj[i2s_id]) == NULL) {
        self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj[i2s_id]) = self;
        self->i2s_id = i2s_id;
    } else {
        self = MP_STATE_PORT(machine_i2s_obj[i2s_id]);
        machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    }

    return self;
}

MP_REGISTER_ROOT_POINTER(struct _machine_i2s_obj_t *machine_i2s_obj[MICROPY_HW_MAX_I2S]);
