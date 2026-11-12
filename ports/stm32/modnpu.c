/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

// The npu module, which runs models on the STM32N6's Neural-ART NPU.
//
// Models are compiled on a PC by ST Edge AI Core into a relocatable binary holding the
// model's code, NPU command streams and weights.  ST's runtime (ll_aton, built from
// STEDGEAI_DIR) installs the binary, linking it to the runtime and to its memory.
//
// Models are compiled for this memory layout (ports/stm32/npu/stm32n6.mpool), which is
// how ST's examples and OpenMV use the NPU:
// - Activations go in AXISRAM3-6, at fixed addresses.  main.c reserves this RAM, and
//   the NPU cache's RAM, for the NPU.  All models share it.
// - Activations that don't fit overflow into a relocatable pool in PSRAM.
// - The weights are a relocatable pool in the binary.  The NPU reads both of these
//   through its cache, which only works for external memory, so the binary is used in
//   place from memory mapped flash (a ROMFS file) or copied into PSRAM.
// - The model's code runs in place from the binary, with its data in the heap.
// The PSRAM for binaries and overflow activations is reserved at boot (see
// npu_psram_reserve()) and handed out like a stack.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/binary.h"
#include "py/objarray.h"
#include "py/stream.h"
#include "extmod/vfs.h"

#if MICROPY_PY_NPU

#include "ll_aton_runtime.h"
#include "ll_aton_reloc_network.h"
#include "ll_aton_version.h"
#include "npu_cache.h"

#include "irq.h"
#include "modnpu.h"
#if MICROPY_HW_XSPI_PSRAM
#include "xspi_psram.h"
#endif

#if !defined(STM32N6)
#error "npu module needs an STM32N6"
#endif

// A whole number of cache lines, so cache maintenance doesn't touch other memory.
#define NPU_ALIGN (32)

// AXISRAM3-6, for activations at fixed addresses.
#define NPU_RAM_START (SRAM3_AXI_BASE_S)
#define NPU_RAM_END (CACHEAXI_RAM_BASE_S)

// The NPU can only cache memory above this address (AI_RELOC_NPU_EXTERNAL_ADDR in the
// runtime's ll_aton_reloc_network.c).
#define NPU_EXTERNAL_ADDR (0x60000000)

// The most PSRAM blocks in use at once: up to two for each model.
#define NPU_PSRAM_MAX_BLOCKS (16)

// The runtime version a model must be compiled for, as in a model's rt_version, which
// the runtime checks when installing it.
#define NPU_RT_VERSION (LL_ATON_VERSION_MAJOR << 24 | LL_ATON_VERSION_MINOR << 16 | LL_ATON_VERSION_MICRO << 8)

// The start of a model file, as struct ai_reloc_bin_hdr in the runtime's
// ll_aton_reloc_network.c, with the fields this module needs.
typedef struct _npu_reloc_hdr_t {
    uint32_t magic;
    uint32_t flags;
    uint32_t data_start;
    uint32_t data_end;
    uint32_t data_data;
    uint32_t bss_start;
    uint32_t bss_end;
    uint32_t unused1[5];
    uint32_t params_offset;
    uint32_t unused2[12];
    uint32_t ctx;
} npu_reloc_hdr_t;

// The file offset of an address in a model file.
#define NPU_RELOC_OFFSET(addr) ((addr) & 0x0fffffff)
#define NPU_RELOC_ROUND_UP(size) (((size) + 7) & ~7)

typedef struct _npu_model_obj_t {
    mp_obj_base_t base;
    // A heap block for the model's data, which is used rounded up to NPU_ALIGN.
    uint8_t *exec_block;
    size_t exec_alloc;
    // PSRAM blocks for the binary (unless used in place) and the overflow activations.
    uint8_t *psram_image;
    uint8_t *ext_ram;
    size_t ext_ram_size;
    // The object holding the binary when it's used in place.
    mp_obj_t image_obj;
    uint8_t *image;
    size_t image_len;
    size_t num_inputs;
    size_t num_outputs;
    mp_obj_t inputs;
    mp_obj_t outputs;
    // Tuples of memoryviews of the input and output buffers, made once so input(),
    // output() and run() don't allocate.
    mp_obj_t input_views;
    mp_obj_t output_views;
    NN_Interface_TypeDef nn_interface;
    NN_Instance_TypeDef nn_instance;
} npu_model_obj_t;

typedef struct _npu_psram_block_t {
    uint8_t *addr;
    bool in_use;
} npu_psram_block_t;

static bool npu_initialised;
static bool npu_busy;

// The NPU cache is enabled once and stays enabled across soft resets, as enabling it
// again after npu_cache_disable() faults (OpenMV also leaves it enabled).
static bool npu_cache_initialised;

// The PSRAM reserved for the NPU (none if npu_psram_start is NULL), the blocks handed
// out from the bottom up, and the first free address above them.
static uint8_t *npu_psram_start;
static uint8_t *npu_psram_end;
static uint8_t *npu_psram_top;
static size_t npu_psram_num_blocks;
static npu_psram_block_t npu_psram_blocks[NPU_PSRAM_MAX_BLOCKS];

// Called by the runtime's npu_cache.c.
void npu_cache_enable_clocks_and_reset(void) {
    __HAL_RCC_CACHEAXI_CLK_ENABLE();
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
    __HAL_RCC_CACHEAXI_FORCE_RESET();
    __HAL_RCC_CACHEAXI_RELEASE_RESET();
}

#if MICROPY_HW_XSPI_PSRAM
size_t npu_psram_reserve(size_t psram_size) {
    size_t size = MIN(MICROPY_HW_NPU_PSRAM_SIZE, psram_size);
    if (size <= NPU_ALIGN) {
        return psram_size;
    }
    npu_psram_start = (uint8_t *)XSPI_PSRAM_BASE + psram_size - size;
    npu_psram_top = npu_psram_start;
    // Leave out the last 32 bytes: a cache line fill that includes the PSRAM's last
    // address can deadlock XSPI1 (ES0620 erratum 2.4.5).
    npu_psram_end = (uint8_t *)XSPI_PSRAM_BASE + psram_size - NPU_ALIGN;
    return psram_size - size;
}
#endif

static uint8_t *npu_psram_alloc(size_t len) {
    len = (len + NPU_ALIGN - 1) & ~(NPU_ALIGN - 1);
    if (npu_psram_num_blocks == NPU_PSRAM_MAX_BLOCKS || len > (size_t)(npu_psram_end - npu_psram_top)) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("not enough PSRAM for the NPU"));
    }
    npu_psram_block_t *block = &npu_psram_blocks[npu_psram_num_blocks++];
    block->addr = npu_psram_top;
    block->in_use = true;
    npu_psram_top += len;
    return block->addr;
}

// Free a block, and give back the space of any free blocks at the top.
static void npu_psram_free(uint8_t *addr) {
    for (size_t i = 0; i < npu_psram_num_blocks; ++i) {
        if (npu_psram_blocks[i].addr == addr) {
            npu_psram_blocks[i].in_use = false;
        }
    }
    while (npu_psram_num_blocks > 0 && !npu_psram_blocks[npu_psram_num_blocks - 1].in_use) {
        npu_psram_top = npu_psram_blocks[--npu_psram_num_blocks].addr;
    }
}

// The runtime provides NPU0_IRQHandler().
static void npu_init(void) {
    if (npu_initialised) {
        return;
    }
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    NVIC_SetPriority(NPU0_IRQn, IRQ_PRI_DMA);
    if (!npu_cache_initialised) {
        npu_cache_enable();
        npu_cache_initialised = true;
    }
    npu_initialised = true;
}

void npu_deinit(void) {
    npu_psram_top = npu_psram_start;
    npu_psram_num_blocks = 0;
    if (!npu_initialised) {
        return;
    }
    NVIC_DisableIRQ(NPU0_IRQn);
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    __HAL_RCC_NPU_CLK_DISABLE();
    npu_initialised = false;
    npu_busy = false;
}

static uint8_t *npu_alloc(size_t size, uint8_t **block, size_t *alloc) {
    *alloc = size + NPU_ALIGN;
    *block = m_new(uint8_t, *alloc);
    return (uint8_t *)(((uintptr_t)*block + NPU_ALIGN - 1) & ~(uintptr_t)(NPU_ALIGN - 1));
}

static void npu_model_free(npu_model_obj_t *self) {
    m_del(uint8_t, self->exec_block, self->exec_alloc);
    npu_psram_free(self->ext_ram);
    npu_psram_free(self->psram_image);
    self->exec_block = NULL;
    self->ext_ram = NULL;
    self->psram_image = NULL;
    self->image_obj = MP_OBJ_NULL;
    self->image = NULL;
}

static void npu_model_check(npu_model_obj_t *self) {
    if (self->image == NULL) {
        mp_raise_OSError(MP_EPERM);
    }
}

// Whether a binary can be used in place: memory mapped flash, eg a ROMFS file.
static bool npu_in_place(const mp_buffer_info_t *bufinfo) {
    return (uintptr_t)bufinfo->buf >= NPU_EXTERNAL_ADDR && ((uintptr_t)bufinfo->buf & (NPU_ALIGN - 1)) == 0;
}

// Get PSRAM for a copy of a binary that can't be used in place.
static uint8_t *npu_model_alloc_image(npu_model_obj_t *self, size_t len) {
    if (npu_psram_start == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("model must be in ROMFS on a board without PSRAM"));
    }
    self->psram_image = npu_psram_alloc(len);
    self->image_len = len;
    return self->psram_image;
}

// Open a model file, using it in place if it's memory mapped, otherwise reading it into
// PSRAM.
static void npu_model_read_file(npu_model_obj_t *self, mp_obj_t path) {
    mp_obj_t args[2] = { path, MP_OBJ_NEW_QSTR(MP_QSTR_rb) };
    mp_obj_t file = mp_vfs_open(MP_ARRAY_SIZE(args), args, (mp_map_t *)&mp_const_empty_map);
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(file, &bufinfo, MP_BUFFER_READ) && npu_in_place(&bufinfo)) {
        self->image_obj = file;
        self->image = bufinfo.buf;
        self->image_len = bufinfo.len;
        return;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        int errcode;
        mp_off_t len = mp_stream_seek(file, 0, MP_SEEK_END, &errcode);
        if (len == (mp_off_t)-1 || mp_stream_seek(file, 0, MP_SEEK_SET, &errcode) == (mp_off_t)-1) {
            mp_raise_OSError(errcode);
        }
        uint8_t *image = npu_model_alloc_image(self, len);
        if (mp_stream_rw(file, image, len, &errcode, MP_STREAM_RW_READ) != (mp_uint_t)len) {
            mp_raise_OSError(errcode ? errcode : MP_EIO);
        }
        self->image = image;
        nlr_pop();
    } else {
        mp_stream_close(file);
        nlr_jump(nlr.ret_val);
    }
    mp_stream_close(file);
}

static char npu_typecode(Buffer_DataType_TypeDef type) {
    switch (type) {
        case DataType_UINT8:
            return 'B';
        case DataType_INT8:
            return 'b';
        case DataType_UINT16:
            return 'H';
        case DataType_INT16:
            return 'h';
        case DataType_UINT32:
            return 'I';
        case DataType_INT32:
            return 'i';
        case DataType_FLOAT:
            return 'f';
        default:
            return 0;
    }
}

// Make a tuple of (shape, typecode, scale, zero_point) for each input or output, and
// return how many there are.
static size_t npu_buffers_info(const LL_Buffer_InfoTypeDef *bufs, mp_obj_t *info_out) {
    size_t n = 0;
    while (bufs[n].name != NULL) {
        ++n;
    }
    mp_obj_tuple_t *info = MP_OBJ_TO_PTR(mp_obj_new_tuple(n, NULL));
    for (size_t i = 0; i < n; ++i) {
        const LL_Buffer_InfoTypeDef *buf = &bufs[i];
        char typecode = npu_typecode(buf->type);
        if (typecode == 0) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unsupported data type %d"), buf->type);
        }
        mp_obj_tuple_t *shape = MP_OBJ_TO_PTR(mp_obj_new_tuple(buf->mem_ndims, NULL));
        for (size_t j = 0; j < buf->mem_ndims; ++j) {
            shape->items[j] = MP_OBJ_NEW_SMALL_INT(buf->mem_shape[j]);
        }
        bool quantised = typecode != 'f' && buf->scale != NULL && buf->scale[0] != 0.0f;
        mp_obj_t items[4] = {
            MP_OBJ_FROM_PTR(shape),
            mp_obj_new_str(&typecode, 1),
            mp_obj_new_float(quantised ? buf->scale[0] : 1.0f),
            MP_OBJ_NEW_SMALL_INT(quantised && buf->offset != NULL ? buf->offset[0] : 0),
        };
        info->items[i] = mp_obj_new_tuple(4, items);
    }
    *info_out = MP_OBJ_FROM_PTR(info);
    return n;
}

// Make a memoryview of an input or output buffer, with its typecode.
static mp_obj_t npu_buffer_view(const LL_Buffer_InfoTypeDef *buf, bool writable) {
    char typecode = npu_typecode(buf->type);
    size_t itemsize = mp_binary_get_size('@', typecode, NULL);
    return mp_obj_new_memoryview(typecode | (writable ? MP_OBJ_ARRAY_TYPECODE_FLAG_RW : 0),
        LL_Buffer_len(buf) / itemsize, LL_Buffer_addr_start(buf));
}

static MP_NORETURN void npu_raise_not_model(void) {
    mp_raise_ValueError(MP_ERROR_TEXT("not a relocatable NPU model"));
}

// Check a model file's header, and that the file holds the model's context.  Returns
// the context's offset in the file.
static size_t npu_reloc_ctx_offset(const npu_reloc_hdr_t *hdr, size_t file_len) {
    size_t offset = NPU_RELOC_OFFSET(hdr->data_data) + NPU_RELOC_OFFSET(hdr->ctx);
    if (hdr->magic != AI_RELOC_MAGIC || offset > file_len || sizeof(struct ai_reloc_rt_ctx) > file_len - offset) {
        npu_raise_not_model();
    }
    return offset;
}

// Check that a model file holds all of the model's weights, which are at its end.
static void npu_reloc_check_len(const npu_reloc_hdr_t *hdr, const struct ai_reloc_rt_ctx *ctx, size_t file_len) {
    size_t params_offset = NPU_RELOC_OFFSET(hdr->params_offset);
    if (params_offset > file_len || ctx->params_sz > file_len - params_offset) {
        npu_raise_not_model();
    }
}

static void npu_model_install(npu_model_obj_t *self) {
    // Check the file is complete, as the runtime doesn't.
    const npu_reloc_hdr_t *hdr = (const npu_reloc_hdr_t *)self->image;
    if (self->image_len < sizeof(*hdr)) {
        npu_raise_not_model();
    }
    size_t ctx_offset = npu_reloc_ctx_offset(hdr, self->image_len);
    npu_reloc_check_len(hdr, (const struct ai_reloc_rt_ctx *)(self->image + ctx_offset), self->image_len);

    uintptr_t file_ptr = (uintptr_t)self->image;
    ll_aton_reloc_info info;
    if (ll_aton_reloc_get_info(file_ptr, &info) != AI_RELOC_RT_ERR_NONE) {
        npu_raise_not_model();
    }
    if ((info.rt_version & 0xffffff00) != NPU_RT_VERSION) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("model is for NPU runtime %d.%d.%d, firmware has %d.%d.%d"),
            (int)(info.rt_version >> 24), (int)(info.rt_version >> 16 & 0xff), (int)(info.rt_version >> 8 & 0xff),
            LL_ATON_VERSION_MAJOR, LL_ATON_VERSION_MINOR, LL_ATON_VERSION_MICRO);
    }

    // The model must use the memory layout described at the top of this file: cacheable
    // relocatable pools for the weights and overflow activations, and fixed pools in
    // AXISRAM3-6 that need no initial data.
    ll_aton_reloc_mem_pool_desc *pool;
    for (int i = 0; (pool = ll_aton_reloc_get_mem_pool_desc(file_ptr, i)) != NULL; ++i) {
        uint32_t flags = pool->flags;
        if (pool->size == 0
            || (AI_RELOC_MPOOL_IS_RELOC(flags) && AI_RELOC_MPOOL_IS_CACHEABLE(flags))
            || (AI_RELOC_MPOOL_IS_RESET(flags) && pool->dst >= NPU_RAM_START && pool->dst + pool->size <= NPU_RAM_END)) {
            continue;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("model not compiled for the npu module"));
    }
    if (info.ext_ram_sz && npu_psram_start == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("model needs PSRAM"));
    }

    // The model's code runs from the binary, with its data in the exec RAM.
    uint8_t *exec_ram = npu_alloc(info.rt_ram_xip, &self->exec_block, &self->exec_alloc);
    if (info.ext_ram_sz) {
        self->ext_ram = npu_psram_alloc(info.ext_ram_sz);
        self->ext_ram_size = info.ext_ram_sz;
    }

    // The NPU reads the weights and command streams from memory, and the CPU runs the code
    // in the binary.  Also make sure no dirty lines get written back over the NPU's data,
    // and drop any lines the NPU cache holds from a model that used this PSRAM before.
    if (self->image_obj == MP_OBJ_NULL) {
        SCB_CleanDCache_by_Addr(self->image, self->image_len);
        SCB_InvalidateICache_by_Addr(self->image, self->image_len);
    }
    SCB_CleanInvalidateDCache_by_Addr(exec_ram, info.rt_ram_xip);
    if (self->ext_ram != NULL) {
        SCB_CleanInvalidateDCache_by_Addr(self->ext_ram, self->ext_ram_size);
    }
    npu_cache_invalidate();

    ll_aton_reloc_config config = {
        .exec_ram_addr = (uintptr_t)exec_ram,
        .exec_ram_size = info.rt_ram_xip,
        .ext_ram_addr = (uintptr_t)self->ext_ram,
        .ext_ram_size = self->ext_ram_size,
        .ext_param_addr = 0,
        .mode = AI_RELOC_RT_LOAD_MODE_XIP,
    };
    self->nn_interface.network_name = "network";
    self->nn_instance.network = &self->nn_interface;
    int ret = ll_aton_reloc_install(file_ptr, &config, &self->nn_instance);
    if (ret != AI_RELOC_RT_ERR_NONE) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("can't install model (%d)"), ret);
    }
    SCB_CleanDCache_by_Addr(exec_ram, info.rt_ram_xip);

    self->num_inputs = npu_buffers_info(ll_aton_reloc_get_input_buffers_info(&self->nn_instance, -1), &self->inputs);
    self->num_outputs = npu_buffers_info(ll_aton_reloc_get_output_buffers_info(&self->nn_instance, -1), &self->outputs);

    // Inputs are writable, outputs are read-only.
    mp_obj_tuple_t *views = MP_OBJ_TO_PTR(mp_obj_new_tuple(self->num_inputs, NULL));
    for (size_t i = 0; i < self->num_inputs; ++i) {
        views->items[i] = npu_buffer_view(ll_aton_reloc_get_input_buffers_info(&self->nn_instance, i), true);
    }
    self->input_views = MP_OBJ_FROM_PTR(views);
    views = MP_OBJ_TO_PTR(mp_obj_new_tuple(self->num_outputs, NULL));
    for (size_t i = 0; i < self->num_outputs; ++i) {
        views->items[i] = npu_buffer_view(ll_aton_reloc_get_output_buffers_info(&self->nn_instance, i), false);
    }
    self->output_views = MP_OBJ_FROM_PTR(views);
}

// npu.Model(source)
static mp_obj_t npu_model_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    npu_init();

    npu_model_obj_t *self = mp_obj_malloc_with_finaliser(npu_model_obj_t, type);
    memset(&self->exec_block, 0, sizeof(*self) - offsetof(npu_model_obj_t, exec_block));
    self->image_obj = MP_OBJ_NULL;

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (mp_obj_is_str(args[0])) {
            npu_model_read_file(self, args[0]);
        } else {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
            if (npu_in_place(&bufinfo)) {
                self->image_obj = args[0];
                self->image = bufinfo.buf;
                self->image_len = bufinfo.len;
            } else {
                self->image = npu_model_alloc_image(self, bufinfo.len);
                memcpy(self->image, bufinfo.buf, bufinfo.len);
            }
        }
        npu_model_install(self);
        nlr_pop();
    } else {
        npu_model_free(self);
        nlr_jump(nlr.ret_val);
    }
    return MP_OBJ_FROM_PTR(self);
}

static size_t npu_model_index(size_t n_args, const mp_obj_t *args, size_t num) {
    mp_int_t index = n_args > 1 ? mp_obj_get_int(args[1]) : 0;
    if (index < 0 || (size_t)index >= num) {
        mp_raise_type(&mp_type_IndexError);
    }
    return index;
}

// Model.input([index]) -> memoryview of an input buffer
static mp_obj_t npu_model_input(size_t n_args, const mp_obj_t *args) {
    npu_model_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    npu_model_check(self);
    size_t index = npu_model_index(n_args, args, self->num_inputs);
    return ((mp_obj_tuple_t *)MP_OBJ_TO_PTR(self->input_views))->items[index];
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(npu_model_input_obj, 1, 2, npu_model_input);

// Model.output([index]) -> read-only memoryview of an output buffer
static mp_obj_t npu_model_output(size_t n_args, const mp_obj_t *args) {
    npu_model_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    npu_model_check(self);
    size_t index = npu_model_index(n_args, args, self->num_outputs);
    return ((mp_obj_tuple_t *)MP_OBJ_TO_PTR(self->output_views))->items[index];
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(npu_model_output_obj, 1, 2, npu_model_output);

static bool npu_overlaps(const uint8_t *buf, size_t len, const LL_Buffer_InfoTypeDef *info) {
    return buf < LL_Buffer_addr_end(info) && buf + len > LL_Buffer_addr_start(info);
}

// Copy the inputs passed to run(), one buffer or a tuple or list of them, into the
// model's input buffers.
static void npu_model_set_inputs(npu_model_obj_t *self, mp_obj_t inputs_in) {
    size_t n = 1;
    mp_obj_t *inputs = &inputs_in;
    if (mp_obj_is_type(inputs_in, &mp_type_tuple) || mp_obj_is_type(inputs_in, &mp_type_list)) {
        mp_obj_get_array(inputs_in, &n, &inputs);
    }
    if (n != self->num_inputs) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("model has %d inputs"), (int)self->num_inputs);
    }

    // Check the lengths, and whether any input overlaps another's buffer, as a previous
    // output can.  Then all the inputs are copied through a temporary buffer.
    size_t total = 0;
    bool overlap = false;
    for (size_t i = 0; i < n; ++i) {
        mp_buffer_info_t src;
        mp_get_buffer_raise(inputs[i], &src, MP_BUFFER_READ);
        const LL_Buffer_InfoTypeDef *dst = ll_aton_reloc_get_input_buffers_info(&self->nn_instance, i);
        if (src.len != LL_Buffer_len(dst)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("input %d has %d bytes, model needs %d"),
                (int)i, (int)src.len, (int)LL_Buffer_len(dst));
        }
        total += src.len;
        for (size_t j = 0; j < n; ++j) {
            if (j != i && npu_overlaps(src.buf, src.len, ll_aton_reloc_get_input_buffers_info(&self->nn_instance, j))) {
                overlap = true;
            }
        }
    }

    uint8_t *tmp = overlap ? m_new(uint8_t, total) : NULL;
    for (int pass = overlap ? 0 : 1; pass < 2; ++pass) {
        size_t offset = 0;
        for (size_t i = 0; i < n; ++i) {
            mp_buffer_info_t src;
            mp_get_buffer_raise(inputs[i], &src, MP_BUFFER_READ);
            uint8_t *dst = LL_Buffer_addr_start(ll_aton_reloc_get_input_buffers_info(&self->nn_instance, i));
            if (pass == 0) {
                memcpy(tmp + offset, src.buf, src.len);
            } else {
                memmove(dst, overlap ? tmp + offset : src.buf, src.len);
            }
            offset += src.len;
        }
    }
    m_del(uint8_t, tmp, total);
}

// Model.run([inputs]) -> tuple of memoryviews of the outputs
static mp_obj_t npu_model_run(size_t n_args, const mp_obj_t *args) {
    npu_model_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    npu_model_check(self);
    if (npu_busy) {
        mp_raise_OSError(MP_EBUSY);
    }
    if (n_args > 1 && args[1] != mp_const_none) {
        npu_model_set_inputs(self, args[1]);
    }
    npu_busy = true;

    for (size_t i = 0; i < self->num_inputs; ++i) {
        const LL_Buffer_InfoTypeDef *buf = ll_aton_reloc_get_input_buffers_info(&self->nn_instance, i);
        SCB_CleanDCache_by_Addr(LL_Buffer_addr_start(buf), LL_Buffer_len(buf));
    }

    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&self->nn_instance);

    // Run the model's epochs, handling events between them so a long model doesn't hold
    // up USB or scheduled callbacks.  An exception is raised at the end, as raising it
    // now would leave the NPU running.
    mp_obj_t exc = MP_OBJ_NULL;
    LL_ATON_RT_RetValues_t ret;
    do {
        ret = LL_ATON_RT_RunEpochBlock(&self->nn_instance);
        if (ret == LL_ATON_RT_WFE) {
            LL_ATON_OSAL_WFE();
        } else if (ret == LL_ATON_RT_NO_WFE && exc == MP_OBJ_NULL) {
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_event_handle_nowait();
                nlr_pop();
            } else {
                exc = MP_OBJ_FROM_PTR(nlr.ret_val);
            }
        }
    } while (ret != LL_ATON_RT_DONE);

    LL_ATON_RT_DeInit_Network(&self->nn_instance);
    LL_ATON_RT_RuntimeDeInit();
    npu_busy = false;

    // Write the overflow activations, which may include outputs, out of the NPU cache,
    // so the CPU sees them and the next run starts with none of them cached.
    if (self->ext_ram != NULL) {
        npu_cache_clean_invalidate_range((uint32_t)self->ext_ram, (uint32_t)(self->ext_ram + self->ext_ram_size));
    }
    for (size_t i = 0; i < self->num_outputs; ++i) {
        const LL_Buffer_InfoTypeDef *buf = ll_aton_reloc_get_output_buffers_info(&self->nn_instance, i);
        SCB_InvalidateDCache_by_Addr(LL_Buffer_addr_start(buf), LL_Buffer_len(buf));
    }

    if (exc != MP_OBJ_NULL) {
        nlr_raise(exc);
    }
    return self->output_views;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(npu_model_run_obj, 1, 2, npu_model_run);

// Model.deinit()
static mp_obj_t npu_model_deinit(mp_obj_t self_in) {
    npu_model_free(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(npu_model_deinit_obj, npu_model_deinit);

static void npu_model_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    npu_model_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL && (attr == MP_QSTR_inputs || attr == MP_QSTR_outputs)) {
        npu_model_check(self);
        dest[0] = attr == MP_QSTR_inputs ? self->inputs : self->outputs;
    } else {
        // Continue lookup in locals_dict.
        dest[1] = MP_OBJ_SENTINEL;
    }
}

static const mp_rom_map_elem_t npu_model_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&npu_model_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_input), MP_ROM_PTR(&npu_model_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_output), MP_ROM_PTR(&npu_model_output_obj) },
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&npu_model_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&npu_model_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(npu_model_locals_dict, npu_model_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    npu_model_type,
    MP_QSTR_Model,
    MP_TYPE_FLAG_NONE,
    make_new, npu_model_make_new,
    attr, npu_model_attr,
    locals_dict, &npu_model_locals_dict
    );

static mp_obj_t npu_version_tuple(uint32_t version) {
    mp_obj_t items[3] = {
        MP_OBJ_NEW_SMALL_INT(version >> 24),
        MP_OBJ_NEW_SMALL_INT(version >> 16 & 0xff),
        MP_OBJ_NEW_SMALL_INT(version >> 8 & 0xff),
    };
    return mp_obj_new_tuple(3, items);
}

// Read part of a model file, or of a buffer holding one.
static void npu_model_info_read(mp_obj_t file, const mp_buffer_info_t *bufinfo, size_t offset, void *dest, size_t len) {
    if (file == MP_OBJ_NULL) {
        if (offset > bufinfo->len || len > bufinfo->len - offset) {
            npu_raise_not_model();
        }
        memcpy(dest, (uint8_t *)bufinfo->buf + offset, len);
        return;
    }
    int errcode;
    if (mp_stream_seek(file, offset, MP_SEEK_SET, &errcode) == (mp_off_t)-1) {
        mp_raise_OSError(errcode);
    }
    mp_uint_t n = mp_stream_rw(file, dest, len, &errcode, MP_STREAM_RW_READ);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    if (n != len) {
        npu_raise_not_model();
    }
}

// npu.model_info(source) -> (version, file_size, acts_size, ext_ram_size, heap_size)
// Reads just the model file's header and context, without loading the model.
static mp_obj_t npu_model_info(mp_obj_t source) {
    mp_obj_t file = MP_OBJ_NULL;
    mp_buffer_info_t bufinfo = { 0 };
    if (mp_obj_is_str(source)) {
        mp_obj_t args[2] = { source, MP_OBJ_NEW_QSTR(MP_QSTR_rb) };
        file = mp_vfs_open(MP_ARRAY_SIZE(args), args, (mp_map_t *)&mp_const_empty_map);
    } else {
        mp_get_buffer_raise(source, &bufinfo, MP_BUFFER_READ);
    }

    npu_reloc_hdr_t hdr;
    uint32_t ctx_buf[sizeof(struct ai_reloc_rt_ctx) / sizeof(uint32_t)];
    const struct ai_reloc_rt_ctx *ctx = (const struct ai_reloc_rt_ctx *)ctx_buf;
    size_t file_size = 0;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (file == MP_OBJ_NULL) {
            file_size = bufinfo.len;
        } else {
            int errcode;
            mp_off_t len = mp_stream_seek(file, 0, MP_SEEK_END, &errcode);
            if (len == (mp_off_t)-1) {
                mp_raise_OSError(errcode);
            }
            file_size = len;
        }
        npu_model_info_read(file, &bufinfo, 0, &hdr, sizeof(hdr));
        size_t ctx_offset = npu_reloc_ctx_offset(&hdr, file_size);
        npu_model_info_read(file, &bufinfo, ctx_offset, ctx_buf, sizeof(ctx_buf));
        npu_reloc_check_len(&hdr, ctx, file_size);
        nlr_pop();
    } else {
        if (file != MP_OBJ_NULL) {
            mp_stream_close(file);
        }
        nlr_jump(nlr.ret_val);
    }
    if (file != MP_OBJ_NULL) {
        mp_stream_close(file);
    }

    mp_obj_t items[5] = {
        npu_version_tuple(ctx->rt_version),
        mp_obj_new_int_from_uint(file_size),
        mp_obj_new_int_from_uint(ctx->acts_sz),
        mp_obj_new_int_from_uint(NPU_RELOC_ROUND_UP(ctx->ext_ram_sz)),
        mp_obj_new_int_from_uint(NPU_RELOC_ROUND_UP(NPU_RELOC_OFFSET(hdr.bss_end))),
    };
    return mp_obj_new_tuple(5, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(npu_model_info_obj, npu_model_info);

// npu.version: the runtime version that models must be compiled for.
static const mp_rom_obj_tuple_t npu_version_obj = {
    {&mp_type_tuple}, 3, {MP_ROM_INT(LL_ATON_VERSION_MAJOR), MP_ROM_INT(LL_ATON_VERSION_MINOR), MP_ROM_INT(LL_ATON_VERSION_MICRO)}
};

static const mp_rom_map_elem_t npu_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_npu) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&npu_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_model_info), MP_ROM_PTR(&npu_model_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_Model), MP_ROM_PTR(&npu_model_type) },
};
static MP_DEFINE_CONST_DICT(npu_module_globals, npu_module_globals_table);

const mp_obj_module_t mp_module_npu = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&npu_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_npu, mp_module_npu);

#endif // MICROPY_PY_NPU
