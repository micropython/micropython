#ifndef __TD02_RTC_H__
#define __TD02_RTC_H__

#include "py/obj.h"

extern RTC_HandleTypeDef RTCHandle;
extern const mp_obj_type_t tiger_rtc_type;

void rtc_init_start(bool force_init);
void rtc_init_finalise(void);

mp_obj_t pyb_rtc_wakeup(size_t n_args, const mp_obj_t *args);

#endif // __TD02_RTC_H__
