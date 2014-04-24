#include <stdio.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"

STATIC const char *help_text =
"Welcome to Micro Python!\n"
"\n"
"For online help please visit http://micropython.org/help/.\n"
"\n"
"Quick overview of commands for the board:\n"
"  pyb.info()    -- print some general information\n"
"  pyb.gc()      -- run the garbage collector\n"
"  pyb.delay(n)  -- wait for n milliseconds\n"
"  pyb.Switch()  -- create a switch object\n"
"                   Switch methods: (), callback(f)\n"
"  pyb.LED(n)    -- create an LED object for LED n (n=1,2,3,4)\n"
"                   LED methods: on(), off(), toggle(), intensity(<n>)\n"
"  pyb.Pin(pin)  -- get a pin, eg pyb.Pin('X1')\n"
"  pyb.Pin(pin, m, [p]) -- get a pin and configure it for IO mode m, pull mode p\n"
"                   Pin methods: init(..), value([v]), high(), low()\n"
"  pyb.ExtInt(pin, m, p, callback) -- create an external interrupt object\n"
"  pyb.ADC(pin)  -- make an analog object from a pin\n"
"                   ADC methods: read(), read_timed(buf, freq)\n"
"  pyb.DAC(port) -- make a DAC object\n"
"                   DAC methods: triangle(freq), write(n), write_timed(buf, freq)\n"
"  pyb.RTC()     -- make an RTC object; methods: datetime([val])\n"
"  pyb.rng()     -- get a 30-bit hardware random number\n"
"  pyb.Servo(n)  -- create Servo object for servo n (n=1,2,3,4)\n"
"                   Servo methods: calibration(..), angle([x, [t]]), speed([x, [t]])\n"
"  pyb.Accel()   -- create an Accelerometer object\n"
"                   Accelerometer methods: x(), y(), z(), tilt(), filtered_xyz()\n"
"\n"
"Pins are numbered X1-X12, X17-X22, Y1-Y12, or by their MCU name\n"
"Pin IO modes are: pyb.Pin.IN, pyb.Pin.OUT_PP, pyb.Pin.OUT_OD\n"
"Pin pull modes are: pyb.Pin.PULL_NONE, pyb.Pin.PULL_UP, pyb.Pin.PULL_DOWN\n"
"Additional serial bus objects: pyb.I2C(n), pyb.SPI(n), pyb.UART(n)\n"
"\n"
"Control commands:\n"
"  CTRL-A        -- on a blank line, enter raw REPL mode\n"
"  CTRL-B        -- on a blank line, enter normal REPL mode\n"
"  CTRL-C        -- interrupt a running program\n"
"  CTRL-D        -- on a blank line, do a soft reset of the board\n"
"\n"
"For further help on a specific object, type help(obj)\n"
;

STATIC void pyb_help_print_info_about_object(mp_obj_t name_o, mp_obj_t value) {
    printf("  ");
    mp_obj_print(name_o, PRINT_STR);
    printf(" -- ");
    mp_obj_print(value, PRINT_STR);
    printf("\n");
}

STATIC mp_obj_t pyb_help(uint n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // print a general help message
        printf("%s", help_text);

    } else {
        // try to print something sensible about the given object

        printf("object ");
        mp_obj_print(args[0], PRINT_STR);
        printf(" is of type %s\n", mp_obj_get_type_str(args[0]));

        mp_map_t *map = NULL;
        if (MP_OBJ_IS_TYPE(args[0], &mp_type_module)) {
            map = mp_obj_dict_get_map(mp_obj_module_get_globals(args[0]));
        } else {
            mp_obj_type_t *type;
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_type)) {
                type = args[0];
            } else {
                type = mp_obj_get_type(args[0]);
            }
            if (type->locals_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(type->locals_dict, &mp_type_dict)) {
                map = mp_obj_dict_get_map(type->locals_dict);
            }
        }
        if (map != NULL) {
            for (uint i = 0; i < map->alloc; i++) {
                if (map->table[i].key != MP_OBJ_NULL) {
                    pyb_help_print_info_about_object(map->table[i].key, map->table[i].value);
                }
            }
        }
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, pyb_help);
