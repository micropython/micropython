#include <mk20dx128.h>
#include "hal_ftm.h"

#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#define HAL_NVIC_EnableIRQ(irq)    NVIC_ENABLE_IRQ(irq)

#define GPIOA   ((GPIO_TypeDef *)&GPIOA_PDOR)
#define GPIOB   ((GPIO_TypeDef *)&GPIOB_PDOR)
#define GPIOC   ((GPIO_TypeDef *)&GPIOC_PDOR)
#define GPIOD   ((GPIO_TypeDef *)&GPIOD_PDOR)
#define GPIOE   ((GPIO_TypeDef *)&GPIOE_PDOR)
#define GPIOZ   ((GPIO_TypeDef *)NULL)

#define I2C0    ((I2C_TypeDef *)0x40066000)
#define I2C1    ((I2C_TypeDef *)0x40067000)

#undef  SPI0
#define SPI0    ((SPI_TypeDef *)0x4002C000)
#define SPI1    ((SPI_TypeDef *)0x4002D000)

#define UART0   ((UART_TypeDef *)&UART0_BDH)
#define UART1   ((UART_TypeDef *)&UART1_BDH)
#define UART2   ((UART_TypeDef *)&UART2_BDH)

typedef struct {
    uint32_t dummy;
} I2C_TypeDef;

typedef struct {
    uint32_t dummy;
} UART_TypeDef;

typedef struct {
    uint32_t dummy;
} SPI_TypeDef;

typedef struct {
    volatile    uint32_t    PDOR;   // Output register
    volatile    uint32_t    PSOR;   // Set output register
    volatile    uint32_t    PCOR;   // Clear output register
    volatile    uint32_t    PTOR;   // Toggle output register
    volatile    uint32_t    PDIR;   // Data Input register
    volatile    uint32_t    PDDR;   // Data Direction register
} GPIO_TypeDef;

#define GPIO_OUTPUT_TYPE    ((uint32_t)0x00000010)  // Indicates OD

#define GPIO_MODE_INPUT     ((uint32_t)0x00000000)
#define GPIO_MODE_OUTPUT_PP ((uint32_t)0x00000001)
#define GPIO_MODE_OUTPUT_OD ((uint32_t)0x00000011)
#define GPIO_MODE_AF_PP     ((uint32_t)0x00000002)
#define GPIO_MODE_AF_OD     ((uint32_t)0x00000012)
#define GPIO_MODE_ANALOG    ((uint32_t)0x00000003)
#define GPIO_MODE_IT_RISING ((uint32_t)1)
#define GPIO_MODE_IT_FALLING ((uint32_t)2)

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT)              ||\
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          ||\
                            ((MODE) == GPIO_MODE_OUTPUT_OD)          ||\
                            ((MODE) == GPIO_MODE_AF_PP)              ||\
                            ((MODE) == GPIO_MODE_AF_OD)              ||\
                            ((MODE) == GPIO_MODE_ANALOG))

#define GPIO_NOPULL         ((uint32_t)0)
#define GPIO_PULLUP         ((uint32_t)1)
#define GPIO_PULLDOWN       ((uint32_t)2)

#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))

#define GPIO_SPEED_FREQ_LOW       ((uint32_t)0)
#define GPIO_SPEED_FREQ_MEDIUM    ((uint32_t)1)
#define GPIO_SPEED_FREQ_HIGH      ((uint32_t)2)
#define GPIO_SPEED_FREQ_VERY_HIGH ((uint32_t)3)

#define IS_GPIO_AF(af)      ((af) >= 0 && (af) <= 7)

typedef struct {
    uint32_t    Pin;
    uint32_t    Mode;
    uint32_t    Pull;
    uint32_t    Speed;
    uint32_t    Alternate;
} GPIO_InitTypeDef;

#define GPIO_PORT_TO_PORT_NUM(GPIOx) \
    ((&GPIOx->PDOR - &GPIOA_PDOR) / (&GPIOB_PDOR - &GPIOA_PDOR))

#define GPIO_PIN_TO_PORT_PCR(GPIOx, pin) \
    (&PORTA_PCR0 + (GPIO_PORT_TO_PORT_NUM(GPIOx) * 0x400) + (pin))

#define GPIO_AF2_I2C0   2
#define GPIO_AF2_I2C1   2
#define GPIO_AF2_SPI0   2
#define GPIO_AF3_FTM0   3
#define GPIO_AF3_FTM1   3
#define GPIO_AF3_FTM2   3
#define GPIO_AF3_UART0  3
#define GPIO_AF3_UART1  3
#define GPIO_AF3_UART2  3
#define GPIO_AF4_FTM0   4
#define GPIO_AF6_FTM1   6
#define GPIO_AF6_FTM2   6
#define GPIO_AF6_I2C1   6
#define GPIO_AF7_FTM1   7

__attribute__(( always_inline )) static inline void __WFI(void) {
  __asm volatile ("wfi");
}

void mp_hal_set_interrupt_char(int c);

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init);

struct _pin_obj_t;
#define mp_hal_pin_obj_t const struct _pin_obj_t*
#define mp_hal_pin_high(p) (((p)->gpio->PSOR) = (p)->pin_mask)
#define mp_hal_pin_low(p)  (((p)->gpio->PCOR) = (p)->pin_mask)
#define mp_hal_pin_read(p) (((p)->gpio->PDIR >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)
