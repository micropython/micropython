#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "usb.h"
#include "uart.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = MP_EIO,
    [HAL_BUSY] = MP_EBUSY,
    [HAL_TIMEOUT] = MP_ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    mp_raise_OSError(mp_hal_status_to_errno_table[status]);
}

MP_WEAK int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
        }
        int dupterm_c = mp_uos_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

MP_WEAK void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    mp_uos_dupterm_tx_strn(str, len);
}

// Efficiently convert "\n" to "\r\n"
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    const char *last = str;
    while (len--) {
        if (*str == '\n') {
            if (str > last) {
                mp_hal_stdout_tx_strn(last, str - last);
            }
            mp_hal_stdout_tx_strn("\r\n", 2);
            ++str;
            last = str;
        } else {
            ++str;
        }
    }
    if (str > last) {
        mp_hal_stdout_tx_strn(last, str - last);
    }
}

void mp_hal_ticks_cpu_enable(void) {
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {
    uint32_t gpio_idx = ((uint32_t)gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    RCC->APB2ENR |= 1 << (RCC_APB2ENR_IOPAEN_Pos + gpio_idx);
    volatile uint32_t tmp = RCC->APB2ENR; // Delay after enabling clock
    (void)tmp;
}

#define MP_HAL_PIN_MODE_INVALID    (4)
// this table converts from MP_HAL_PIN_MODE_xxx to STM32F1 CRL/CRH value, output use highest speed
const byte mp_hal_pin_mod_cfg[7] = {
	[MP_HAL_PIN_MODE_INPUT]          = 0x04, // floating input
	[MP_HAL_PIN_MODE_OUTPUT]         = 0x03, // output PP, 50MHz
	[MP_HAL_PIN_MODE_ALT]            = 0x0b, // alternat PP, 50MHz
	[MP_HAL_PIN_MODE_ANALOG]         = 0x00, // analog input
	[MP_HAL_PIN_MODE_INVALID]        = 0x00, // not used
	[MP_HAL_PIN_MODE_OPEN_DRAIN]     = 0x07, // output OD, 50MHz
	[MP_HAL_PIN_MODE_ALT_OPEN_DRAIN] = 0x0f, // alternat OD, 50MHz
};

void mp_hal_pin_config(mp_hal_pin_obj_t pin_obj, uint32_t mode, uint32_t pull, uint32_t alt) {
    GPIO_TypeDef *gpio = pin_obj->gpio;
    uint32_t pin = pin_obj->pin;
    mp_hal_gpio_clock_enable(gpio);
	uint32_t moder = mp_hal_pin_mod_cfg[mode];

	// 带上拉/下拉的输入
	if ( (moder & 3) == 0  && pull ) {
		moder = 0x08U;
	}

    // 获取需要修改的寄存器 CRL/CRH (pin0~pin7 use CRL, pin8~pin15 use CRH)
    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin / 7 * 4));
	
	// pin 配置占4bit, 前2bit是CNF， 后2bit是MODE
	// 对于CRL, mask = 0x0f << (pin * 4)
	// 对于CRH, mask = 0x0f << ( (pin - 8) * 4 )
	// 以上可统一为: mask = 0x0f << ( (pin % 8) * 4 )
	*pReg = (*pReg & ~(0x0f << ( (pin%8) * 4))) | ( (moder & 0x0f) << ( (pin%8) * 4) );

	// 设置上拉下拉电阻(输入有效)
	if (moder == 0 && pull > 0) {
		gpio->ODR = (gpio->ODR & ~(1 << pin)) | ( (pull%2) << pin);
	}
	
	// 设置AFIO
	if (mode == MP_HAL_PIN_MODE_ALT || mode == MP_HAL_PIN_MODE_ALT_OPEN_DRAIN) {
		// 1. alt = 0, 及原先基本功能， 什么也不做(需要设为输出？)
		// 2. alt = 15, 设为外部中断线
		// 3. 其他的保持默认, 无需指定
		
		// 4. 有管脚映射或部分映射 如何设置?(存储AFIO->MAPRx set mask 到alt中？)
	}
}

bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit) {
    const pin_af_obj_t *af = pin_find_af(pin, fn, unit);
    if (af == NULL) {
        return false;
    }
    mp_hal_pin_config(pin, mode, pull, af->idx);
    return true;
}

void mp_hal_pin_config_speed(mp_hal_pin_obj_t pin_obj, uint32_t speed) {
    GPIO_TypeDef *gpio = pin_obj->gpio;
    uint32_t pin = pin_obj->pin;
	// FIXME: 这里需要保证是输出模式，否则会把输入变成输出
    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin / 7 * 4));
    *pReg = (*pReg & ~(0x03 << ( (pin%8) * 4))) | ( ( (speed+1) & 0x03) << ( (pin%8) *4) );
}

/*******************************************************************************/
// MAC address

typedef struct _pyb_otp_t {
    uint16_t series;
    uint16_t rev;
    uint8_t mac[6];
} pyb_otp_t;

#define OTP_ADDR (0x1ff0f3c0)
#define OTP ((pyb_otp_t*)OTP_ADDR)

MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    // Check if OTP region has a valid MAC address, and use it if it does
    if (OTP->series == 0x00d1 && OTP->mac[0] == 'H' && OTP->mac[1] == 'J' && OTP->mac[2] == '0') {
        memcpy(buf, OTP->mac, 6);
        buf[5] += idx;
        return;
    }

    // Generate a random locally administered MAC address (LAA)
    uint8_t *id = (uint8_t *)MP_HAL_UNIQUE_ID_ADDRESS;
    buf[0] = 0x02; // LAA range
    buf[1] = (id[11] << 4) | (id[10] & 0xf);
    buf[2] = (id[9] << 4) | (id[8] & 0xf);
    buf[3] = (id[7] << 4) | (id[6] & 0xf);
    buf[4] = id[2];
    buf[5] = (id[0] << 2) | idx;
}

void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    mp_hal_get_mac(idx, mac);
    for (; chr_len; ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
}
