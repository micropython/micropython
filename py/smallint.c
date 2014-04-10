#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"

bool mp_small_int_mul_overflow(machine_int_t x, machine_int_t y) {
    // Check for multiply overflow; see CERT INT32-C
    if (x > 0) { // x is positive
        if (y > 0) { // x and y are positive
            if (x > (MP_SMALL_INT_MAX / y)) {
                return true;
            }
        } else { // x positive, y nonpositive
            if (y < (MP_SMALL_INT_MIN / x)) {
                return true;
            }
        } // x positive, y nonpositive
    } else { // x is nonpositive
        if (y > 0) { // x is nonpositive, y is positive
            if (x < (MP_SMALL_INT_MIN / y)) {
                return true;
            }
        } else { // x and y are nonpositive
            if (x != 0 && y < (MP_SMALL_INT_MAX / x)) {
                return true;
            }
        } // End if x and y are nonpositive
    } // End if x is nonpositive
    return false;
}

machine_int_t mp_small_int_modulo(machine_int_t dividend, machine_int_t divisor) {
    machine_int_t lsign = (dividend >= 0) ? 1 :-1;
    machine_int_t rsign = (divisor >= 0) ? 1 :-1;
    dividend %= divisor;
    if (lsign != rsign) {
        dividend += divisor;
    }
  return dividend;
}


machine_int_t mp_small_int_floor_divide(machine_int_t num, machine_int_t denom) {
    machine_int_t lsign = num > 0 ? 1 : -1;
    machine_int_t rsign = denom > 0 ? 1 : -1;
    if (lsign == -1) {num *= -1;}
    if (rsign == -1) {denom *= -1;}
    if (lsign != rsign){
        return - ( num + denom - 1) / denom;
    } else {
        return num / denom;
    }
}
