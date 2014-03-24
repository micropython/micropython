enum {
  PORT_A,
  PORT_B,
  PORT_C,
  PORT_D,
  PORT_E,
  PORT_F,
  PORT_G,
  PORT_H,
  PORT_I,
  PORT_J,
};

enum {
  AF_FN_TIM,
  AF_FN_I2C,
  AF_FN_USART,
  AF_FN_UART = AF_FN_USART,
  AF_FN_SPI
};

enum {
  AF_PIN_TYPE_TIM_CH1 = 0,
  AF_PIN_TYPE_TIM_CH2,
  AF_PIN_TYPE_TIM_CH3,
  AF_PIN_TYPE_TIM_CH4,
  AF_PIN_TYPE_TIM_CH1N,
  AF_PIN_TYPE_TIM_CH2N,
  AF_PIN_TYPE_TIM_CH3N,
  AF_PIN_TYPE_TIM_CH1_ETR,
  AF_PIN_TYPE_TIM_ETR,
  AF_PIN_TYPE_TIM_BKIN,

  AF_PIN_TYPE_I2C_SDA = 0,
  AF_PIN_TYPE_I2C_SCL,

  AF_PIN_TYPE_USART_TX = 0,
  AF_PIN_TYPE_USART_RX,
  AF_PIN_TYPE_USART_CTS,
  AF_PIN_TYPE_USART_RTS,
  AF_PIN_TYPE_USART_CK,
  AF_PIN_TYPE_UART_TX  = AF_PIN_TYPE_USART_TX,
  AF_PIN_TYPE_UART_RX  = AF_PIN_TYPE_USART_RX,
  AF_PIN_TYPE_UART_CTS = AF_PIN_TYPE_USART_CTS,
  AF_PIN_TYPE_UART_RTS = AF_PIN_TYPE_USART_RTS,

  AF_PIN_TYPE_SPI_MOSI = 0,
  AF_PIN_TYPE_SPI_MISO,
  AF_PIN_TYPE_SPI_SCK,
  AF_PIN_TYPE_SPI_NSS,
};

enum {
  PIN_ADC1  = (1 << 0),
  PIN_ADC2  = (1 << 1),
  PIN_ADC3  = (1 << 2),
};

typedef struct {
  mp_obj_base_t base;
  uint8_t idx;
  uint8_t fn;
  uint8_t unit;
  uint8_t type;

  union {
    void          *reg;
    TIM_TypeDef   *TIM;
    I2C_TypeDef   *I2C;
    USART_TypeDef *USART;
    USART_TypeDef *UART;
    SPI_TypeDef   *SPI;
  };
} pin_af_obj_t;

typedef struct {
  mp_obj_base_t base;
  const char *name;
  uint16_t port   : 4;
  uint16_t pin    : 4;
  uint16_t num_af : 4;
  uint16_t adc_channel : 4;
  uint16_t adc_num  : 3;  // 1 bit per ADC
  uint16_t pin_mask;
  GPIO_TypeDef *gpio;
  const pin_af_obj_t *af;
} pin_obj_t;

extern const mp_obj_type_t pin_obj_type;
extern const mp_obj_type_t pin_af_obj_type;

typedef struct {
  const char  *name;
  const pin_obj_t *pin;
} pin_named_pin_t;

extern const pin_named_pin_t pin_board_pins[];
extern const pin_named_pin_t pin_cpu_pins[];

typedef struct {
    mp_obj_base_t base;
    mp_obj_t mapper;
    mp_obj_t map_dict;
    bool debug;
} pin_map_obj_t;

extern pin_map_obj_t pin_map_obj;

typedef struct {
    mp_obj_base_t base;
    const char *name;
    const pin_named_pin_t *named_pins;
} pin_named_pins_obj_t;

extern const pin_named_pins_obj_t pin_board_pins_obj;
extern const pin_named_pins_obj_t pin_cpu_pins_obj;

const pin_obj_t *pin_find_named_pin(const pin_named_pin_t *pins, const char *name);
const pin_af_obj_t *pin_find_af(const pin_obj_t *pin, uint8_t fn, uint8_t unit, uint8_t pin_type);

void pin_map_init(void);

// C function for mapping python pin identifier into an ordinal pin number.
const pin_obj_t *pin_map_user_obj(mp_obj_t user_obj);

