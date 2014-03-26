#include <stdio.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"

STATIC const char *help_text =
"Welcome to Micro Python!\n"
"\n"
"For online help please visit http://micropython.org/help/.\n"
"\n"
"Specific commands for the board:\n"
"  pyb.info()      -- print some general information\n"
"  pyb.gc()        -- run the garbage collector\n"
"  pyb.repl_info(<val>) -- enable/disable printing of info after each command\n"
"  pyb.delay(<n>)  -- wait for n milliseconds\n"
"  pyb.udelay(<n>) -- wait for n microseconds\n"
"  pyb.switch()    -- return True/False if switch pressed or not\n"
"  pyb.Led(<n>)    -- create Led object for LED n (n=1,2,3,4)\n"
"                     Led methods: on(), off(), toggle(), intensity(<n>)\n"
"  pyb.Servo(<n>)  -- create Servo object for servo n (n=1,2,3,4)\n"
"                     Servo methods: angle(<x>)\n"
"  pyb.Accel()     -- create an Accelerometer object\n"
"                     Accelerometer methods: x(), y(), z(), tilt()\n"
"  pyb.rng()       -- get a 30-bit hardware random number\n"
"  pyb.gpio(<port>)        -- get port value (port='A4' for example)\n"
"  pyb.gpio(<port>, <val>) -- set port value, True or False, 1 or 0\n"
"  pyb.ADC(<port>) -- make an analog port object (port='C0' for example)\n"
"                     ADC methods: read()\n"
"\n"
"Control commands:\n"
"  CTRL-A       -- on a blank line, enter raw REPL mode\n"
"  CTRL-B       -- on a blank line, enter normal REPL mode\n"
"  CTRL-C       -- interrupt a running program\n"
"  CTRL-D       -- on a blank line, do a soft reset of the board\n"
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
            map = mp_obj_module_get_globals(args[0]);
        } else {
            mp_obj_type_t *type;
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_type)) {
                type = args[0];
            } else {
                type = mp_obj_get_type(args[0]);
            }
            if (type->locals_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(type->locals_dict, &dict_type)) {
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
