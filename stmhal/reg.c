#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "nlr.h"

#include "reg.h"

// This file adds support for accessing registers using:
//
// val = pyb.reg.TIM1.CR1
// pyb.reg.TIM1.CR1 = 0
//
// We use objects/types as follows:
//
//    Python            object          type
//    ----------------  --------------  ---------------
//    pyb.reg           reg_map_obj_t   reg_map_obj_type
//    pyb.reg.TIM1      reg_base_obj_t  reg_base_obj_type

typedef struct {
    const char *name;
    uint8_t     start;  // bit position
    uint8_t     size;   // in bits
} reg_field_t;

struct reg_s {
    const char *name;
    uint16_t    offset;     // in bytes
    uint8_t     size;       // in bits
    uint8_t     num_fields;
    const reg_field_t *field;
};

#define REG(base, fld, sz)  \
    { \
      .name   = # fld, \
      .size   = sz, \
      .offset = offsetof(base ## _TypeDef, fld), \
      .field  = reg_field_ ## base ## _ ## fld, \
      .num_fields = sizeof(reg_field_ ## base ## _ ## fld) / sizeof(reg_field_ ## base ## _ ## fld [0]), \
    }

#define REG2(base, fld, struct_fld, sz)  \
    { \
      .name   = # fld, \
      .size   = sz, \
      .offset = offsetof(base ## _TypeDef, struct_fld), \
      .field  = reg_field_ ## base ## _ ## fld, \
      .num_fields = sizeof(reg_field_ ## base ## _ ## fld) / sizeof(reg_field_ ## base ## _ ## fld [0]), \
    }

STATIC const reg_field_t reg_field_GPIO_MODER[] = {
};

STATIC const reg_field_t reg_field_GPIO_OTYPER[] = {
};

STATIC const reg_field_t reg_field_GPIO_OSPEEDR[] = {
};

STATIC const reg_field_t reg_field_GPIO_PUPDR[] = {
};

STATIC const reg_field_t reg_field_GPIO_IDR[] = {
};

STATIC const reg_field_t reg_field_GPIO_ODR[] = {
};

STATIC const reg_field_t reg_field_GPIO_BSRRH[] = {
};

STATIC const reg_field_t reg_field_GPIO_BSRRL[] = {
};

STATIC const reg_field_t reg_field_GPIO_LCKR[] = {
};

STATIC const reg_field_t reg_field_GPIO_AFRL[] = {
};

STATIC const reg_field_t reg_field_GPIO_AFRH[] = {
};

STATIC const reg_t reg_GPIO[] = {
    REG(GPIO, MODER,    32),
    REG(GPIO, OTYPER,   16),
    REG(GPIO, OSPEEDR,  32),
    REG(GPIO, PUPDR,    32),
    REG(GPIO, IDR,      16),
    REG(GPIO, ODR,      16),
    REG(GPIO, BSRRL,    16),
    REG(GPIO, BSRRH,    16),
    REG(GPIO, LCKR,     16),
    REG2(GPIO, AFRL, AFR[0], 32),
    REG2(GPIO, AFRH, AFR[1], 32),
};

STATIC const reg_field_t reg_field_TIM_CR1[] = {
    { "CEN",    0,  1 },  // Counter enable
    { "UDIS",   1,  1 },  // Update disable
    { "URS",    2,  1 },  // Update request source
    { "OPM",    3,  1 },  // One pulse mode                 (Timers 1-9, 12)
    { "DIR",    4,  1 },  // Direction                      (Timers 1-5, 8)
    { "CMS",    5,  2 },  // Center-aligned mode selection  (Timers 1-5, 8)
    { "ARPE",   7,  1 },  // Auto-reload preload enable
    { "CKD",    8,  2 },  // Clock division                 (Timers 1-5, 8-14)
};

STATIC const reg_field_t reg_field_TIM_CR2[] = {  // Timers 1-8
    { "CCPC",   0,  1 },  // Capture/compare preloaded control        (Timers 1-5, 8)
    { "CCUS",   2,  1 },  // Capture/compare control update selection (Timers 1-5, 8)
    { "CCDS",   3,  1 },  // Capture/compare DMA selection            (Timers 1-5, 8)
    { "MMS",    4,  3 },  // Master mode selection
    { "TI1S",   7,  1 },  // TI1 selection                            (Timers 1-5, 8)
    { "OIS1",   8,  1 },  // Output Idle state 1 (OC1 output)         (Timers 1-5, 8)
    { "OIS1N",  9,  1 },  // Output Idle state 1 (OC1N output)        (Timers 1-5, 8)
    { "OIS2",  10,  1 },  // Output Idle state 1 (OC2 output)         (Timers 1-5, 8)
    { "OIS2N", 11,  1 },  // Output Idle state 1 (OC2N output)        (Timers 1-5, 8)
    { "OIS3",  12,  1 },  // Output Idle state 1 (OC3  output)        (Timers 1-5, 8)
    { "OIS3N", 13,  1 },  // Output Idle state 1 (OC3N output)        (Timers 1-5, 8)
    { "OIS4",  14,  1 },  // Output Idle state 1 (OC4 output)         (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_SMCR[] = { // Timers 1-5, 8, 9, 12
    { "SMS",    0,  3 },  // Slave Mode selection
    { "TS",     4,  3 },  // Trigger selection
    { "MSM",    7,  1 },  // Master/Slave mode
    { "ETF",    8,  4 },  // External trigger filter      (Timers 1-5, 8)
    { "ETPS",  12,  2 },  // External trigger prescalar   (Timers 1-5, 8)
    { "ECE",   14,  1 },  // External clock enable        (Timers 1-5, 8)
    { "ETP",   15,  1 },  // External trigger polarity    (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_DIER[] = {
    { "UIE",    0,  1 },  // Update interrupt enable
    { "CC1IE",  1,  1 },  // Capture/Compare 1 interrupt enable   (Timers 1-5, 8, 9, 12)
    { "CC2IE",  2,  1 },  // Capture/Compare 2 interrupt enable   (Timers 1-5, 8, 9, 12)
    { "CC3IE",  3,  1 },  // Capture/Compare 3 interrupt enable   (Timers 1-5, 8)
    { "CC4IE",  4,  1 },  // Capture/Compare 4 interrupt enable   (Timers 1-5, 8)
    { "COMIE",  5,  1 },  // COM interrupt enable                 (Timers 1-5, 8)
    { "TIE",    6,  1 },  // Trigger interrupt enable             (Timers 1-5, 8, 9, 12)
    { "BIE",    7,  1 },  // Break interrupt enable               (Timers 1-5, 8)
    { "UDE",    8,  1 },  // Update DMA request enable            (Timers 1-8)
    { "CC1DE",  9,  1 },  // Capture/Compare 1 DMA request enable (Timers 1-5, 8)
    { "CC2DE", 10,  1 },  // Capture/Compare 2 DMA request enable (Timers 1-5, 8)
    { "CC3DE", 11,  1 },  // Capture/Compare 3 DMA request enable (Timers 1-5, 8)
    { "CC4DE", 12,  1 },  // Capture/Compare 4 DMA request enable (Timers 1-5, 8)
    { "COMDE", 13,  1 },  // COM DMA request enable               (Timers 1-5, 8)
    { "TDE",   14,  1 },  // Trigger DMA request enable           (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_SR[] = {
    { "UIF",    0,  1 },  // Update interrupt flag
    { "CC1IF",  1,  1 },  // Capture/Compare 1 interrupt flag     (Timers 1-5, 8-14)
    { "CC2IF",  2,  1 },  // Capture/Compare 2 interrupt flag     (Timers 1-5, 8, 9, 12)
    { "CC3IF",  3,  1 },  // Capture/Compare 3 interrupt flag     (Timers 1-5, 8)
    { "CC4IF",  4,  1 },  // Capture/Compare 4 interrupt flag     (Timers 1-5, 8)
    { "COMIF",  5,  1 },  // COM interrupt flag                   (Timers 1-5, 8)
    { "TIF",    6,  1 },  // Trigger interrupt flag               (Timers 1-5, 8, 9, 12)
    { "BIF",    7,  1 },  // Break interrupt flag                 (Timers 1-8)
    { "CC1OF",  9,  1 },  // Capture/Compare 1 overcapture flag   (Timers 1-5, 8-14)
    { "CC2OF", 10,  1 },  // Capture/Compare 2 overcapture flag   (Timers 1-5, 8, 9, 12)
    { "CC3OF", 11,  1 },  // Capture/Compare 3 overcapture flag   (Timers 1-5, 8)
    { "CC4OF", 12,  1 },  // Capture/Compare 4 overcapture flag   (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_EGR[] = {
    { "UG",   0,  1 },  // Update generation
    { "CC1G", 1,  1 },  // Capture/Compare 1 generation         (Timers 1-5, 8-14)
    { "CC2G", 2,  1 },  // Capture/Compare 2 generation         (Timers 1-5, 8, 9, 12)
    { "CC3G", 3,  1 },  // Capture/Compare 3 generation         (Timers 1-5, 8)
    { "CC4G", 4,  1 },  // Capture/Compare 4 generation         (Timers 1-5, 8)
    { "COMG", 5,  1 },  // Capture/Compare control update generation (Timers 1-5, 8)
    { "TG",   6,  1 },  // Trigger generation                   (Timers 1-5, 8, 9, 12)
    { "BG",   7,  1 },  // Break generation                     (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_CCMR1[] = {  // Timers 1-5, 8-14
    { "CC1S",   0,  2 },  // Capture/Compare 1 selection
    { "IC1PSC", 2,  2 },  // Input capture 1 prescaler
    { "IC1F",   4,  4 },  // Input capture 1 filter
    { "CC2S",   8,  2 },  // Capture/Compare 2 selection
    { "IC2PSC",10,  2 },  // Input capture 2 prescaler
    { "IC2F",  12,  4 },  // Input capture 2 filter
    { "OC1FE",  2,  1 },  // Output Compare 1 fast enable
    { "OC1PE",  3,  1 },  // Output Compare 1 preload enable
    { "OC1M",   4,  3 },  // Output Compare 1 mode
    { "OC1CE",  7,  1 },  // Output Compare 1 clear enable
    { "OC2FE", 10,  1 },  // Output Compare 2 fast enable
    { "OC2PE", 11,  1 },  // Output Compare 2 preload enable
    { "OC2M",  12,  3 },  // Output Compare 2 mode
    { "OC2CE", 15,  1 },  // Output Compare 2 clear enable
};

STATIC const reg_field_t reg_field_TIM_CCMR2[] = {  // (Timers 1-5, 8)
    { "CC3S",   0,  2 },  // Capture/Compare 3 selection
    { "IC3PSC", 2,  2 },  // Input capture 3 prescaler
    { "IC3F",   4,  4 },  // Input capture 3 filter
    { "CC4S",   8,  2 },  // Capture/Compare 4 selection
    { "IC4PSC",10,  2 },  // Input capture 4 prescaler
    { "IC4F",  12,  4 },  // Input capture 4 filter
    { "OC3FE",  2,  1 },  // Output Compare 3 fast enable
    { "OC3PE",  3,  1 },  // Output Compare 3 preload enable
    { "OC3M",   4,  3 },  // Output Compare 3 mode
    { "OC3CE",  7,  1 },  // Output Compare 3 clear enable
    { "OC4FE", 10,  1 },  // Output Compare 4 fast enable
    { "OC4PE", 11,  1 },  // Output Compare 4 preload enable
    { "OC4M",  12,  3 },  // Output Compare 4 mode
    { "OC4CE", 15,  1 },  // Output Compare 4 clear enable
};

STATIC const reg_field_t reg_field_TIM_CCER[] = { // Timers 1-5, 8-14
    { "CC1E",   0,  1 },  // Capture/Compare 1 output enable                  (Timers 1-5, 8-14)
    { "CC1P",   1,  1 },  // Capture/Compare 1 output polarity                (Timers 1-5, 8-14)
    { "CC1NE",  2,  1 },  // Capture/Compare 1 complementary output enable    (Timers 1-5, 8)
    { "CC1NP",  3,  1 },  // Capture/Compare 1 complementary output polarity  (Timers 1-5, 8-14)
    { "CC2E",   4,  1 },  // Capture/Compare 2 output enable                  (Timers 1-5, 8, 9,12)
    { "CC2P",   5,  1 },  // Capture/Compare 2 output polarity                (Timers 1-5, 8, 9, 12)
    { "CC2NE",  6,  1 },  // Capture/Compare 2 complementary output enable    (Timers 1-5, 8)
    { "CC2NP",  7,  1 },  // Capture/Compare 2 complementary output polarity  (Timers 1-5, 8, 9, 12)
    { "CC3E",   8,  1 },  // Capture/Compare 3 output enable                  (Timers 1-5, 8)
    { "CC3P",   9,  1 },  // Capture/Compare 3 output polarity                (Timers 1-5, 8)
    { "CC3NE", 10,  1 },  // Capture/Compare 3 complementary output enable    (Timers 1-5, 8)
    { "CC3NP", 11,  1 },  // Capture/Compare 3 complementary output polarity  (Timers 1-5, 8)
    { "CC4E",  12,  1 },  // Capture/Compare 4 output enable                  (Timers 1-5, 8)
    { "CC4P",  13,  1 },  // Capture/Compare 4 output polarity                (Timers 1-5, 8)
};

STATIC const reg_field_t reg_field_TIM_CNT[] = {
};

STATIC const reg_field_t reg_field_TIM_PSC[] = {
};

STATIC const reg_field_t reg_field_TIM_ARR[] = {
};

STATIC const reg_field_t reg_field_TIM_RCR[] = {  // (Timers 1 and 8)
    { "REP",    0,  8 },  // Repetition counter value
};

STATIC const reg_field_t reg_field_TIM_CCR1[] = { // Timers 1-5, 8-14
};

STATIC const reg_field_t reg_field_TIM_CCR2[] = { // Timers 1-5, 8-14
};

STATIC const reg_field_t reg_field_TIM_CCR3[] = { // Timers 1-5, 8
};

STATIC const reg_field_t reg_field_TIM_CCR4[] = { // Timers 1-5, 8
};

STATIC const reg_field_t reg_field_TIM_BDTR[] = { // Timers 1 and 8
  { "DTG",    0,  8 },  // Dead-time generator setup
  { "LOCK",   8,  2 },  // Lock configuration
  { "OSSI",  10,  1 },  // Off-state selection for Idle mode
  { "OSSR",  11,  1 },  // Off-state selection for Run mode
  { "BKE",   12,  1 },  // Break enable
  { "BKP",   13,  1 },  // Break polarity
  { "AOE",   14,  1 },  // Automatic output enable
  { "MOE",   15,  1 },  // Main output enable
};

STATIC const reg_field_t reg_field_TIM_DCR[] = {  // Timers 1-5, 8
  { "DBA",    0,  5 },  // DMA base address
  { "DBL",    8,  5 },  // DMA burst length
};

STATIC const reg_field_t reg_field_TIM_DMAR[] = { // Timers 1-5, 8
};

STATIC const reg_field_t reg_field_TIM_OR[] = { // Timers 2 and 5
    { "ITR1_RMP", 10, 2 },  // Internal trigger 1 remap   (Timer 2 only)
    { "TI4_RMP",   6, 2 },  // Timer Input 4 remap        (Timer 5 only)
};

STATIC const reg_t reg_TIM[] = {
    REG(TIM, CR1,   10),
    REG(TIM, CR2,   15),  // Timers 1-8
    REG(TIM, SMCR,  16),  // Timers 1-5, 8, 9, 12
    REG(TIM, DIER,  15),
    REG(TIM, SR,    13),
    REG(TIM, EGR,    8),
    REG(TIM, CCMR1, 16),  // Timers 1-5, 8-14
    REG(TIM, CCMR2, 16),  // Timers 1-5, 8
    REG(TIM, CCER,  14),  // Timers 1-5, 8-14
    REG(TIM, CNT,   16),
    REG(TIM, PSC,   16),
    REG(TIM, ARR,   16),
    REG(TIM, RCR,    8),  // Timers 1 and 8
    REG(TIM, CCR1,  16),  // Timers 1-5, 8-14
    REG(TIM, CCR2,  16),  // Timers 1-5, 8-14
    REG(TIM, CCR3,  16),  // Timers 1-5, 8
    REG(TIM, CCR4,  16),  // Timers 1-5, 8
    REG(TIM, BDTR,  16),  // Timers 1 and 8
    REG(TIM, DCR,   13),  // Timers 1-5, 8
    REG(TIM, DMAR,  16),  // Timers 1-5, 8
    REG(TIM, OR,    12),  // Timers 2 and 5
};

STATIC void reg_base_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    reg_base_obj_t *self = self_in;
    print(env, "<RegBase %s 0x%08x>", qstr_str(self->base_name), self->base_addr);
}

STATIC const reg_t *reg_lookup_field(const reg_base_obj_t *self, const char *attr, reg_field_t *out_field) {
    const reg_t *reg;
    const reg_t *end_reg = &self->reg[self->num_regs];
    size_t reg_name_len = 0;
    for (reg = self->reg; reg < end_reg; reg++) {
        reg_name_len = strlen(reg->name);
        if (strncmp(reg->name, attr, reg_name_len) == 0) {
            if (attr[reg_name_len] == '\0') {
                printf("Found reg: '%s'\n", reg->name);
                // We found a match, but only the register name was matched
                // so we create a field for the entire register.
                out_field->name = "";
                out_field->start = 0;
                out_field->size = reg->size;
                return reg;
            }
            if (attr[reg_name_len == '_']) {
                // We found a register name with a field specifier
                break;
            }
        }
    }
    if (reg >= end_reg) {
        // No match.
        return NULL;
    }
    printf("Found reg: '%s'\n", reg->name);

    // We know that the register name was followed by an underscore. See if
    // if matches a field name.

    const char *field_name = &attr[reg_name_len + 1];
    if (strcmp(field_name, "L") == 0) {
        // Return a field that matches the lower half of the register
        out_field->name = "L";
        out_field->start = 0;
        out_field->size = reg->size > 16 ? 16 : reg->size;
        return reg;
    }
    if ((strcmp(field_name, "H") == 0) && reg->size > 16) {
        // Return a field that matches the upper half of the register
        out_field->name = "H";
        out_field->start = 16;
        out_field->size = reg->size - 16;
        return reg;
    }

    const reg_field_t *reg_field;
    const reg_field_t *end_field = &reg->field[reg->num_fields];
    for (reg_field = reg->field; reg_field < end_field; reg_field++) {
        if (strcmp(field_name, reg_field->name) == 0) {
            // We found a matching field, return it
            *out_field = *reg_field;
            return reg;
        }
    }

    // See if the field name looks like a number between 0 and 31

    uint8_t bit_num = 0;
    if (*field_name >= '0' && *field_name <= '9') {
        bit_num += *field_name - '0';
        field_name++;
        if (*field_name >= '0' && *field_name <= '9') {
            bit_num *= 10;
            bit_num += *field_name - '0';
            field_name++;
        }
        if (*field_name != '\0' || bit_num >= reg->size) {
            return NULL;
        }

        out_field->name = "#";
        out_field->start = bit_num;
        out_field->size = 1;
        return reg;
    }
    return NULL;
}

STATIC void reg_base_obj_load_attr(mp_obj_t self_in, qstr attr_qstr, mp_obj_t *dest) {
    const reg_base_obj_t *self = self_in;
    const char *attr = qstr_str(attr_qstr);

    printf("RegBase: '%s' attr: '%s'\n", qstr_str(self->base_name), attr);

    reg_field_t reg_field;
    const reg_t *reg = reg_lookup_field(self, attr, &reg_field);
    if (reg == NULL) {
        return;
    }

    printf("Found a match: '%s' bit:%d size:%d\n", reg_field.name, reg_field.start, reg_field.size);

    dest[0] = mp_obj_new_int(self->base_addr + reg->offset);
    dest[1] = MP_OBJ_NULL;
}

STATIC const mp_obj_type_t reg_base_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_RegBase,
    .print = reg_base_obj_print,
    .load_attr = reg_base_obj_load_attr,
};

#define REG_BASE(periph, r) \
STATIC const reg_base_obj_t reg_base_ ## periph ## _obj = { \
    { &reg_base_obj_type }, \
    .base_name = MP_QSTR_ ## periph, \
    .base_addr = periph ## _BASE, \
    .reg = reg_ ## r, \
    .num_regs = sizeof(reg_ ## r) / sizeof(reg_ ## r [0]), \
}

REG_BASE(TIM1, TIM);
REG_BASE(TIM2, TIM);
REG_BASE(TIM3, TIM);
REG_BASE(TIM4, TIM);
REG_BASE(TIM5, TIM);
REG_BASE(TIM6, TIM);
REG_BASE(TIM7, TIM);
REG_BASE(TIM8, TIM);
REG_BASE(TIM9, TIM);
REG_BASE(TIM10, TIM);
REG_BASE(TIM11, TIM);
REG_BASE(TIM12, TIM);
REG_BASE(TIM13, TIM);
REG_BASE(TIM14, TIM);

REG_BASE(GPIOA, GPIO);
REG_BASE(GPIOB, GPIO);
REG_BASE(GPIOC, GPIO);
REG_BASE(GPIOD, GPIO);
REG_BASE(GPIOE, GPIO);
REG_BASE(GPIOF, GPIO);
REG_BASE(GPIOG, GPIO);
REG_BASE(GPIOH, GPIO);
REG_BASE(GPIOI, GPIO);

STATIC void reg_map_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    print(env, "<RegMap>");
}

STATIC const mp_map_elem_t reg_map_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_TIM1),  (mp_obj_t)&reg_base_TIM1_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TIM2),  (mp_obj_t)&reg_base_TIM2_obj },
};

STATIC MP_DEFINE_CONST_DICT(reg_map_locals_dict, reg_map_locals_dict_table);

STATIC const mp_obj_type_t reg_map_obj_type = {
  { &mp_type_type },
  .name = MP_QSTR_RegMap,
  .print = reg_map_obj_print,
  .locals_dict = (mp_obj_t)&reg_map_locals_dict,
};

const reg_map_obj_t reg_module = {
    { &reg_map_obj_type },
    .name = MP_QSTR_reg,
};
