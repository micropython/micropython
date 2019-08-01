// We use the ST Cube HAL library for most hardware peripherals
#include "pin.h"
#include <stm32f1xx_hal.h>

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c);  // -1 to disable

// timing functions

#include "irq.h"

#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

void mp_hal_ticks_cpu_enable(void);

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        mp_hal_ticks_cpu_enable();
    }
    return DWT->CYCCNT;
}

// C-level pin HAL

#include "pin.h"

#define MP_HAL_PIN_FMT "%q"
#define MP_HAL_PIN_MODE_INPUT (0)           // 浮空输入
#define MP_HAL_PIN_MODE_OUTPUT (1)          // 输出(推挽)
#define MP_HAL_PIN_MODE_ALT (2)             // 功能复用(推挽)
#define MP_HAL_PIN_MODE_ANALOG (3)          // 模拟(输入)
#define MP_HAL_PIN_MODE_ADC (3)             // 模拟(输入)
#define MP_HAL_PIN_MODE_OPEN_DRAIN (5)      // 开漏(输出)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN (6)  // 复用开漏(输出)
#define MP_HAL_PIN_PULL_NONE (GPIO_NOPULL)
#define MP_HAL_PIN_PULL_UP (GPIO_PULLUP)
#define MP_HAL_PIN_PULL_DOWN (GPIO_PULLDOWN)
#define MP_HAL_PIN_SPEED_LOW (GPIO_SPEED_FREQ_LOW)
#define MP_HAL_PIN_SPEED_MEDIUM (GPIO_SPEED_FREQ_MEDIUM)
#define MP_HAL_PIN_SPEED_HIGH (GPIO_SPEED_FREQ_HIGH)

#define mp_hal_pin_obj_t const pin_obj_t*
#define mp_hal_get_pin_obj(o) pin_find(o)
#define mp_hal_pin_name(p) ((p)->name)
#define mp_hal_pin_input(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_output(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, 0)

#define mp_hal_pin_high(p) (((p)->gpio->BSRR) = (p)->pin_mask)
#define mp_hal_pin_low(p) (((p)->gpio->BSRR) = ((p)->pin_mask << 16))
#define mp_hal_pin_od_low(p) mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p) mp_hal_pin_high(p)
#define mp_hal_pin_read(p) (((p)->gpio->IDR >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)                                                                                         \
    do {                                                                                                               \
        if (v) {                                                                                                       \
            mp_hal_pin_high(p);                                                                                        \
        } else {                                                                                                       \
            mp_hal_pin_low(p);                                                                                         \
        }                                                                                                              \
    } while (0)

/**
 * 打开GPIOx时钟
 */
void mp_hal_gpio_clock_enable(GPIO_TypeDef* gpio);

/**
 * 配置管脚
 * pin_obj - 管脚对象
 * mode    - IO模式
 * pull    - 上拉/下拉 (F1中对输出无效)
 * alt     - pin_af_obj_t.idx 字段, 表示复用索引
 */
void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt);

/**
 * 配置Pin复用功能
 * pin  - Pin对象定义
 * mode - IO模式
 * pull - 上拉/下拉 (F1中对输出无效)
 * fn   - 参考ports/stm32f1/pin_defs_stm32.h, 标识唯一外设种类(TIM, UART, USART, SPI等)
 * unit - 外设序号, 例如SPI2, 序号是2
 */
bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit);

/**
 * 配置Pin复用功能
 * pin   - Pin对象定义
 * speed - 接口翻转速度(F1中输出有效)
 */
void mp_hal_pin_config_speed(mp_hal_pin_obj_t pin_obj, uint32_t speed);

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_WLAN1,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
};

void mp_hal_get_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char* dest);
