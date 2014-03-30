#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

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
// # Configure the pin as a GPIO input.
// pin = pyb.Pin.board.X1
// pyb.gpio_in(pin, pyb.PULL_UP)
// exti = pyb.Exti(pin, pyb.Exti.MODE_IRQ, pyb.Exti.TRIGGER_RISING, callback, 37)
//
// Now every time a rising edge is seen on the X1 pin, the callback will be
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
// Valid modes are pyb.Exti.MODE_IRQ and pyb.Exti.MODE_EVENT (Only MODE_IRQ
// has been tested. MODE_EVENT has something to do with sleep mode and the
// WFE instruction).
//
// Valid edge triggers are pyb.Exti.TRIGGER_RISING, TRIGGER_FALLING, and TRIGGER_BOTH.
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
#define EXTI_MODE_BB(mode, line) (*(vu32 *)(PERIPH_BB_BASE + ((EXTI_OFFSET + (mode)) * 32) + ((line) * 4)))

// This macro will work with the EXTI_Trigger_Rising and EXTI_Trigger_Falling constants
// but not EXTI_Trigger_Rising_Falling.
#define EXTI_EDGE_BB(edge, line) (*(vu32 *)(PERIPH_BB_BASE + ((EXTI_OFFSET + (edge)) * 32) + ((line) * 4)))

#define EXTI_SWIER_BB(line) (*(vu32 *)(PERIPH_BB_BASE + ((EXTI_OFFSET + offsetof(EXTI_TypeDef, SWIER)) * 32) + ((line) * 4)))
#define EXTI_PR_BB(line)    (*(vu32 *)(PERIPH_BB_BASE + ((EXTI_OFFSET + offsetof(EXTI_TypeDef, PR)) * 32) + ((line) * 4)))

typedef struct {
    mp_obj_base_t  base;
    mp_small_int_t line;
} exti_obj_t;

typedef struct {
  mp_obj_t callback_obj;
  void *param;
  EXTIMode_TypeDef mode;
} exti_vector_t;

static exti_vector_t exti_vector[EXTI_NUM_VECTORS];

static const uint8_t nvic_irq_channel[EXTI_NUM_VECTORS] = {
    EXTI0_IRQn,     EXTI1_IRQn,     EXTI2_IRQn,     EXTI3_IRQn,     EXTI4_IRQn,
    EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,
    EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,
    EXTI15_10_IRQn, PVD_IRQn,       RTC_Alarm_IRQn, OTG_FS_WKUP_IRQn, ETH_WKUP_IRQn,
    OTG_HS_WKUP_IRQn, TAMP_STAMP_IRQn, RTC_WKUP_IRQn
};

// NOTE: param is for C callers. Python can use closure to get an object bound
//       with the function.
uint exti_register(mp_obj_t pin_obj, mp_obj_t mode_obj, mp_obj_t trigger_obj, mp_obj_t callback_obj, void *param) {
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
    if (!IS_EXTI_MODE(mode)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid EXTI Mode: %d", mode));
    }
    int trigger = mp_obj_get_int(trigger_obj);
    if (!IS_EXTI_TRIGGER(trigger)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid EXTI Trigger: %d", trigger));
    }

    exti_vector_t *v = &exti_vector[v_line];
    if (v->callback_obj != mp_const_none && callback_obj != mp_const_none) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "EXTI vector %d is already in use", v_line));
    }

    // We need to update callback and param atomically, so we disable the line
    // before we update anything.

    exti_disable(v_line);

    if (pin && callback_obj) {
        // Enable SYSCFG clock
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

        // For EXTI lines 0 thru 15, we need to configure which port controls
        // the line.
        SYSCFG_EXTILineConfig(pin->port, v_line);
    }
    v->callback_obj = callback_obj;
    v->param = param;
    v->mode = mode;

    if (v->callback_obj != mp_const_none) {
        // The EXTI_Init function isn't atomic. It uses |= and &=.
        // We use bit band operations to make it atomic.
        EXTI_EDGE_BB(EXTI_Trigger_Rising, v_line) =
            trigger == EXTI_Trigger_Rising || trigger == EXTI_Trigger_Rising_Falling;
        EXTI_EDGE_BB(EXTI_Trigger_Falling, v_line) =
            trigger == EXTI_Trigger_Falling || trigger == EXTI_Trigger_Rising_Falling;
        exti_enable(v_line);

        /* Enable and set NVIC Interrupt to the lowest priority */
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = nvic_irq_channel[v_line];
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
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
    EXTI_SWIER_BB(line) = 1;
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

STATIC const mp_map_elem_t exti_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_line), (mp_obj_t) &exti_obj_line_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable), (mp_obj_t) &exti_obj_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable), (mp_obj_t) &exti_obj_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_swint), (mp_obj_t) &exti_obj_swint_obj },
};

STATIC MP_DEFINE_CONST_DICT(exti_locals_dict, exti_locals_dict_table);

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
    { "MODE_IRQ",           EXTI_Mode_Interrupt },
    { "MODE_EVENT",         EXTI_Mode_Event },
    { "TRIGGER_RISING",     EXTI_Trigger_Rising },
    { "TRIGGER_FALLING",    EXTI_Trigger_Falling },
    { "TRIGGER_BOTH",       EXTI_Trigger_Rising_Falling },
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

    mp_check_nargs(n_args, 4, 4, n_kw, 0);

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
    .locals_dict = (mp_obj_t)&exti_locals_dict,
};

void exti_init(void) {
    for (exti_vector_t *v = exti_vector; v < &exti_vector[EXTI_NUM_VECTORS]; v++) {
        v->callback_obj = mp_const_none;
        v->param = NULL;
        v->mode = EXTI_Mode_Interrupt;
    }
}

static void Handle_EXTI_Irq(uint32_t line) {
    if (EXTI_PR_BB(line)) {
        EXTI_PR_BB(line) = 1; // Clears bit
        if (line < EXTI_NUM_VECTORS) {
            exti_vector_t *v = &exti_vector[line];
            if (v->callback_obj != mp_const_none) {
                mp_call_function_1(v->callback_obj, MP_OBJ_NEW_SMALL_INT(line));
            }
        }
    }
}

void EXTI0_IRQHandler(void) {
    Handle_EXTI_Irq(0);
}

void EXTI1_IRQHandler(void) {
    Handle_EXTI_Irq(1);
}

void EXTI2_IRQHandler(void) {
    Handle_EXTI_Irq(2);
}

void EXTI3_IRQHandler(void) {
    Handle_EXTI_Irq(3);
}

void EXTI4_IRQHandler(void) {
    Handle_EXTI_Irq(4);
}

void EXTI9_5_IRQHandler(void) {
    Handle_EXTI_Irq(5);
    Handle_EXTI_Irq(6);
    Handle_EXTI_Irq(7);
    Handle_EXTI_Irq(8);
    Handle_EXTI_Irq(9);
}

void EXTI15_10_IRQHandler(void) {
    Handle_EXTI_Irq(10);
    Handle_EXTI_Irq(11);
    Handle_EXTI_Irq(12);
    Handle_EXTI_Irq(13);
    Handle_EXTI_Irq(14);
    Handle_EXTI_Irq(15);

#if 0
    // for CC3000 support, needs to be re-written to use new EXTI code 
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        led_toggle(PYB_LED_G2);
        /* these are needed for CC3000 support
        extern void SpiIntGPIOHandler(void);
        extern uint32_t exti14_enabled;
        extern uint32_t exti14_missed;
        //printf("-> EXTI14 en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
        if (exti14_enabled) {
            exti14_missed = 0;
            SpiIntGPIOHandler(); // CC3000 interrupt
        } else {
            exti14_missed = 1;
        }
        */
        EXTI_ClearITPendingBit(EXTI_Line14);
        //printf("<- EXTI14 done\n");
    }
#endif
}

void PVD_IRQHandler(void) {
    Handle_EXTI_Irq(16);
}

void RTC_Alarm_IRQHandler(void) {
    Handle_EXTI_Irq(17);
}

#if 0 // dealt with in stm32fxxx_it.c
void OTG_FS_WKUP_IRQHandler(void) {
    Handle_EXTI_Irq(18);
}
#endif

void ETH_WKUP_IRQHandler(void)  {
    Handle_EXTI_Irq(19);
}

#if 0 // dealt with in stm32fxxx_it.c
void OTG_HS_WKUP_IRQHandler(void) {
    Handle_EXTI_Irq(20);
}
#endif

void TAMP_STAMP_IRQHandler(void) {
    Handle_EXTI_Irq(21);
}

void RTC_WKUP_IRQHandler(void) {
    Handle_EXTI_Irq(22);
}
