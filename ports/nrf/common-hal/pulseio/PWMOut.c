/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2016 Damien P. George
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

#include <stdint.h>
#include "nrf.h"

#include "py/runtime.h"
#include "common-hal/pulseio/PWMOut.h"
#include "shared-bindings/pulseio/PWMOut.h"

#define PWM_MAX_MODULE    3
#define PWM_MAX_CHANNEL   4

#define PWM_MAX_FREQ      (16000000)

NRF_PWM_Type* const pwm_arr[PWM_MAX_MODULE] = { NRF_PWM0, NRF_PWM1, NRF_PWM2 };

uint16_t _seq0[PWM_MAX_MODULE][PWM_MAX_CHANNEL];


static int pin2channel(NRF_PWM_Type* pwm, uint8_t pin)
{
  for(int i=0; i < PWM_MAX_CHANNEL; i++)
  {
    if ( pwm->PSEL.OUT[i] == ((uint32_t)pin) ) return i;
  }

  return -1;
}

static int find_free_channel(NRF_PWM_Type* pwm)
{
  for(int i=0; i < PWM_MAX_CHANNEL; i++)
  {
    if (pwm->PSEL.OUT[i] == 0xFFFFFFFFUL)
    {
      return i;
    }
  }

  return -1;
}

static bool pwm_is_unused(NRF_PWM_Type* pwm)
{
  for(int i=0; i < PWM_MAX_CHANNEL; i++)
  {
    if (pwm->PSEL.OUT[i] != 0xFFFFFFFFUL)
    {
      return false;
    }
  }

  return true;
}

static void find_new_pwm(pulseio_pwmout_obj_t* self)
{
  // First find unused PWM module
  for(int i=0; i<PWM_MAX_MODULE; i++)
  {
    if ( pwm_is_unused(pwm_arr[i]) )
    {
      self->pwm     = pwm_arr[i];
      self->channel = 0;
      return;
    }
  }

  // Find available channel in a using PWM
  for(int i=0; i<PWM_MAX_MODULE; i++)
  {
    int ch = find_free_channel(pwm_arr[i]);
    if ( ch >= 0 )
    {
      self->pwm     = pwm_arr[i];
      self->channel = (uint8_t) ch;
      return;
    }
  }
}

void pwmout_reset(void)
{
  for(int i=0; i<PWM_MAX_MODULE; i++)
  {
    NRF_PWM_Type* pwm = pwm_arr[i];

    pwm->MODE            = PWM_MODE_UPDOWN_Up;
    pwm->DECODER         = PWM_DECODER_LOAD_Individual;
    pwm->LOOP            = 0;
    pwm->PRESCALER       = PWM_PRESCALER_PRESCALER_DIV_1; // default is 500 hz
    pwm->COUNTERTOP      = (PWM_MAX_FREQ/500);                // default is 500 hz

    pwm->SEQ[0].PTR      = (uint32_t) _seq0[i];
    pwm->SEQ[0].CNT      = PWM_MAX_CHANNEL; // default mode is Individual --> count must be 4
    pwm->SEQ[0].REFRESH  = 0;
    pwm->SEQ[0].ENDDELAY = 0;

    pwm->SEQ[1].PTR      = 0;
    pwm->SEQ[1].CNT      = 0;
    pwm->SEQ[1].REFRESH  = 0;
    pwm->SEQ[1].ENDDELAY = 0;

    for(int ch =0; ch < PWM_MAX_CHANNEL; ch++)
    {
      _seq0[i][ch] = (1UL << 15); // polarity = 0
    }
  }
}

void common_hal_pulseio_pwmout_construct(pulseio_pwmout_obj_t* self,
                                          const mcu_pin_obj_t* pin,
                                          uint16_t duty,
                                          uint32_t frequency,
                                          bool variable_frequency) {
  self->pwm = NULL;
  self->pin = pin;

  // check if mapped to PWM channel already
  for(int i=0; i<PWM_MAX_MODULE; i++)
  {
    int ch = pin2channel(pwm_arr[i], pin->pin);
    if ( ch >= 0 )
    {
      self->pwm = pwm_arr[i];
      self->channel = (uint8_t) ch;
      break;
    }
  }

  // Haven't mapped before
  if ( !self->pwm )
  {
    find_new_pwm(self);
  }

  if (self->pwm)
  {
    hal_gpio_cfg_pin(pin->port, pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);

    // disable before mapping pin channel
    self->pwm->ENABLE = 0;

    self->pwm->PSEL.OUT[self->channel] = pin->pin;

    self->pwm->COUNTERTOP = (PWM_MAX_FREQ/frequency);
    self->freq = frequency;
    self->variable_freq = variable_frequency;

    self->pwm->ENABLE = 1;

    common_hal_pulseio_pwmout_set_duty_cycle(self, duty);
  }
}

bool common_hal_pulseio_pwmout_deinited(pulseio_pwmout_obj_t* self) {
    return self->pwm == NULL;
}

void common_hal_pulseio_pwmout_deinit(pulseio_pwmout_obj_t* self) {
  if (common_hal_pulseio_pwmout_deinited(self)) {
    return;
  }

  self->pwm->ENABLE = 0;

  self->pwm->PSEL.OUT[self->channel] = 0xFFFFFFFFUL;

  // re-enable PWM module if there is other active channel
  for(int i=0; i < PWM_MAX_CHANNEL; i++)
  {
    if (self->pwm->PSEL.OUT[i] != 0xFFFFFFFFUL)
    {
      self->pwm->ENABLE = 1;
      break;
    }
  }

  hal_gpio_cfg_pin(self->pin->port, self->pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

  self->pwm = NULL;
  self->pin = mp_const_none;
}

void common_hal_pulseio_pwmout_set_duty_cycle(pulseio_pwmout_obj_t* self, uint16_t duty) {
  self->duty = duty;

  uint16_t* p_value = ((uint16_t*)self->pwm->SEQ[0].PTR) + self->channel;
  *p_value = ((duty * self->pwm->COUNTERTOP) / 0xFFFF) | (1 << 15);

  self->pwm->TASKS_SEQSTART[0] = 1;
}

uint16_t common_hal_pulseio_pwmout_get_duty_cycle(pulseio_pwmout_obj_t* self) {
  return self->duty;
}

void common_hal_pulseio_pwmout_set_frequency(pulseio_pwmout_obj_t* self, uint32_t frequency) {
  if (frequency == 0 || frequency > 16000000) {
    mp_raise_ValueError("Invalid PWM frequency");
  }

  self->freq = frequency;
  self->pwm->COUNTERTOP = (PWM_MAX_FREQ/frequency);
  self->pwm->TASKS_SEQSTART[0] = 1;
}

uint32_t common_hal_pulseio_pwmout_get_frequency(pulseio_pwmout_obj_t* self) {
  return self->freq;
}

bool common_hal_pulseio_pwmout_get_variable_frequency(pulseio_pwmout_obj_t* self) {
  return self->variable_freq;
}

