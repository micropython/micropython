#if MICROPY_PY_MACHINE_PDM_PCM

#define MICROPY_HW_MAX_PDM_PCM              1
#define DEFAULT_LEFT_GAIN                   0
#define DEFAULT_RIGHT_GAIN                  0

#define AUDIO_SYS_CLOCK_24_576_000_HZ      24576000u /* (Ideally 24.576 MHz) For sample rates: 8 KHz / 16 KHz / 48 KHz */
#define AUDIO_SYS_CLOCK_22_579_000_HZ      22579000u /* (Ideally 22.579 MHz) For sample rates: 22.05 KHz / 44.1 KHz */

#define SIZEOF_DMA_BUFFER                  (128)
#define SIZEOF_HALF_DMA_BUFFER             (SIZEOF_DMA_BUFFER / 2)
#define SIZEOF_DMA_BUFFER_IN_BYTES         (SIZEOF_DMA_BUFFER * sizeof(uint32_t))
#define SIZEOF_HALF_DMA_BUFFER_IN_BYTES    (SIZEOF_DMA_BUFFER_IN_BYTES / 2)
#define PDM_PCM_RX_FRAME_SIZE_IN_BYTES     (8)

#define NON_BLOCKING_RATE_MULTIPLIER       (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES  (SIZEOF_HALF_DMA_BUFFER * NON_BLOCKING_RATE_MULTIPLIER)


#define pdm_pcm_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}


// Constructor args
enum {
    ARG_clk,
    ARG_data,
    ARG_sample_rate,
    ARG_decimation_rate,
    ARG_bits,
    ARG_format,
    ARG_left_gain,
    ARG_right_gain,
};

typedef enum {
    BITS_16 = 16,
    BITS_18 = 18,
    BITS_20 = 20,
    BITS_24 = 24
} pdm_pcm_word_length_t;

typedef enum {
    MONO_LEFT   = CYHAL_PDM_PCM_MODE_LEFT,
    MONO_RIGHT  = CYHAL_PDM_PCM_MODE_RIGHT,
    STEREO      = CYHAL_PDM_PCM_MODE_STEREO
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
} io_mode_t;

#if MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

typedef struct _ring_buf_t {
    uint8_t *buffer;
    size_t head;
    size_t tail;
    size_t size;
} ring_buf_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    uint32_t index;
    bool copy_in_progress;
} non_blocking_descriptor_t;

void ringbuf_init(ring_buf_t *rbuf, size_t size);
bool ringbuf_push(ring_buf_t *rbuf, uint8_t data);
bool ringbuf_pop(ring_buf_t *rbuf, uint8_t *data);
size_t ringbuf_available_data(ring_buf_t *rbuf);
size_t ringbuf_available_space(ring_buf_t *rbuf);
#endif // MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

typedef struct _machine_pdm_pcm_obj_t {
    mp_obj_base_t base;
    uint8_t pdm_pcm_id;     // Private variable in this port. ID not associated to any port pin pdm-pcm group.
    cyhal_pdm_pcm_t pdm_pcm_obj;
    uint32_t clk;
    uint32_t data;
    io_mode_t io_mode;
    format_t format;
    uint8_t bits;
    uint32_t sample_rate;
    uint8_t decimation_rate;
    int16_t left_gain;
    int16_t right_gain;
    int32_t ibuf;   // Private variable
    mp_obj_t callback_for_non_blocking;
    uint32_t dma_active_buffer[SIZEOF_DMA_BUFFER];
    uint32_t dma_processing_buffer[SIZEOF_DMA_BUFFER];
    uint32_t *dma_active_buf_p;
    uint32_t *dma_processing_buf_p;
    ring_buf_t ring_buffer;
    non_blocking_descriptor_t non_blocking_descriptor; // For non-blocking mode
} machine_pdm_pcm_obj_t;

static void mp_machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, mp_arg_val_t *args);
static machine_pdm_pcm_obj_t *mp_machine_pdm_pcm_make_new_instance(mp_int_t pdm_pcm_id);
static void mp_machine_pdm_pcm_init(machine_pdm_pcm_obj_t *self);
static void mp_machine_pdm_pcm_deinit(machine_pdm_pcm_obj_t *self);
static void mp_machine_pdm_pcm_set_gain(machine_pdm_pcm_obj_t *self, int16_t left_gain, int16_t right_gain);
static void mp_machine_pdm_pcm_irq_update(machine_pdm_pcm_obj_t *self);

int8_t get_frame_mapping_index(int8_t bits, format_t format);

// Only 16 bit mono and stereo modes tested
static const int8_t pdm_pcm_frame_map[4][PDM_PCM_RX_FRAME_SIZE_IN_BYTES] = {
    { 0,  1, -1, -1, -1, -1, -1, -1 },   // Mono, 16-bits
    { 0,  1,  2, -1, -1, -1, -1, -1 },   // Mono, >16-bits
    { 0,  1, -1, -1,  2,  3, -1, -1 },   // Stereo, 16-bits
    { 0,  1,  2, -1,  3,  4,  5, -1 },   // Stereo, >16-bits
};


#endif // MICROPY_PY_MACHINE_PDM_PCM
