/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "board.h"

/* SDK drivers */
#include "fsl_ctimer.h"

#include "drv_pwm.h"

static int drv_pwm_current_period_channel(drv_pwm_t *pwm) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    uint32_t mcr = ct->MCR;

    for (uint8_t i = 0; i < 4; i++) {
        if (mcr & (1U << (3 * i + CTIMER_MCR_MR0R_SHIFT))) {
            return i;
        }
    }

    return -1;
}

static int drv_pwm_first_free_channel(drv_pwm_t *pwm) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    int pc = drv_pwm_current_period_channel(pwm);

    uint32_t pwmc = ct->PWMC;

    for (uint8_t i = 0; i < 4; i++) {
        if (pwmc & (1U << i)) {
            /* Skip this channel if there's an active PWM output */
            continue;
        }

        if (i == pc) {
            /* Skip this channel if this channel is the current period channel */
            continue;
        }

        return i;
    }

    /* There are no free channels left. */
    return -1;
}

int drv_pwm_init(drv_pwm_t *pwm, uint8_t id, uint8_t channel, uint32_t freq, bool inverted) {
    pwm->id = id;
    pwm->channel = channel;
    pwm->instance = MCX_BoardGetCT32Instance(id);

    if (pwm->instance == NULL) {
        return -EINVAL;
    }

    pwm->input_freq = MCX_BoardConfigureCT32Clock(id);

    pwm->inverted = inverted;

    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    if ((ct->TCR & CTIMER_TCR_CEN_MASK) == 0U) {
        /* There's two conditions for a all-zero TCR: either a reset condition or timer is stopped. */
        /* In either case, we need to initialize the timer instance (AHB RST CTRL). */

        /* TODO: Do not use SDK functions */

        ctimer_config_t ct_cfg = {
            .mode = kCTIMER_TimerMode,
            .prescale = 1U,
        };

        /* Frequency: 150MHz max., we got 32bit counters, we can take that. */
        /* Approx. maximum period: 28.6 seconds. */

        CTIMER_Init(ct, &ct_cfg);

        /* Current timer is not running, we are the first channel being configured. */

        ct->TC = 0U;   /* Reset counter */
        ct->PC = 0U;   /* Reset prescaler counter */
        ct->PR = 0U;   /* Prescaler, divide by 1 to get best resolution */
        ct->MCR = 0U;  /* Reset interrupt and reset condition */
        ct->EMR = 0U;  /* Do nothing on match event and output 0 as default state */
        ct->PWMC = 0U; /* Disable all PWM channels, outputs will be controlled by EMn */

        /* Here, we have a favoritism of using channel 3 as period channel, unless channel 3 is used for output */
        if (channel != 3) {
            ct->MR[3] = pwm->input_freq / freq;
            ct->MCR |= CTIMER_MCR_MR3R_MASK;
        } else {
            /* Use channel 2 as period channel. */
            ct->MR[2] = pwm->input_freq / freq;
            ct->MCR |= CTIMER_MCR_MR2R_MASK;
        }

        /* Start counter */
        ct->TCR |= CTIMER_TCR_CEN_MASK;
    } else {
        /*
         * Due to the nature of the CTimer, one of the 4 match channels is needed for period control (frequency)
         * To find out which one is the current period channel, check the MRxR bit for each match output.
         * If we are configuring the same match being used as periodic channel, configure the next free match as period
         * then current channel can be re-used. If all 4 channels are in use then the function will fail with an errno.
         */

        /* The timer is running, check whether we need to re-locate the period channel */
        int p_channel = drv_pwm_current_period_channel(pwm);
        if (p_channel < 0) {
            return -EINVAL;
        }

        if (p_channel == channel) {
            /* We need to re-locate the period channel */

            int f_channel = drv_pwm_first_free_channel(pwm);
            if (f_channel < 0) {
                /* There's no free channel, bail out. */
                return -EBUSY;
            }

            /* Transfer the period channel to first free channel */

            /* Step 1: Copy current period to first free channel */
            ct->MR[f_channel] = ct->MR[p_channel];

            /* Step 2: Enable reset for new period channel */
            /* Note: it's safe doing it here since both old and new channel MRs contains same value */
            ct->MCR |= (CTIMER_MCR_MR0R_MASK << (3 * f_channel));

            /* Step 3: Disable reset for old period channel */
            ct->MCR &= ~(CTIMER_MCR_MR0R_MASK << (3 * p_channel));

            /* The old period channel is now available for PWM output */
            p_channel = f_channel;
        }

        if (drv_pwm_freq_set(pwm, freq) < 0) {
            return -EINVAL;
        }

    }

    return 0;
}

int drv_pwm_deinit(drv_pwm_t *pwm) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    /* Disable PWM output for channel, now the output is controlled by EMR */
    ct->PWMC &= ~(CTIMER_PWMC_PWMEN0_MASK << pwm->channel);

    /* Check whether there's still enabled channels */
    if ((ct->PWMC & 0x0FU) == 0) {
        /* Stop counter if this is the last. */
        ct->TCR &= ~CTIMER_TCR_CEN_MASK;
    }

    return 0;
}

int drv_pwm_start(drv_pwm_t *pwm) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    ct->PWMC |= (1U << pwm->channel);

    return 0;
}

int drv_pwm_stop(drv_pwm_t *pwm) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    ct->PWMC &= ~(1U << pwm->channel);

    return 0;
}

int drv_pwm_freq_set(drv_pwm_t *pwm, uint32_t freq) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    int p_channel = drv_pwm_current_period_channel(pwm);
    if (p_channel < 0) {
        return -EINVAL;
    }

    /* Store new values in shadow registers */
    ct->MSR[p_channel] = pwm->input_freq / freq;

    /* Enable period channel interrupt to check a reload event occurs.
     * Since interrupts are not configured from NVIC, so no ISR will occur.
     * Check IR[MRnINT] for reload point.
     */
    uint32_t mcr_mask = (CTIMER_MCR_MR0RL_MASK << p_channel) | (CTIMER_MCR_MR0I_MASK << (3 * p_channel));

    for (uint8_t i = 0; i < 4; i++) {
        if (ct->PWMC & (1U << i)) {
            /* Channel PWM output is enabled, calculate new values and store into shadow registers */
            uint32_t new_mr = ct->MR[i] * ct->MSR[p_channel] / ct->MR[p_channel];
            ct->MSR[i] = new_mr;

            /* Update MRnRL map */
            mcr_mask |= CTIMER_MCR_MR0RL_MASK << i;
        }
    }

    /* Reload MRs on next counter reset, enable reload MR interrupt */
    ct->MCR |= mcr_mask;

    while ((ct->IR & (CTIMER_IR_MR0INT_MASK << p_channel)) == 0U) {
        /* -- */
    }

    /* Disable reload channel interrupt and MSR synchronization */
    ct->MCR &= ~mcr_mask;

    /* Clear interrupt flags. */
    ct->IR |= (CTIMER_IR_MR0INT_MASK << p_channel);

    return 0;
}

int drv_pwm_freq_get(drv_pwm_t *pwm, uint32_t *freq) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    int p_channel = drv_pwm_current_period_channel(pwm);
    if (p_channel < 0) {
        return -1;
    }

    *freq = pwm->input_freq / ct->MR[p_channel];

    return 0;
}

int drv_pwm_duty_set(drv_pwm_t *pwm, uint16_t duty_u16) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    int p_channel = drv_pwm_current_period_channel(pwm);
    if (p_channel < 0) {
        return -1;
    }

    /* To avoid overflow, use 64bit values here. */
    uint64_t period = ct->MR[p_channel];

    if (pwm->inverted) {
        ct->MSR[pwm->channel] = period * duty_u16 / 65535;
    } else {
        ct->MSR[pwm->channel] = period - (period * duty_u16 / 65535);
    }

    /* Reload MRn on the next cycle */
    ct->MCR |= (CTIMER_MCR_MR0RL_MASK << pwm->channel);

    /* Wait for new duty cycle loaded into the MRn */
    while (ct->MR[pwm->channel] != ct->MSR[pwm->channel]) {
        /* -- */
    }

    /* Disable shadow register updates */
    ct->MCR &= ~(CTIMER_MCR_MR0RL_MASK << pwm->channel);

    return 0;
}

int drv_pwm_duty_get(drv_pwm_t *pwm, uint16_t *duty_u16) {
    CTIMER_Type *ct = (CTIMER_Type *)pwm->instance;

    int p_channel = drv_pwm_current_period_channel(pwm);
    if (p_channel < 0) {
        return -1;
    }

    uint32_t period = ct->MR[p_channel];

    /* To avoid overflow, use 64bit values here. */
    uint64_t duty = ct->MR[pwm->channel];
    *duty_u16 = duty * 65535 / period;

    return 0;
}
