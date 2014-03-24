#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "nlr.h"

#include "pin.h"
#include "exti.h"

// Usage Model:
//
// There are a total of 22 interrupt lines. 16 of these can come from GPIO pins
// and the remaining 6 are from internal sources.
//
// For lines 0 thru 15, a given line can map to the corresponding line from an
// arbitrary port. So line 0 can map to Px0 where x is A, B, C, ... and
// line 1 can map to Px1 where x is A, B, C, ...
//
// def callback(line):
//     print("line =", line)
//
// # Note: Exti will automatically configure the gpio line as an input.
// exti = pyb.Exti(pin, pyb.Exti.MODE_IRQ_FALLING, pyb.PULLUP, callback)
//
// Now every time a falling edge is seen on the X1 pin, the callback will be
// called. Caution: mechanical pushbuttons have "bounce" and pushing or
// releasing a switch will often generate multiple edges.
// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
// explanation, along with various techniques for debouncing.
//
// Trying to register 2 callbacks onto the same pin will throw an exception.
//
// If pin is passed as an integer, then it is assumed to map to one of the
// internal interrupt sources, and must be in the range 16 thru 22.
//
// All other pin objects go through the pin mapper to come up with one of the
// gpio pins.
//
// exti = pyb.Exti(pin, mode, pull, callback)
//
// Valid modes are pyb.Exti.MODE_IRQ_RISING, pyb.Exti.MODE_IRQ_FALLING,
// pyb.Exti.MODE_IRQ_RISING_FALLING, pyb.Exti.MODE_EVT_RISING,
// pyb.Exti.MODE_EVT_FALLING, and pyb.Exti.MODE_EVT_RISING_FALLING.
//
// Only the MODE_IRQ_xxx modes have been tested. The MODE_EVENT_xxx modes have
// something to do with sleep mode and he WFE instruction.
//
// Valid pull values are pyb.PULL_UP, pyb.PULL_DOWN, pyb.PULL_NONE.
//
// exti.line() will return the line number that pin was mapped to.
// exti.disable() can be use to disable the interrupt associated with a given
//                exti object. This could be useful for debouncing.
// exti.enable()  enables a disabled interrupt
// exti.swint()   will allow the callback to be triggered from software.
//
// pyb.Exti.regs() will dump the values of the EXTI registers.
//
// There is also a C API, so that drivers which require EXTI interrupt lines
// can also use this code. See exti.h for the available functions and
// usrsw.h for an example of using this.

#define EXTI_OFFSET	(EXTI_BASE - PERIPH_BASE)

// Macro used to set/clear the bit corresponding to the line in the IMR/EMR
// register in an atomic fashion by using bitband addressing.
#define EXTI_MODE_BB(mode, line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + (mode)) * 32) + ((line) * 4)))

#define EXTI_Mode_Interrupt offsetof(EXTI_TypeDef, IMR)
#define EXTI_Mode_Event     offsetof(EXTI_TypeDef, EMR)

#define EXTI_SWIER_BB(line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + offsetof(EXTI_TypeDef, SWIER)) * 32) + ((line) * 4)))

typedef struct {
    mp_obj_base_t  base;
    mp_small_int_t line;
} exti_obj_t;

typedef struct {
  mp_obj_t callback_obj;
  void *param;
  uint32_t mode;
} exti_vector_t;

static exti_vector_t exti_vector[EXTI_NUM_VECTORS];

#if !defined(ETH)
#define ETH_WKUP_IRQn   62  // The 405 doesn't have ETH, but we want a value to put in our table
#endif

static const uint8_t nvic_irq_channel[EXTI_NUM_VECTORS] = {
    EXTI0_IRQn,     EXTI1_IRQn,     EXTI2_IRQn,     EXTI3_IRQn,     EXTI4_IRQn,
    EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,
    EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,
    EXTI15_10_IRQn, PVD_IRQn,       RTC_Alarm_IRQn, OTG_FS_WKUP_IRQn, ETH_WKUP_IRQn,
    OTG_HS_WKUP_IRQn, TAMP_STAMP_IRQn, RTC_WKUP_IRQn
};

// NOTE: param is for C callers. Python can use closure to get an object bound
//       with the function.
uint exti_register(mp_obj_t pin_obj, mp_obj_t mode_obj, mp_obj_t pull_obj, mp_obj_t callback_obj, void *param) {
    const pin_obj_t *pin = NULL;
    uint v_line;

    if (MP_OBJ_IS_INT(pin_obj)) {
        // If an integer is passed in, then use it to identify lines 16 thru 22
        // We expect lines 0 thru 15 to be passed in as a pin, so that we can
        // get both the port number and line number.
        v_line = mp_obj_get_int(pin_obj);
        if (v_line < 16) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "EXTI vector %d < 16, use a Pin object", v_line));
        }
        if (v_line >= EXTI_NUM_VECTORS) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "EXTI vector %d >= max of %d", v_line, EXTI_NUM_VECTORS));
        }
    } else {
        pin = pin_map_user_obj(pin_obj);
        v_line = pin->pin;
    }
    int mode = mp_obj_get_int(mode_obj);
    if (mode != GPIO_MODE_IT_RISING &&
        mode != GPIO_MODE_IT_FALLING &&
        mode != GPIO_MODE_IT_RISING_FALLING &&
        mode != GPIO_MODE_EVT_RISING &&
        mode != GPIO_MODE_EVT_FALLING &&
        mode != GPIO_MODE_EVT_RISING_FALLING) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid EXTI Mode: %d", mode));
    }
    int pull = mp_obj_get_int(pull_obj);
    if (pull != GPIO_NOPULL &&
        pull != GPIO_PULLUP &&
        pull != GPIO_PULLDOWN) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid EXTI Pull: %d", pull));
    }

    exti_vector_t *v = &exti_vector[v_line];
    if (v->callback_obj != mp_const_none && callback_obj != mp_const_none) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "EXTI vector %d is already in use", v_line));
    }

    // We need to update callback and param atomically, so we disable the line
    // before we update anything.

    exti_disable(v_line);

    v->callback_obj = callback_obj;
    v->param = param;
    v->mode = (mode & 0x00010000) ? // GPIO_MODE_IT == 0x00010000
        EXTI_Mode_Interrupt : EXTI_Mode_Event;

    if (v->callback_obj != mp_const_none) {

        GPIO_InitTypeDef exti;
        exti.Pin = pin->pin_mask;
        exti.Mode = mode;
        exti.Pull = pull;
        exti.Speed = GPIO_SPEED_FAST;
        HAL_GPIO_Init(pin->gpio, &exti);

        // Calling HAL_GPIO_Init does an implicit exti_enable

        /* Enable and set NVIC Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(nvic_irq_channel[v_line], 0x0F, 0x0F);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[v_line]);
    }
    return v_line;
}

void exti_enable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(exti_vector[line].mode, line) = 1;
}

void exti_disable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(EXTI_Mode_Interrupt, line) = 0;
    EXTI_MODE_BB(EXTI_Mode_Event, line) = 0;
}

void exti_swint(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    EXTI->SWIER = (1 << line);
}

static mp_obj_t exti_obj_line(mp_obj_t self_in) {
    exti_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->line);
}

static mp_obj_t exti_obj_enable(mp_obj_t self_in) {
    exti_obj_t *self = self_in;
    exti_enable(self->line);
    return mp_const_none;
}

static mp_obj_t exti_obj_disable(mp_obj_t self_in) {
    exti_obj_t *self = self_in;
    exti_disable(self->line);
    return mp_const_none;
}

static mp_obj_t exti_obj_swint(mp_obj_t self_in) {
    exti_obj_t *self = self_in;
    exti_swint(self->line);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(exti_obj_line_obj,    exti_obj_line);
static MP_DEFINE_CONST_FUN_OBJ_1(exti_obj_enable_obj,  exti_obj_enable);
static MP_DEFINE_CONST_FUN_OBJ_1(exti_obj_disable_obj, exti_obj_disable);
static MP_DEFINE_CONST_FUN_OBJ_1(exti_obj_swint_obj,   exti_obj_swint);

static const mp_method_t exti_methods[] = {
    { "line",  &exti_obj_line_obj },
    { "enable",  &exti_obj_enable_obj },
    { "disable",  &exti_obj_disable_obj },
    { "swint",  &exti_obj_swint_obj },
    { NULL, NULL },
};

static mp_obj_t exti_regs(void) {
    printf("EXTI_IMR   %08lx\n", EXTI->IMR);
    printf("EXTI_EMR   %08lx\n", EXTI->EMR);
    printf("EXTI_RTSR  %08lx\n", EXTI->RTSR);
    printf("EXTI_FTSR  %08lx\n", EXTI->FTSR);
    printf("EXTI_SWIER %08lx\n", EXTI->SWIER);
    printf("EXTI_PR    %08lx\n", EXTI->PR);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(exti_regs_obj, exti_regs);

typedef struct {
    const char *name;
    uint val;
} exti_const_t;

static const exti_const_t exti_const[] = {
    { "MODE_IRQ_RISING",            GPIO_MODE_IT_RISING },
    { "MODE_IRQ_FALLING",           GPIO_MODE_IT_FALLING },
    { "MODE_IRQ_RISING_FALLING",    GPIO_MODE_IT_RISING_FALLING },
    { "MODE_EVT_RISING",            GPIO_MODE_EVT_RISING },
    { "MODE_EVT_FALLING",           GPIO_MODE_EVT_FALLING },
    { "MODE_EVT_RISING_FALLING",    GPIO_MODE_EVT_RISING_FALLING },
};
#define EXTI_NUM_CONST  (sizeof(exti_const) / sizeof(exti_const[0]))

static void exti_load_attr(mp_obj_t self_in, qstr attr_qstr, mp_obj_t *dest) {
    (void)self_in;
    const char *attr = qstr_str(attr_qstr);

    if (strcmp(attr, "regs") == 0) {
        dest[0] = (mp_obj_t)&exti_regs_obj;
        return;
    }
    const exti_const_t *entry = &exti_const[0];
    for (; entry < &exti_const[EXTI_NUM_CONST]; entry++) {
        if (strcmp(attr, entry->name) == 0) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(entry->val);
            dest[1] = MP_OBJ_NULL;
            return;
        }
    }
}

// line_obj = pyb.Exti(pin, mode, trigger, callback)

static mp_obj_t exti_call(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // type_in == exti_obj_type

    rt_check_nargs(n_args, 4, 4, n_kw, 0);

    exti_obj_t *self = m_new_obj(exti_obj_t);
    self->base.type = type_in;
    mp_obj_t line_obj = args[0];
    mp_obj_t mode_obj = args[1];
    mp_obj_t trigger_obj = args[2];
    mp_obj_t callback_obj = args[3];
    self->line = exti_register(line_obj, mode_obj, trigger_obj, callback_obj, NULL);

    return self;
}

static void exti_meta_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)  self_in;
    print(env, "<Exti meta>");
}

static void exti_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    exti_obj_t *self = self_in;
    print(env, "<Exti line=%u>", self->line);
}

static const mp_obj_type_t exti_meta_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtiMeta,
    .print = exti_meta_obj_print,
    .call = exti_call,
    .load_attr = exti_load_attr,
};

const mp_obj_type_t exti_obj_type = {
    { &exti_meta_obj_type },
    .name = MP_QSTR_Exti,
    .print = exti_obj_print,
    .methods = exti_methods,
};

void exti_init(void) {
    for (exti_vector_t *v = exti_vector; v < &exti_vector[EXTI_NUM_VECTORS]; v++) {
        v->callback_obj = mp_const_none;
        v->param = NULL;
        v->mode = EXTI_Mode_Interrupt;
    }
}

void Handle_EXTI_Irq(uint32_t line) {
    if (__HAL_GPIO_EXTI_GET_FLAG(1 << line)) {
        __HAL_GPIO_EXTI_CLEAR_FLAG(1 << line);
        if (line < EXTI_NUM_VECTORS) {
            exti_vector_t *v = &exti_vector[line];
            if (v->callback_obj != mp_const_none) {
                rt_call_function_1(v->callback_obj, MP_OBJ_NEW_SMALL_INT(line));
            }
        }
    }
}
