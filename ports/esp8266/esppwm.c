/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: pwm.c
 *
 * Description: pwm driver
 *
 * Modification history:
 *     2014/5/1, v1.0 create this file.
 *     2016/3/2: Modifications by dpgeorge to suit MicroPython
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "etshal.h"
#include "os_type.h"
#include "gpio.h"

#include "esppwm.h"

#include "py/mpprint.h"
#define PWM_DBG(...)
// #define PWM_DBG(...) mp_printf(&mp_plat_print, __VA_ARGS__)

#define ICACHE_RAM_ATTR // __attribute__((section(".text")))

#define PWM_CHANNEL 8
#define PWM_DEPTH 1023
#define PWM_FREQ_MAX 1000
#define PWM_1S 1000000

struct pwm_single_param {
    uint16_t gpio_set;
    uint16_t gpio_clear;
    uint32_t h_time;
};

struct pwm_param {
    uint32_t period;
    uint16_t freq;
    uint16_t duty[PWM_CHANNEL];
};

STATIC const uint8_t pin_num[PWM_CHANNEL] = {0, 2, 4, 5, 12, 13, 14, 15};

STATIC struct pwm_single_param pwm_single_toggle[2][PWM_CHANNEL + 1];
STATIC struct pwm_single_param *pwm_single;

STATIC struct pwm_param pwm;

STATIC int8_t pwm_out_io_num[PWM_CHANNEL] = {-1, -1, -1, -1, -1, -1, -1, -1};

STATIC uint8_t pwm_channel_toggle[2];
STATIC uint8_t *pwm_channel;
STATIC uint8_t pwm_toggle = 1;
STATIC uint8_t pwm_timer_down = 1;
STATIC uint8_t pwm_current_channel = 0;
STATIC uint16_t pwm_gpio = 0;
STATIC uint8_t pwm_channel_num = 0;
STATIC volatile uint8_t pwm_toggle_request = 0;

// XXX: 0xffffffff/(80000000/16)=35A
#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
    (((t) > 0x35A) ?                   \
    (((t) >> 2) * ((APB_CLK_FREQ >> 4) / 250000) + ((t) & 0x3) * ((APB_CLK_FREQ >> 4) / 1000000))  :    \
    (((t) * (APB_CLK_FREQ >> 4)) / 1000000)) :    \
    0)

// FRC1
#define FRC1_ENABLE_TIMER  BIT7

typedef enum {
    DIVDED_BY_1 = 0,
    DIVDED_BY_16 = 4,
    DIVDED_BY_256 = 8,
} TIMER_PREDIVED_MODE;

typedef enum {
    TM_LEVEL_INT = 1,
    TM_EDGE_INT   = 0,
} TIMER_INT_MODE;

STATIC void ICACHE_FLASH_ATTR
pwm_insert_sort(struct pwm_single_param pwm[], uint8 n) {
    uint8 i;

    for (i = 1; i < n; i++) {
        if (pwm[i].h_time < pwm[i - 1].h_time) {
            int8 j = i - 1;
            struct pwm_single_param tmp;

            memcpy(&tmp, &pwm[i], sizeof(struct pwm_single_param));
            memcpy(&pwm[i], &pwm[i - 1], sizeof(struct pwm_single_param));

            while (tmp.h_time < pwm[j].h_time) {
                memcpy(&pwm[j + 1], &pwm[j], sizeof(struct pwm_single_param));
                j--;
                if (j < 0) {
                    break;
                }
            }

            memcpy(&pwm[j + 1], &tmp, sizeof(struct pwm_single_param));
        }
    }
}

STATIC volatile uint8 critical = 0;

#define LOCK_PWM(c)  do {                       \
        while ((c) == 1);                            \
        (c) = 1;                                    \
} while (0)

#define UNLOCK_PWM(c) do {                      \
        (c) = 0;                                    \
} while (0)

void ICACHE_FLASH_ATTR
pwm_start(void) {
    uint8 i, j;
    PWM_DBG("--Function pwm_start() is called\n");
    PWM_DBG("pwm_gpio:%x,pwm_channel_num:%d\n", pwm_gpio, pwm_channel_num);
    PWM_DBG("pwm_out_io_num[0]:%d,[1]:%d,[2]:%d\n", pwm_out_io_num[0], pwm_out_io_num[1], pwm_out_io_num[2]);
    PWM_DBG("pwm.period:%d,pwm.duty[0]:%d,[1]:%d,[2]:%d\n", pwm.period, pwm.duty[0], pwm.duty[1], pwm.duty[2]);

    LOCK_PWM(critical);   // enter critical

    // if a toggle is pending, we reset it since we're changing the settings again
    pwm_toggle_request = 0;

    struct pwm_single_param *local_single = pwm_single_toggle[pwm_toggle ^ 0x01];
    uint8 *local_channel = &pwm_channel_toggle[pwm_toggle ^ 0x01];

    // step 1: init PWM_CHANNEL+1 channels param
    for (i = 0; i < pwm_channel_num; i++) {
        uint32 us = pwm.period * pwm.duty[i] / PWM_DEPTH;
        local_single[i].h_time = US_TO_RTC_TIMER_TICKS(us);
        PWM_DBG("i:%d us:%d ht:%d\n", i, us, local_single[i].h_time);
        local_single[i].gpio_set = 0;
        local_single[i].gpio_clear = 1 << pin_num[pwm_out_io_num[i]];
    }

    local_single[pwm_channel_num].h_time = US_TO_RTC_TIMER_TICKS(pwm.period);
    local_single[pwm_channel_num].gpio_set = pwm_gpio;
    local_single[pwm_channel_num].gpio_clear = 0;
    PWM_DBG("i:%d period:%d ht:%d\n", pwm_channel_num, pwm.period, local_single[pwm_channel_num].h_time);
    // step 2: sort, small to big
    pwm_insert_sort(local_single, pwm_channel_num + 1);

    *local_channel = pwm_channel_num + 1;
    PWM_DBG("1channel:%d,single[0]:%d,[1]:%d,[2]:%d,[3]:%d\n", *local_channel, local_single[0].h_time, local_single[1].h_time, local_single[2].h_time, local_single[3].h_time);
    // step 3: combine same duty channels
    for (i = pwm_channel_num; i > 0; i--) {
        if (local_single[i].h_time == local_single[i - 1].h_time) {
            local_single[i - 1].gpio_set |= local_single[i].gpio_set;
            local_single[i - 1].gpio_clear |= local_single[i].gpio_clear;

            for (j = i + 1; j < *local_channel; j++) {
                memcpy(&local_single[j - 1], &local_single[j], sizeof(struct pwm_single_param));
            }

            (*local_channel)--;
        }
    }
    PWM_DBG("2channel:%d,single[0]:%d,[1]:%d,[2]:%d,[3]:%d\n", *local_channel, local_single[0].h_time, local_single[1].h_time, local_single[2].h_time, local_single[3].h_time);
    // step 4: cacl delt time
    for (i = *local_channel - 1; i > 0; i--) {
        local_single[i].h_time -= local_single[i - 1].h_time;
    }

    // step 5: last channel needs to clean
    local_single[*local_channel - 1].gpio_clear = 0;

    // step 6: if first channel duty is 0, remove it
    if (local_single[0].h_time == 0) {
        local_single[*local_channel - 1].gpio_set &= ~local_single[0].gpio_clear;
        local_single[*local_channel - 1].gpio_clear |= local_single[0].gpio_clear;

        for (i = 1; i < *local_channel; i++) {
            memcpy(&local_single[i - 1], &local_single[i], sizeof(struct pwm_single_param));
        }

        (*local_channel)--;
    }

    // if timer is down, need to set gpio and start timer
    if (pwm_timer_down == 1) {
        pwm_channel = local_channel;
        pwm_single = local_single;
        // start
        gpio_output_set(local_single[0].gpio_set, local_single[0].gpio_clear, pwm_gpio, 0);

        // do the first toggle because timer has to have a valid set to do it's job
        pwm_toggle ^= 0x01;

        pwm_timer_down = 0;
        RTC_REG_WRITE(FRC1_LOAD_ADDRESS, local_single[0].h_time);
    } else {
        // request pwm_tim1_intr_handler to swap the timing buffers
        pwm_toggle_request = 1;
    }

    UNLOCK_PWM(critical);   // leave critical
    PWM_DBG("3channel:%d,single[0]:%d,[1]:%d,[2]:%d,[3]:%d\n", *local_channel, local_single[0].h_time, local_single[1].h_time, local_single[2].h_time, local_single[3].h_time);
}

/******************************************************************************
 * FunctionName : pwm_set_duty
 * Description  : set each channel's duty params
 * Parameters   : int16_t duty  : 0 ~ PWM_DEPTH
 *                uint8 channel : channel index
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
pwm_set_duty(int16_t duty, uint8 channel) {
    uint8 i;
    for (i = 0; i < pwm_channel_num; i++) {
        if (pwm_out_io_num[i] == channel) {
            channel = i;
            break;
        }
    }
    if (i == pwm_channel_num) {    // non found
        return;
    }

    LOCK_PWM(critical);   // enter critical
    if (duty < 1) {
        pwm.duty[channel] = 0;
    } else if (duty >= PWM_DEPTH) {
        pwm.duty[channel] = PWM_DEPTH;
    } else {
        pwm.duty[channel] = duty;
    }
    UNLOCK_PWM(critical);   // leave critical
}

/******************************************************************************
 * FunctionName : pwm_set_freq
 * Description  : set pwm frequency
 * Parameters   : uint16 freq : 100hz typically
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
pwm_set_freq(uint16 freq, uint8 channel) {
    LOCK_PWM(critical);   // enter critical
    if (freq > PWM_FREQ_MAX) {
        pwm.freq = PWM_FREQ_MAX;
    } else if (freq < 1) {
        pwm.freq = 1;
    } else {
        pwm.freq = freq;
    }

    pwm.period = PWM_1S / pwm.freq;
    UNLOCK_PWM(critical);   // leave critical
}

/******************************************************************************
 * FunctionName : pwm_get_duty
 * Description  : get duty of each channel
 * Parameters   : uint8 channel : channel index
 * Returns      : NONE
*******************************************************************************/
uint16 ICACHE_FLASH_ATTR
pwm_get_duty(uint8 channel) {
    uint8 i;
    for (i = 0; i < pwm_channel_num; i++) {
        if (pwm_out_io_num[i] == channel) {
            channel = i;
            break;
        }
    }
    if (i == pwm_channel_num) {    // non found
        return 0;
    }

    return pwm.duty[channel];
}

/******************************************************************************
 * FunctionName : pwm_get_freq
 * Description  : get pwm frequency
 * Parameters   : NONE
 * Returns      : uint16 : pwm frequency
*******************************************************************************/
uint16 ICACHE_FLASH_ATTR
pwm_get_freq(uint8 channel) {
    return pwm.freq;
}

/******************************************************************************
 * FunctionName : pwm_period_timer
 * Description  : pwm period timer function, output high level,
 *                start each channel's high level timer
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
STATIC void ICACHE_RAM_ATTR
pwm_tim1_intr_handler(void *dummy) {
    (void)dummy;

    RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);

    if (pwm_current_channel >= (*pwm_channel - 1)) {        // *pwm_channel may change outside

        if (pwm_toggle_request != 0) {
            pwm_toggle ^= 1;
            pwm_toggle_request = 0;
        }

        pwm_single = pwm_single_toggle[pwm_toggle];
        pwm_channel = &pwm_channel_toggle[pwm_toggle];

        gpio_output_set(pwm_single[*pwm_channel - 1].gpio_set,
            pwm_single[*pwm_channel - 1].gpio_clear,
            pwm_gpio,
            0);

        pwm_current_channel = 0;

        RTC_REG_WRITE(FRC1_LOAD_ADDRESS, pwm_single[pwm_current_channel].h_time);
    } else {
        gpio_output_set(pwm_single[pwm_current_channel].gpio_set,
            pwm_single[pwm_current_channel].gpio_clear,
            pwm_gpio, 0);

        pwm_current_channel++;
        RTC_REG_WRITE(FRC1_LOAD_ADDRESS, pwm_single[pwm_current_channel].h_time);
    }
}

/******************************************************************************
 * FunctionName : pwm_init
 * Description  : pwm gpio, params and timer initialization
 * Parameters   : uint16 freq : pwm freq param
 *                uint16 *duty : each channel's duty
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
pwm_init(void) {
    uint8 i;

    RTC_REG_WRITE(FRC1_CTRL_ADDRESS,  // FRC2_AUTO_RELOAD|
        DIVDED_BY_16
        | FRC1_ENABLE_TIMER
        | TM_EDGE_INT);
    RTC_REG_WRITE(FRC1_LOAD_ADDRESS, 0);

    for (i = 0; i < PWM_CHANNEL; i++) {
        pwm_gpio = 0;
        pwm.duty[i] = 0;
    }

    pwm_set_freq(500, 0);
    pwm_start();

    ETS_FRC_TIMER1_INTR_ATTACH(pwm_tim1_intr_handler, NULL);
    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INTR_ENABLE();
}

int ICACHE_FLASH_ATTR
pwm_add(uint8_t pin_id, uint32_t pin_mux, uint32_t pin_func) {
    PWM_DBG("--Function pwm_add() is called. channel:%d\n", channel);
    PWM_DBG("pwm_gpio:%x,pwm_channel_num:%d\n", pwm_gpio, pwm_channel_num);
    PWM_DBG("pwm_out_io_num[0]:%d,[1]:%d,[2]:%d\n", pwm_out_io_num[0], pwm_out_io_num[1], pwm_out_io_num[2]);
    PWM_DBG("pwm.duty[0]:%d,[1]:%d,[2]:%d\n", pwm.duty[0], pwm.duty[1], pwm.duty[2]);
    int channel = -1;
    for (int i = 0; i < PWM_CHANNEL; ++i) {
        if (pin_num[i] == pin_id) {
            channel = i;
            break;
        }
    }
    if (channel == -1) {
        return -1;
    }
    uint8 i;
    for (i = 0; i < PWM_CHANNEL; i++) {
        if (pwm_out_io_num[i] == channel) { // already exist
            return channel;
        }
        if (pwm_out_io_num[i] == -1) { // empty exist
            LOCK_PWM(critical);   // enter critical
            pwm_out_io_num[i] = channel;
            pwm.duty[i] = 0;
            pwm_gpio |= (1 << pin_num[channel]);
            PIN_FUNC_SELECT(pin_mux, pin_func);
            pwm_channel_num++;
            UNLOCK_PWM(critical);   // leave critical
            return channel;
        }
    }
    return -1;
}

bool ICACHE_FLASH_ATTR
pwm_delete(uint8 channel) {
    PWM_DBG("--Function pwm_delete() is called. channel:%d\n", channel);
    PWM_DBG("pwm_gpio:%x,pwm_channel_num:%d\n", pwm_gpio, pwm_channel_num);
    PWM_DBG("pwm_out_io_num[0]:%d,[1]:%d,[2]:%d\n", pwm_out_io_num[0], pwm_out_io_num[1], pwm_out_io_num[2]);
    PWM_DBG("pwm.duty[0]:%d,[1]:%d,[2]:%d\n", pwm.duty[0], pwm.duty[1], pwm.duty[2]);
    uint8 i, j;
    for (i = 0; i < pwm_channel_num; i++) {
        if (pwm_out_io_num[i] == channel) { // exist
            LOCK_PWM(critical);   // enter critical
            pwm_out_io_num[i] = -1;
            pwm_gpio &= ~(1 << pin_num[channel]);   // clear the bit
            for (j = i; j < pwm_channel_num - 1; j++) {
                pwm_out_io_num[j] = pwm_out_io_num[j + 1];
                pwm.duty[j] = pwm.duty[j + 1];
            }
            pwm_out_io_num[pwm_channel_num - 1] = -1;
            pwm.duty[pwm_channel_num - 1] = 0;
            pwm_channel_num--;
            UNLOCK_PWM(critical);   // leave critical
            return true;
        }
    }
    // non found
    return true;
}
