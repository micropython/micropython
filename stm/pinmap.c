#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "nlr.h"

#include "pinmap.h"

typedef struct {
  const char *name;
  pinmap_pin_t pin;
} pinmap_map_t;

static const pinmap_map_t pinmap_board_map[] = {

#if defined(PYBOARD3)

  {"LED_G1",  PINMAP_PORT_PIN(GPIOC,  4)},
  {"LED_G2",  PINMAP_PORT_PIN(GPIOC,  5)},
  {"LED_R1",  PINMAP_PORT_PIN(GPIOA,  8)},
  {"LED_R2",  PINMAP_PORT_PIN(GPIOA, 10)},
  {"SW",      PINMAP_PORT_PIN(GPIOA, 13)},

#elif defined(PYBOARD4)

    {"X1",  PINMAP_PORT_PIN(GPIOB,  6)},
    {"X2",  PINMAP_PORT_PIN(GPIOB,  7)},
    {"X3",  PINMAP_PORT_PIN(GPIOC,  4)},
    {"X4",  PINMAP_PORT_PIN(GPIOB,  5)},
  //{"X5",  Reset },
  //{"X6",  GND   },
  //{"X7",  3.3V  },
  //{"X8",  VIN   },
    {"X9",  PINMAP_PORT_PIN(GPIOA,  0)},
    {"X10", PINMAP_PORT_PIN(GPIOA,  1)},
    {"X11", PINMAP_PORT_PIN(GPIOA,  2)},
    {"X12", PINMAP_PORT_PIN(GPIOA,  3)},
    {"X13", PINMAP_PORT_PIN(GPIOA,  4)},
    {"X14", PINMAP_PORT_PIN(GPIOA,  5)},
    {"X15", PINMAP_PORT_PIN(GPIOA,  6)},
    {"X16", PINMAP_PORT_PIN(GPIOA,  7)},

    {"X17", PINMAP_PORT_PIN(GPIOB, 3)},
    {"X18", PINMAP_PORT_PIN(GPIOC, 13)},
    {"X19", PINMAP_PORT_PIN(GPIOC,  0)},
    {"X20", PINMAP_PORT_PIN(GPIOC,  1)},
    {"X21", PINMAP_PORT_PIN(GPIOC,  2)},
    {"X22", PINMAP_PORT_PIN(GPIOC,  3)},
  //{"X23", A3.3V },
  //{"X24", AGND  },

    {"Y1",  PINMAP_PORT_PIN(GPIOB, 10)},
    {"Y2",  PINMAP_PORT_PIN(GPIOB, 11)},
    {"Y3",  PINMAP_PORT_PIN(GPIOB,  0)},
    {"Y4",  PINMAP_PORT_PIN(GPIOB,  1)},
  //{"Y5",  Reset },
  //{"Y6",  GND   },
  //{"Y7",  3.3V  },
  //{"Y8",  VIN   },
    {"Y9",  PINMAP_PORT_PIN(GPIOC,  6)},
    {"Y10", PINMAP_PORT_PIN(GPIOC,  7)},
    {"Y11", PINMAP_PORT_PIN(GPIOB,  8)},
    {"Y12", PINMAP_PORT_PIN(GPIOB,  9)},
    {"Y13", PINMAP_PORT_PIN(GPIOB, 12)},
    {"Y14", PINMAP_PORT_PIN(GPIOB, 13)},
    {"Y15", PINMAP_PORT_PIN(GPIOB, 14)},
    {"Y16", PINMAP_PORT_PIN(GPIOB, 15)},

    {"LED_BLUE",   PINMAP_PORT_PIN(GPIOB,  4)},
    {"LED_RED",    PINMAP_PORT_PIN(GPIOA, 13)},
    {"LED_GREEN",  PINMAP_PORT_PIN(GPIOA, 14)},
    {"LED_YELLOW", PINMAP_PORT_PIN(GPIOA, 15)},
    {"SW",         PINMAP_PORT_PIN(GPIOB,  3)},

#elif defined(STM32F4DISC)

    {"LED_GREEN",   PINMAP_PORT_PIN(GPIOD, 12)},
    {"LED_ORANGE",  PINMAP_PORT_PIN(GPIOD, 13)},
    {"LED_RED",     PINMAP_PORT_PIN(GPIOD, 14)},
    {"LED_BLUE",    PINMAP_PORT_PIN(GPIOD, 15)},
    {"SW",          PINMAP_PORT_PIN(GPIOA,  0)},

#elif defined(NETDUINO_PLUS_2)

    {"D0",  PINMAP_PORT_PIN(GPIOC,  7)},
    {"D1",  PINMAP_PORT_PIN(GPIOC,  6)},
    {"D2",  PINMAP_PORT_PIN(GPIOA,  3)},
    {"D3",  PINMAP_PORT_PIN(GPIOA,  2)},
    {"D4",  PINMAP_PORT_PIN(GPIOB, 12)},
    {"D5",  PINMAP_PORT_PIN(GPIOB,  8)},
    {"D6",  PINMAP_PORT_PIN(GPIOB,  9)},
    {"D7",  PINMAP_PORT_PIN(GPIOA,  1)},
    {"D8",  PINMAP_PORT_PIN(GPIOA,  0)},
    {"D9",  PINMAP_PORT_PIN(GPIOA,  6)},
    {"D10", PINMAP_PORT_PIN(GPIOB, 10)},
    {"D11", PINMAP_PORT_PIN(GPIOB, 15)},
    {"D12", PINMAP_PORT_PIN(GPIOB, 14)},
    {"D13", PINMAP_PORT_PIN(GPIOB, 13)},

    {"A0",  PINMAP_PORT_PIN(GPIOC, 0)},
    {"A1",  PINMAP_PORT_PIN(GPIOC, 1)},
    {"A2",  PINMAP_PORT_PIN(GPIOC, 2)},
    {"A3",  PINMAP_PORT_PIN(GPIOC, 3)},
    {"A4",  PINMAP_PORT_PIN(GPIOC, 4)},
    {"A5",  PINMAP_PORT_PIN(GPIOC, 5)},

    {"LED",       PINMAP_PORT_PIN(GPIOA, 10)},
    {"SW",        PINMAP_PORT_PIN(GPIOB, 11)},
    {"PWR_LED",   PINMAP_PORT_PIN(GPIOC, 13)},
    {"PWR_SD",    PINMAP_PORT_PIN(GPIOB,  1)},
    {"PWR_HDR",   PINMAP_PORT_PIN(GPIOB,  2)},
    {"PWR_ETH",   PINMAP_PORT_PIN(GPIOC, 15)},
    {"RST_ETH",   PINMAP_PORT_PIN(GPIOD,  2)},

#endif
};

#define NUM_MAP_ENTRIES (sizeof(pinmap_board_map)/sizeof(pinmap_board_map[0]))

static mp_obj_t pinmap_map_fn_obj = MP_OBJ_NULL;

// Function used by the python bundings to convert a user-supplied pin name
// into an ordinal pin number.
pinmap_pin_t pinmap_map_user_obj(mp_obj_t user_obj)
{
  if (pinmap_map_fn_obj) {
    mp_obj_t pin_obj = rt_call_function_1(pinmap_map_fn_obj, user_obj);
    if (!MP_OBJ_IS_SMALL_INT(pin_obj)) {
      nlr_jump(mp_obj_new_exception_msg(MP_QSTR_ValueError, "pin mapper didn't return an integer"));
    }
    pinmap_pin_t pin = MP_OBJ_SMALL_INT_VALUE(pin_obj);
    if (pin != PINMAP_PIN_NONE) {
      return pin;
    }
    // The pin mapping function returned PINMAP_PIN_NONE, fall through to the
    // default lookup methods.
  }

  const char *pin_name = mp_obj_str_get_str(user_obj);
  const pinmap_map_t *entry = &pinmap_board_map[0];
  for (; entry < &pinmap_board_map[NUM_MAP_ENTRIES]; entry++) {
    if (strcmp(pin_name, entry->name) == 0) {
      return entry->pin;
    }
  }

  // Not a board pin, try port/pin
  uint port_num;
  if ((pin_name[0] >= 'A') && (pin_name[0] <= 'J')) {
    port_num = pin_name[0] - 'A';
  } else if ((pin_name[0] >= 'a') && (pin_name[0] <= 'j')) {
    port_num = pin_name[0] - 'a';
  } else {
    goto pin_error;
  }

  uint pin_num = 0;
  for (const char *s = pin_name + 1; *s; s++) {
      if (!('0' <= *s && *s <= '9')) {
          goto pin_error;
      }
      pin_num = 10 * pin_num + *s - '0';
  }
  if (!(0 <= pin_num && pin_num <= 15)) {
    goto pin_error;
  }

  return (port_num << 4) | pin_num;

pin_error:
    nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_ValueError, "pin '%s' not a valid pin identifier", pin_name));
}

static mp_obj_t pyb_pinmap_register_map_fn(mp_obj_t map_fn_obj)
{
  pinmap_map_fn_obj = map_fn_obj;
  return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_pinmap_register_map_fn_obj, pyb_pinmap_register_map_fn);
