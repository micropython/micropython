#include "mpconfig.h"

machine_int_t python_modulo(machine_int_t dividend, machine_int_t divisor) {
    machine_int_t lsign = (dividend >= 0) ? 1 :-1;
    machine_int_t rsign = (divisor >= 0) ? 1 :-1;
    dividend %= divisor;
    if (lsign != rsign) {
        dividend += divisor;
    }
  return dividend;
}


machine_int_t python_floor_divide(machine_int_t num, machine_int_t denom) {
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
