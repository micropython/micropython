#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "runtime0.h"
#include "builtin.h"

STATIC mp_obj_t op_getitem(mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_type_t *type = mp_obj_get_type(lhs_in);
    return type->binary_op(MP_BINARY_OP_SUBSCR, lhs_in, rhs_in);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_getitem_obj, op_getitem);

STATIC mp_obj_t op_contains(mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_type_t *type = mp_obj_get_type(lhs_in);
    return type->binary_op(MP_BINARY_OP_IN, lhs_in, rhs_in);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_contains_obj, op_contains);
