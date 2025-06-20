#include "py/runtime.h"
#include "py/mphal.h"


#if MICROPY_HW_HAS_XOR

#include "pin.h"
#include "td22-xor.h"
#include "timer.h"

enum 
{
    SET_STATE,
    RESET_STATE,
    XOR_STATE,
};


typedef struct _tiger_xor_obj_t 
{
    mp_obj_base_t base;
    mp_uint_t state;
	bool installed;
    const machine_pin_obj_t *pin;
} tiger_xor_obj_t;

tiger_xor_obj_t xor_obj = {{&tiger_xor_type}, SET_STATE, false, NULL};


static void ddr_xor(void)
{
	switch(xor_obj.state)
	{
		case SET_STATE:
			mp_hal_pin_high(xor_obj.pin);
			break;
		case RESET_STATE:
			mp_hal_pin_low(xor_obj.pin);
			break;
		case XOR_STATE:
			mp_hal_pin_toggle(xor_obj.pin);
			break;
		default:
			mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid mode (%d)"), xor_obj.state);
	}
}

mp_obj_t xor_obj_set(mp_obj_t self_in)
{
	tiger_xor_obj_t *self = MP_OBJ_TO_PTR(self_in);
	self->state = SET_STATE;
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(xor_obj_set_obj, xor_obj_set);

mp_obj_t xor_obj_reset(mp_obj_t self_in)
{
	tiger_xor_obj_t *self = MP_OBJ_TO_PTR(self_in);
	self->state = RESET_STATE;
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(xor_obj_reset_obj, xor_obj_reset);

mp_obj_t xor_obj_xor(mp_obj_t self_in)
{
	tiger_xor_obj_t *self = MP_OBJ_TO_PTR(self_in);
	self->state = XOR_STATE;
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(xor_obj_xor_obj, xor_obj_xor);

static mp_obj_t tiger_xor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args)
{

	static const mp_arg_t allowed_args[] = 
	{
		{ MP_QSTR_pin, MP_ARG_OBJ       							},
		{ MP_QSTR_xor, MP_ARG_BOOL, 	 {.u_bool = false}          },

	};

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
	
	mp_obj_t pin_obj = args[0].u_obj;
	
	if(mp_obj_is_str(pin_obj))
	{
		xor_obj.pin = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_obj);
	}
	else
	{
		xor_obj.pin = pin_find(pin_obj);
	}
	
	mp_hal_pin_output(xor_obj.pin);
	
	if(xor_obj.installed == false)
	{	
		if(addTimAInt(ddr_xor) == -1)
		{
			mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("Timer A not installed"));
		}
    }
	
	if(args[1].u_bool == true)
	{
		xor_obj.state = XOR_STATE;
	}
	else
	{
		xor_obj.state = RESET_STATE;
	}
	
    xor_obj.base.type = &tiger_xor_type;
	
	xor_obj.installed = true;
	
    return MP_OBJ_FROM_PTR(&xor_obj);
}

void xor_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) 
{
    tiger_xor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "XOR(%s)", self->state == XOR_STATE ? "XOR" : "SET & RESET");
}


static const mp_rom_map_elem_t xor_locals_dict_table[] = 
{
    { MP_ROM_QSTR(MP_QSTR_xor), MP_ROM_PTR(&xor_obj_xor_obj) },
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&xor_obj_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&xor_obj_reset_obj) },
};

MP_DEFINE_CONST_OBJ_TYPE
(
    tiger_xor_type,
    MP_QSTR_XOR,
    MP_TYPE_FLAG_NONE,
    make_new, tiger_xor_make_new,
    print, xor_obj_print,
    locals_dict, &xor_locals_dict_table
);
#endif