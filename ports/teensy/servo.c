#include <stdio.h>
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "nlr.h"
#include "obj.h"
#include "servo.h"

#include "Arduino.h"

#define MAX_SERVOS  12
#define INVALID_SERVO   -1

#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000     // minumim time to refresh servos in microseconds

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE \
	| PDB_SC_CONT | PDB_SC_PRESCALER(2) | PDB_SC_MULT(0))
#define PDB_PRESCALE 4
#define usToTicks(us)    ((us) * (F_BUS / 1000) / PDB_PRESCALE / 1000)
#define ticksToUs(ticks) ((ticks) * PDB_PRESCALE * 1000 / (F_BUS / 1000))

static uint16_t servo_active_mask = 0;
static uint16_t servo_allocated_mask = 0;
static uint8_t servo_pin[MAX_SERVOS];
static uint16_t servo_ticks[MAX_SERVOS];

typedef struct _pyb_servo_obj_t {
    mp_obj_base_t base;
    uint servo_id;
    uint min_usecs;
    uint max_usecs;
} pyb_servo_obj_t;

#define clamp(v, min_val, max_val) ((v) < (min_val) ? (min_val) : (v) > (max_val) ? (max_val) : (v))

static float map_uint_to_float(uint x, uint in_min, uint in_max, float out_min, float out_max)
{
	return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + (float)out_min;
}

static uint map_float_to_uint(float x, float in_min, float in_max, uint out_min, uint out_max)
{
	return (int)((x - in_min) * (float)(out_max - out_min) / (in_max - in_min) + (float)out_min);
}

static mp_obj_t servo_obj_attach(mp_obj_t self_in, mp_obj_t pin_obj) {
    pyb_servo_obj_t *self = self_in;
    uint pin = mp_obj_get_int(pin_obj);
    if (pin > CORE_NUM_DIGITAL) {
        goto pin_error;
    }

    pinMode(pin, OUTPUT);
    servo_pin[self->servo_id] = pin;
    servo_active_mask |= (1 << self->servo_id);
    if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) {
        SIM_SCGC6 |= SIM_SCGC6_PDB; // TODO: use bitband for atomic bitset
        PDB0_MOD = 0xFFFF;
        PDB0_CNT = 0;
        PDB0_IDLY = 0;
        PDB0_SC = PDB_CONFIG;
        // TODO: maybe this should be a higher priority than most
        // other interrupts (init all to some default?)
        PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG;
    }
    NVIC_ENABLE_IRQ(IRQ_PDB);
    return mp_const_none;

pin_error:
    mp_raise_msg_varg(MP_QSTR_ValueError, "pin %d does not exist", pin);
}

static mp_obj_t servo_obj_detach(mp_obj_t self_in) {
    //pyb_servo_obj_t *self = self_in;
    return mp_const_none;
}

static mp_obj_t servo_obj_pin(mp_obj_t self_in) {
    pyb_servo_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(servo_pin[self->servo_id]);
}

static mp_obj_t servo_obj_min_usecs(int n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = args[0];
    if (n_args == 1) {
        // get min
        return MP_OBJ_NEW_SMALL_INT(self->min_usecs);
    }
    // Set min
    self->min_usecs = mp_obj_get_int(args[1]);
    return mp_const_none;
}

static mp_obj_t servo_obj_max_usecs(int n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = args[0];
    if (n_args == 1) {
        // get max
        return MP_OBJ_NEW_SMALL_INT(self->max_usecs);
    }
    // Set max
    self->max_usecs = mp_obj_get_int(args[1]);
    return mp_const_none;
}

static mp_obj_t servo_obj_angle(int n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        float angle = map_uint_to_float(servo_ticks[self->servo_id],
                                        usToTicks(self->min_usecs),
                                        usToTicks(self->max_usecs),
                                        0.0, 180.0);
        return mp_obj_new_float(angle);
    }
    // Set
    float angle = mp_obj_get_float(args[1]);
    if (angle < 0.0F) {
        angle = 0.0F;
    }
    if (angle > 180.0F) {
        angle = 180.0F;
    }
    servo_ticks[self->servo_id] = map_float_to_uint(angle,
                                                    0.0F, 180.0F,
                                                    usToTicks(self->min_usecs),
                                                    usToTicks(self->max_usecs));
    return mp_const_none;
}

static mp_obj_t servo_obj_usecs(int n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = args[0];
    uint usecs;
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(ticksToUs(servo_ticks[self->servo_id]));
    }
    // Set
    usecs = mp_obj_get_int(args[1]);

    if (self->min_usecs < self->max_usecs) {
        usecs = clamp(usecs, self->min_usecs, self->max_usecs);
    } else {
        usecs = clamp(usecs, self->max_usecs, self->min_usecs);
    }
    servo_ticks[self->servo_id] = usToTicks(usecs);
    return mp_const_none;
}

static mp_obj_t servo_obj_attached(mp_obj_t self_in) {
    pyb_servo_obj_t *self = self_in;
    uint attached = (servo_active_mask & (1 << self->servo_id)) != 0;
    return MP_OBJ_NEW_SMALL_INT(attached);
}

static void servo_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_servo_obj_t *self = self_in;
    (void)kind;
    print(env, "<Servo %lu>", self->servo_id);
}

static MP_DEFINE_CONST_FUN_OBJ_2(servo_obj_attach_obj, servo_obj_attach);
static MP_DEFINE_CONST_FUN_OBJ_1(servo_obj_detach_obj, servo_obj_detach);
static MP_DEFINE_CONST_FUN_OBJ_1(servo_obj_pin_obj, servo_obj_pin);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(servo_obj_min_usecs_obj, 1, 2, servo_obj_min_usecs);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(servo_obj_max_usecs_obj, 1, 2, servo_obj_max_usecs);
static MP_DEFINE_CONST_FUN_OBJ_1(servo_obj_attached_obj, servo_obj_attached);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(servo_obj_angle_obj, 1, 2, servo_obj_angle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(servo_obj_usecs_obj, 1, 2, servo_obj_usecs);

static const mp_method_t servo_methods[] = {
    { "attach", &servo_obj_attach_obj },
    { "detach", &servo_obj_detach_obj },
    { "pin", &servo_obj_pin_obj },
    { "min_usecs", &servo_obj_min_usecs_obj },
    { "max_usecs", &servo_obj_max_usecs_obj },
    { "attached", &servo_obj_attached_obj },
    { "angle", &servo_obj_angle_obj },
    { "usecs", &servo_obj_usecs_obj },
    { NULL, NULL },
};

/*
 * Notes:
 *
 * ISR needs to know pin #, ticks
 */

static const mp_obj_type_t servo_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Servo,
    .print = servo_obj_print,
    .methods = servo_methods,
};

/* servo = pyb.Servo(pin, [min_uecs, [max_usecs]]) */

mp_obj_t pyb_Servo(void) {
    uint16_t mask;
    pyb_servo_obj_t *self = m_new_obj(pyb_servo_obj_t);
    self->base.type = &servo_obj_type;
    self->min_usecs = MIN_PULSE_WIDTH;
    self->max_usecs = MAX_PULSE_WIDTH;

    /* Find an unallocated servo id */

    self->servo_id = 0;
    for (mask=1; mask < (1<<MAX_SERVOS); mask <<= 1) {
        if (!(servo_allocated_mask & mask)) {
            servo_allocated_mask |= mask;
            servo_active_mask &= ~mask;
            servo_ticks[self->servo_id] = usToTicks(DEFAULT_PULSE_WIDTH);
            return self;
        }
        self->servo_id++;
    }
    m_del_obj(pyb_servo_obj_t, self);
    mp_raise_ValueError("No available servo ids");
    return mp_const_none;
}

void pdb_isr(void)
{
	static int8_t channel = 0, channel_high = MAX_SERVOS;
	static uint32_t tick_accum = 0;
	uint32_t ticks;
	int32_t wait_ticks;

	// first, if any channel was left high from the previous
	// run, now is the time to shut it off
	if (servo_active_mask & (1 << channel_high)) {
		digitalWrite(servo_pin[channel_high], LOW);
		channel_high = MAX_SERVOS;
	}
	// search for the next channel to turn on
	while (channel < MAX_SERVOS) {
		if (servo_active_mask & (1 << channel)) {
			digitalWrite(servo_pin[channel], HIGH);
			channel_high = channel;
			ticks = servo_ticks[channel];
			tick_accum += ticks;
			PDB0_IDLY += ticks;
			PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
			channel++;
			return;
		}
		channel++;
	}
	// when all channels have output, wait for the
	// minimum refresh interval
	wait_ticks = usToTicks(REFRESH_INTERVAL) - tick_accum;
	if (wait_ticks < usToTicks(100)) wait_ticks = usToTicks(100);
	else if (wait_ticks > 60000) wait_ticks = 60000;
	tick_accum += wait_ticks;
	PDB0_IDLY += wait_ticks;
	PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
	// if this wait is enough to satisfy the refresh
	// interval, next time begin again at channel zero
	if (tick_accum >= usToTicks(REFRESH_INTERVAL)) {
		tick_accum = 0;
		channel = 0;
	}
}
