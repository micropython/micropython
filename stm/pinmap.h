MP_DECLARE_CONST_FUN_OBJ(pyb_pinmap_register_map_fn_obj);

#define PINMAP_PIN_NONE 0xffff

typedef uint pinmap_pin_t;

// C function for mapping python pin identifier into an ordinal pin number.
pinmap_pin_t pinmap_map_user_obj(mp_obj_t user_obj);

// Each GPIO block occupies 0x400 addreses (1 << 10)
#define PINMAP_PORTNUM(port)  (((uint8_t *)(port) - (uint8_t *)GPIOA) >> 10)

// PINMAP_PORT_PIN(GPIOB, 12) will return the ordinal pin number for B12 which is 28
#define PINMAP_PORT_PIN(port_num, pin_num) ((PINMAP_PORTNUM(port_num) << 4) | ((pin_num) & 0xf))

#define PINMAP_PORT_NUM_FROM_PIN(pin)  (((pin) & 0xf0) >> 4)
#define PINMAP_PIN_NUM_FROM_PIN(pin)   ((pin) & 0xf)
#define PINMAP_GPIO_PORT(port_num)     ((GPIO_TypeDef *)((uint8_t *)GPIOA + ((port_num) << 10)))
#define PINMAP_GPIO_PORT_FROM_PIN(pin) PINMAP_GPIO_PORT(PINMAP_PORT_NUM_FROM_PIN(pin))
