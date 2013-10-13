#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include "std.h"

#include "font_petme128_8x8.h"

void delay_ms(int ms);

void impl02_c_version() {
    int x = 0;
    while (x < 400) {
        int y = 0;
        while (y < 400) {
            volatile int z = 0;
            while (z < 400) {
                z = z + 1;
            }
            y = y + 1;
        }
        x = x + 1;
    }
}

void set_bits(__IO uint32_t *addr, uint32_t shift, uint32_t mask, uint32_t value) {
    uint32_t x = *addr;
    x &= ~(mask << shift);
    x |= (value << shift);
    *addr = x;
}

void gpio_init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOAEN;
}

#define PYB_LEDR_PORT (GPIOA)
#define PYB_LEDR1_PORT_NUM (8)
#define PYB_LEDR2_PORT_NUM (10)
#define PYB_LEDG_PORT (GPIOC)
#define PYB_LEDG1_PORT_NUM (4)
#define PYB_LEDG2_PORT_NUM (5)

void gpio_pin_init(GPIO_TypeDef *gpio, uint32_t pin, uint32_t moder, uint32_t otyper, uint32_t ospeedr, uint32_t pupdr) {
    set_bits(&gpio->MODER, 2 * pin, 3, moder);
    set_bits(&gpio->OTYPER, pin, 1, otyper);
    set_bits(&gpio->OSPEEDR, 2 * pin, 3, ospeedr);
    set_bits(&gpio->PUPDR, 2 * pin, 3, pupdr);
}

void gpio_pin_af(GPIO_TypeDef *gpio, uint32_t pin, uint32_t af) {
    // set the AF bits for the given pin
    // pins 0-7 use low word of AFR, pins 8-15 use high word
    set_bits(&gpio->AFR[pin >> 3], 4 * (pin & 0x07), 0xf, af);
}

void mma_init() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable I2C1
    gpio_pin_init(GPIOB, 6 /* B6 is SCL */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    gpio_pin_init(GPIOB, 7 /* B7 is SDA */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    gpio_pin_af(GPIOB, 6, 4 /* AF 4 for I2C1 */);
    gpio_pin_af(GPIOB, 7, 4 /* AF 4 for I2C1 */);

    // get clock speeds
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);

    // disable the I2C peripheral before we configure it
    I2C1->CR1 &= ~I2C_CR1_PE;

    // program peripheral input clock
    I2C1->CR2 = 4; // no interrupts; 4 MHz (hopefully!) (could go up to 42MHz)

    // configure clock control reg
    uint32_t freq = rcc_clocks.PCLK1_Frequency / (100000 << 1); // want 100kHz, this is the formula for freq
    I2C1->CCR = freq; // standard mode (speed), freq calculated as above

    // configure rise time reg
    I2C1->TRISE = (rcc_clocks.PCLK1_Frequency / 1000000) + 1; // formula for trise, gives maximum rise time

    // enable the I2C peripheral
    I2C1->CR1 |= I2C_CR1_PE;

    // set START bit in CR1 to generate a start cond!
}

uint32_t i2c_get_sr() {
    // must read SR1 first, then SR2, as the read can clear some flags
    uint32_t sr1 = I2C1->SR1;
    uint32_t sr2 = I2C1->SR2;
    return (sr2 << 16) | sr1;
}

void mma_restart(uint8_t addr, int write) {
    // send start condition
    I2C1->CR1 |= I2C_CR1_START;

    // wait for BUSY, MSL and SB --> Slave has acknowledged start condition
    while ((i2c_get_sr() & 0x00030001) != 0x00030001) {
    }

    if (write) {
        // send address and write bit
        I2C1->DR = (addr << 1) | 0;
        // wait for BUSY, MSL, ADDR, TXE and TRA
        while ((i2c_get_sr() & 0x00070082) != 0x00070082) {
        }
    } else {
        // send address and read bit
        I2C1->DR = (addr << 1) | 1;
        // wait for BUSY, MSL and ADDR flags
        while ((i2c_get_sr() & 0x00030002) != 0x00030002) {
        }
    }
}

void mma_start(uint8_t addr, int write) {
    // wait until I2C is not busy
    while (I2C1->SR2 & I2C_SR2_BUSY) {
    }

    // do rest of start
    mma_restart(addr, write);
}

void mma_send_byte(uint8_t data) {
    // send byte
    I2C1->DR = data;
    // wait for TRA, BUSY, MSL, TXE and BTF (byte transmitted)
    int timeout = 1000000;
    while ((i2c_get_sr() & 0x00070084) != 0x00070084) {
        if (timeout-- <= 0) {
            printf("mma_send_byte timed out!\n");
            break;
        }
    }
}

uint8_t mma_read_ack() {
    // enable ACK of received byte
    I2C1->CR1 |= I2C_CR1_ACK;
    // wait for BUSY, MSL and RXNE (byte received)
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

uint8_t mma_read_nack() {
    // disable ACK of received byte (to indicate end of receiving)
    I2C1->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
    // last byte should apparently also generate a stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
    // wait for BUSY, MSL and RXNE (byte received)
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

void mma_stop() {
    // send stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
}

void led_init() {
    // set the output high (so LED is off)
    PYB_LEDR_PORT->BSRRL = 1 << PYB_LEDR1_PORT_NUM;
    PYB_LEDR_PORT->BSRRL = 1 << PYB_LEDR2_PORT_NUM;
    PYB_LEDG_PORT->BSRRL = 1 << PYB_LEDG1_PORT_NUM;
    PYB_LEDG_PORT->BSRRL = 1 << PYB_LEDG2_PORT_NUM;
    // make it an open drain output
    gpio_pin_init(PYB_LEDR_PORT, PYB_LEDR1_PORT_NUM, 1, 1, 0, 0);
    gpio_pin_init(PYB_LEDR_PORT, PYB_LEDR2_PORT_NUM, 1, 1, 0, 0);
    gpio_pin_init(PYB_LEDG_PORT, PYB_LEDG1_PORT_NUM, 1, 1, 0, 0);
    gpio_pin_init(PYB_LEDG_PORT, PYB_LEDG2_PORT_NUM, 1, 1, 0, 0);
}

static void led_state(uint32_t led_port, int s) {
    if (s == 0) {
        // LED off, output is high
        if (led_port == PYB_LEDR1_PORT_NUM || led_port == PYB_LEDR2_PORT_NUM) {
            PYB_LEDR_PORT->BSRRL = 1 << led_port;
        } else {
            PYB_LEDG_PORT->BSRRL = 1 << led_port;
        }
    } else {
        // LED on, output is low
        if (led_port == PYB_LEDR1_PORT_NUM || led_port == PYB_LEDR2_PORT_NUM) {
            PYB_LEDR_PORT->BSRRH = 1 << led_port;
        } else {
            PYB_LEDG_PORT->BSRRH = 1 << led_port;
        }
    }
}

#define PYB_USRSW_PORT (GPIOA)
#define PYB_USRSW_PORT_NUM (13)

void sw_init() {
    // make it an input with pull-up
    gpio_pin_init(PYB_USRSW_PORT, PYB_USRSW_PORT_NUM, 0, 0, 0, 1);
}

int sw_get() {
    if (PYB_USRSW_PORT->IDR & (1 << PYB_USRSW_PORT_NUM)) {
        // pulled high, so switch is not pressed
        return 0;
    } else {
        // pulled low, so switch is pressed
        return 1;
    }
}

#define PYB_LCD_PORT        (GPIOA)
#define PYB_LCD_CS1_PIN     (0)
#define PYB_LCD_RST_PIN     (1)
#define PYB_LCD_A0_PIN      (2)
#define PYB_LCD_SCL_PIN     (3)
#define PYB_LCD_SI_PIN      (4)

static void lcd_comm_out(uint8_t i) {
    delay_ms(0);
    PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_CS1_PIN; // CS=0; enable
    PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_A0_PIN; // A0=0; select instr reg
    // send byte bigendian, latches on rising clock
    for (uint32_t n = 0; n < 8; n++) {
        delay_ms(0);
        PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_SCL_PIN; // SCL=0
        if ((i & 0x80) == 0) {
            PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_SI_PIN; // SI=0
        } else {
            PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SI_PIN; // SI=1
        }
        i <<= 1;
        delay_ms(0);
        PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SCL_PIN; // SCL=1
    }
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_CS1_PIN; // CS=1; disable

    /*
    in Python, native types:
    CS1_PIN(const) = 0
    n = int(0)
    delay_ms(0)
    PORT[word:BSRRH] = 1 << CS1_PIN
    for n in range(0, 8):
        delay_ms(0)
        PORT[word:BSRRH] = 1 << SCL_PIN
        if i & 0x80 == 0:
            PORT[word:BSRRH] = 1 << SI_PIN
        else:
            PORT[word:BSRRL] = 1 << SI_PIN
        i <<= 1
        delay_ms(0)
        PORT[word:BSRRL] = 1 << SCL_PIN
    */
}

static void lcd_data_out(uint8_t i) {
    delay_ms(0);
    PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_CS1_PIN; // CS=0; enable
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_A0_PIN; // A0=1; select data reg
    // send byte bigendian, latches on rising clock
    for (uint32_t n = 0; n < 8; n++) {
        delay_ms(0);
        PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_SCL_PIN; // SCL=0
        if ((i & 0x80) == 0) {
            PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_SI_PIN; // SI=0
        } else {
            PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SI_PIN; // SI=1
        }
        i <<= 1;
        delay_ms(0);
        PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SCL_PIN; // SCL=1
    }
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_CS1_PIN; // CS=1; disable
}

#define LCD_BUF_W (16)
#define LCD_BUF_H (4)
char lcd_buffer[LCD_BUF_W * LCD_BUF_H];
int lcd_line;
int lcd_column;
int lcd_next_line;

void lcd_print_strn(const char *str, unsigned int len) {
    int redraw_min = lcd_line * LCD_BUF_W + lcd_column;
    int redraw_max = redraw_min;
    int did_new_line = 0;
    for (; len > 0; len--, str++) {
        // move to next line if needed
        if (lcd_next_line) {
            if (lcd_line + 1 < LCD_BUF_H) {
                lcd_line += 1;
            } else {
                lcd_line = LCD_BUF_H - 1;
                for (int i = 0; i < LCD_BUF_W * (LCD_BUF_H - 1); i++) {
                    lcd_buffer[i] = lcd_buffer[i + LCD_BUF_W];
                }
                for (int i = 0; i < LCD_BUF_W; i++) {
                    lcd_buffer[LCD_BUF_W * (LCD_BUF_H - 1) + i] = ' ';
                }
                redraw_min = 0;
                redraw_max = LCD_BUF_W * LCD_BUF_H;
            }
            lcd_next_line = 0;
            lcd_column = 0;
            did_new_line = 1;
        }
        if (*str == '\n') {
            lcd_next_line = 1;
        } else if (lcd_column >= LCD_BUF_W) {
            lcd_next_line = 1;
            str -= 1;
            len += 1;
        } else {
            lcd_buffer[lcd_line * LCD_BUF_W + lcd_column] = *str;
            lcd_column += 1;
            int max = lcd_line * LCD_BUF_W + lcd_column;
            if (max > redraw_max) {
                redraw_max = max;
            }
        }
    }

    int last_page = -1;
    for (int i = redraw_min; i < redraw_max; i++) {
        int page = i / LCD_BUF_W;
        if (page != last_page) {
            int offset = 8 * (i - (page * LCD_BUF_W));
            lcd_comm_out(0xb0 | page); // page address set
            lcd_comm_out(0x10 | ((offset >> 4) & 0x0f)); // column address set upper
            lcd_comm_out(0x00 | (offset & 0x0f)); // column address set lower
            last_page = page;
        }
        int chr = lcd_buffer[i];
        if (chr < 32 || chr > 126) {
            chr = 127;
        }
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        for (int i = 0; i < 8; i++) {
            lcd_data_out(chr_data[i]);
        }
    }

    if (did_new_line) {
        delay_ms(200);
    }
}

static void lcd_init() {
    // set the outputs high
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_CS1_PIN;
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_RST_PIN;
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_A0_PIN;
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SCL_PIN;
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_SI_PIN;
    // make them push/pull outputs
    gpio_pin_init(PYB_LCD_PORT, PYB_LCD_CS1_PIN, 1, 0, 0, 0);
    gpio_pin_init(PYB_LCD_PORT, PYB_LCD_RST_PIN, 1, 0, 0, 0);
    gpio_pin_init(PYB_LCD_PORT, PYB_LCD_A0_PIN, 1, 0, 0, 0);
    gpio_pin_init(PYB_LCD_PORT, PYB_LCD_SCL_PIN, 1, 0, 0, 0);
    gpio_pin_init(PYB_LCD_PORT, PYB_LCD_SI_PIN, 1, 0, 0, 0);

    // init the LCD
    delay_ms(1); // wait a bit
    PYB_LCD_PORT->BSRRH = 1 << PYB_LCD_RST_PIN; // RST=0; reset
    delay_ms(1); // wait for reset; 2us min
    PYB_LCD_PORT->BSRRL = 1 << PYB_LCD_RST_PIN; // RST=1; enable
    delay_ms(1); // wait for reset; 2us min
    lcd_comm_out(0xa0); // ADC select, normal
    lcd_comm_out(0xc8); // common output mode select, reverse
    lcd_comm_out(0xa2); // LCD bias set, 1/9 bias
    lcd_comm_out(0x2f); // power control set, 0b111=(booster on, vreg on, vfollow on)
    lcd_comm_out(0x21); // v0 voltage regulator internal resistor ratio set, 0b001=small
    lcd_comm_out(0x81); // electronic volume mode set
    lcd_comm_out(0x34); // electronic volume register set, 0b110100
    lcd_comm_out(0x40); // display start line set, 0
    lcd_comm_out(0xaf); // LCD display, on

    // clear display
    for (int page = 0; page < 4; page++) {
        lcd_comm_out(0xb0 | page); // page address set
        lcd_comm_out(0x10); // column address set upper
        lcd_comm_out(0x00); // column address set lower
        for (int i = 0; i < 128; i++) {
            lcd_data_out(0x00);
        }
    }

    for (int i = 0; i < LCD_BUF_H * LCD_BUF_W; i++) {
        lcd_buffer[i] = ' ';
    }
    lcd_line = 0;
    lcd_column = 0;
    lcd_next_line = 0;
}

void __fatal_error(const char *msg) {
    lcd_print_strn("\nFATAL ERROR:\n", 14);
    lcd_print_strn(msg, strlen(msg));

    for (;;) {
        led_state(PYB_LEDR1_PORT_NUM, 1);
        led_state(PYB_LEDR2_PORT_NUM, 0);
        delay_ms(150);
        led_state(PYB_LEDR1_PORT_NUM, 0);
        led_state(PYB_LEDR2_PORT_NUM, 1);
        delay_ms(150);
    }
}

#include "misc.h"
#include "lexer.h"
#include "mpyconfig.h"
#include "parse.h"
#include "compile.h"
#include "runtime.h"

/*
py_obj_t pyb_delay(py_obj_t count) {
    delay_ms(rt_get_int(count));
    return py_const_none;
}

py_obj_t pyb_led(py_obj_t state) {
    led_state(PYB_LEDG1_PORT_NUM, rt_is_true(state));
    return state;
}

py_obj_t pyb_sw() {
    if (sw_get()) {
        return py_const_true;
    } else {
        return py_const_false;
    }
}
*/

#include "asmthumb.h"
typedef void (*fun_t)();

#include "ff.h"
FATFS fatfs0;

int main() {
    // should disable JTAG

    //qstr_init();
    //rt_init();

    gpio_init();
    led_init();
    sw_init();
    lcd_init();

    // print a message
    printf(" micro py board\n");

    // flash to indicate we are alive!
    for (int i = 0; i < 2; i++) {
        led_state(PYB_LEDR1_PORT_NUM, 1);
        led_state(PYB_LEDR2_PORT_NUM, 0);
        delay_ms(200);
        led_state(PYB_LEDR1_PORT_NUM, 0);
        led_state(PYB_LEDR2_PORT_NUM, 1);
        delay_ms(200);
    }

    led_state(PYB_LEDR1_PORT_NUM, 0);
    led_state(PYB_LEDR2_PORT_NUM, 0);
    led_state(PYB_LEDG1_PORT_NUM, 0);
    led_state(PYB_LEDG2_PORT_NUM, 0);

    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    /*
    {
        RCC_ClocksTypeDef rcc_clocks;
        RCC_GetClocksFreq(&rcc_clocks);
        printf("S=%lu H=%lu P1=%lu P2=%lu\n", rcc_clocks.SYSCLK_Frequency, rcc_clocks.HCLK_Frequency, rcc_clocks.PCLK1_Frequency, rcc_clocks.PCLK2_Frequency);
        delay_ms(1000);
    }
    */

    /*
    extern int _sidata;
    extern int _sdata;
    extern int _edata;
    extern int _sbss;
    extern int _ebss;
    delay_ms(2000);
    printf("_sidata=%04x\n", _sidata);
    printf("_sdata=%04x\n", _sdata);
    printf("_edata=%04x\n", _edata);
    printf("_sbss=%04x\n", _sbss);
    printf("_ebss=%04x\n", _ebss);
    //printf("sizeof(int)=%d\n", sizeof(int)); // 4
    delay_ms(2000);
    */

    //printf("init;al=%u\n", m_get_total_bytes_allocated()); // 1600, due to qstr_init
    //delay_ms(1000);

    #if 0
    // Python!
    if (0) {
        //const char *pysrc = "def f():\n  x=x+1\nprint(42)\n";
        const char *pysrc =
            // impl01.py
            /*
            "x = 0\n"
            "while x < 400:\n"
            "    y = 0\n"
            "    while y < 400:\n"
            "        z = 0\n"
            "        while z < 400:\n"
            "            z = z + 1\n"
            "        y = y + 1\n"
            "    x = x + 1\n";
            */
            // impl02.py
            "#@micropython.native\n"
            "def f():\n"
            "    x = 0\n"
            "    while x < 400:\n"
            "        y = 0\n"
            "        while y < 400:\n"
            "            z = 0\n"
            "            while z < 400:\n"
            "                z = z + 1\n"
            "            y = y + 1\n"
            "        x = x + 1\n"
            "f()\n";
            /*
            "print('in python!')\n"
            "x = 0\n"
            "while x < 4:\n"
            "    pyb_led(True)\n"
            "    pyb_delay(201)\n"
            "    pyb_led(False)\n"
            "    pyb_delay(201)\n"
            "    x = x + 1\n"
            "print('press me!')\n"
            "while True:\n"
            "    pyb_led(pyb_sw())\n";
            */
            /*
            // impl16.py
            "@micropython.asm_thumb\n"
            "def delay(r0):\n"
            "    b(loop_entry)\n"
            "    label(loop1)\n"
            "    movw(r1, 55999)\n"
            "    label(loop2)\n"
            "    subs(r1, r1, 1)\n"
            "    cmp(r1, 0)\n"
            "    bgt(loop2)\n"
            "    subs(r0, r0, 1)\n"
            "    label(loop_entry)\n"
            "    cmp(r0, 0)\n"
            "    bgt(loop1)\n"
            "print('in python!')\n"
            "@micropython.native\n"
            "def flash(n):\n"
            "    x = 0\n"
            "    while x < n:\n"
            "        pyb_led(True)\n"
            "        delay(249)\n"
            "        pyb_led(False)\n"
            "        delay(249)\n"
            "        x = x + 1\n"
            "flash(20)\n";
            */

        py_lexer_t *lex = py_lexer_from_str_len("<>", pysrc, strlen(pysrc), false);

        if (0) {
            while (!py_lexer_is_kind(lex, PY_TOKEN_END)) {
                py_token_show(py_lexer_cur(lex));
                py_lexer_to_next(lex);
                delay_ms(1000);
            }
        } else {
            // nalloc=1740;6340;6836 -> 140;4600;496 bytes for lexer, parser, compiler
            printf("lex; al=%u\n", m_get_total_bytes_allocated());
            delay_ms(1000);
            py_parse_node_t pn = py_parse(lex, 0);
            //printf("----------------\n");
            printf("pars;al=%u\n", m_get_total_bytes_allocated());
            delay_ms(1000);
            //parse_node_show(pn, 0);
            py_compile(pn);
            printf("comp;al=%u\n", m_get_total_bytes_allocated());
            delay_ms(1000);

            if (1) {
                // execute it!

                // add some functions to the python namespace
                rt_store_name(qstr_from_str_static("pyb_delay"), rt_make_function_1(pyb_delay));
                rt_store_name(qstr_from_str_static("pyb_led"), rt_make_function_1(pyb_led));
                rt_store_name(qstr_from_str_static("pyb_sw"), rt_make_function_0(pyb_sw));

                py_obj_t module_fun = rt_make_function_from_id(1);

                led_state(PYB_LEDG1_PORT_NUM, 1);
                delay_ms(100);
                led_state(PYB_LEDG1_PORT_NUM, 0);
                py_obj_t ret = rt_call_function_0(module_fun);
                led_state(PYB_LEDG1_PORT_NUM, 1);
                delay_ms(100);
                led_state(PYB_LEDG1_PORT_NUM, 0);

                printf("done! got: ");
                py_obj_print(ret);
                printf("\n");
                delay_ms(1000);
                printf("nalloc=%u\n", m_get_total_bytes_allocated());
                delay_ms(1000);
            }
        }
    }
    #endif

    // benchmark C version of impl02.py
    if (0) {
        led_state(PYB_LEDG1_PORT_NUM, 1);
        delay_ms(100);
        led_state(PYB_LEDG1_PORT_NUM, 0);
        impl02_c_version();
        led_state(PYB_LEDG1_PORT_NUM, 1);
        delay_ms(100);
        led_state(PYB_LEDG1_PORT_NUM, 0);
    }

    // MMA testing
    if (0) {
        printf("1");
        mma_init();
        printf("2");
        mma_start(0x4c, 1);
        printf("3");
        mma_send_byte(0);
        printf("4");
        mma_stop();
        printf("5");
        mma_start(0x4c, 1);
        printf("6");
        mma_send_byte(0);
        printf("7");
        mma_restart(0x4c, 0);
        for (int i = 0; i <= 0xa; i++) {
            int data;
            if (i == 0xa) {
                data = mma_read_nack();
            } else {
                data = mma_read_ack();
            }
            printf(" %02x", data);
        }
        printf("\n");

        mma_start(0x4c, 1);
        mma_send_byte(7); // mode
        mma_send_byte(1); // active mode
        mma_stop();

        for (;;) {
            delay_ms(500);

            mma_start(0x4c, 1);
            mma_send_byte(0);
            mma_restart(0x4c, 0);
            for (int i = 0; i <= 3; i++) {
                int data;
                if (i == 3) {
                    data = mma_read_nack();
                    printf(" %02x\n", data);
                } else {
                    data = mma_read_ack() & 0x3f;
                    if (data & 0x20) {
                        data |= 0xc0;
                    }
                    printf(" % 2d", data);
                }
            }
        }
    }

    // fatfs testing
    if (1) {
        FRESULT res = f_mount(&fatfs0, "0:", 1);
        if (res == FR_OK) {
            printf("mount success\n");
        } else if (res == FR_NO_FILESYSTEM) {
            res = f_mkfs("0:", 0, 0);
            if (res == FR_OK) {
                printf("mkfs success\n");
            } else {
                printf("mkfs fail %d\n", res);
            }
        } else {
            printf("mount fail %d\n", res);
        }

        // write a file
        if (0) {
            FIL fp;
            f_open(&fp, "0:/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, "# this is boot.py\n", 18, &n);
            printf("wrote %d\n", n);
            f_close(&fp);
        }

        // read a file
        if (0) {
            FIL fp;
            f_open(&fp, "0:/boot.py", FA_READ);
            UINT n;
            char buf[20];
            f_read(&fp, buf, 18, &n);
            buf[n + 1] = 0;
            printf("read %d\n%s", n, buf);
            f_close(&fp);
        }

        DWORD nclst;
        FATFS *fatfs;
        f_getfree("0:", &nclst, &fatfs);
        printf("free=%d\n", nclst * fatfs->csize * 512);

    }

    // SD card testing
    if (0) {
        //sdio_init();
    }

    // USB VCP testing
    if (0) {
        //usb_vcp_init();
    }

    // USB testing
    if (1) {
        void usb_init();
        usb_init();
    }

    int i = 0;
    int n = 0;

    for (;;) {
        delay_ms(10);
        if (sw_get()) {
            led_state(PYB_LEDR1_PORT_NUM, 1);
            led_state(PYB_LEDG1_PORT_NUM, 0);
            i = 1 - i;
            if (i) {
                printf(" angel %05x.\n", n);
                //usb_vcp_send("hello!\r\n", 8);
            } else {
                printf(" mishka %4u.\n", n);
                //usb_vcp_send("angel!\r\n", 8);
            }
            n += 1;
        } else {
            led_state(PYB_LEDR1_PORT_NUM, 0);
            led_state(PYB_LEDG1_PORT_NUM, 1);
        }
    }

    return 0;
}

/*
void testf() {
    testf(1, 2, 3);
    testf(1, 2, 3, 4);
    testf(1, 2, 3, 4, 5);
    testf(1, 2, 3, 4, 5, 6);
    testf(1, 2, 3, 4, 5, 6, 7);
}

int testg(int a, int b, int c, int d, int e) {
    return a + b + c + d + testh(e);
}

int testh(int x, byte *y) {
    return x + (y[-2] << 2);
}
*/

/*
void print_int(int x, int y, int z, int zz) {
    printf("I %x %x %x %x", x, y, z, zz);
    byte* ptr = (byte*)z;
    printf("\nP %02x %02x %02x %02x", ptr[-4], ptr[-3], ptr[-2], ptr[-1]);
    for (;;) {
    }
}
void print_int_0(int x) { printf("P0 %x", x); }
void print_int_1(int x) { printf("P1 %x", x); }
void print_int_2(int x) { printf("P2 %x", x); }
void print_int_3(int x) { printf("P3 %x", x); }
void print_int_4(int x) { printf("P4 %x", x); }

typedef struct _b_t {
    void (*m1)(void*, int);
    void (*m2)(void*, int);
} b_t;
typedef struct _a_t {
    b_t *b;
} a_t;
void b_m1(b_t*, int);
void b_m2(b_t*, int);
void f1(a_t *a) {
    a->b->m1(a->b, 2);
    a->b->m2(a->b, 4);
    b_m1(a->b, 2);
    b_m2(a->b, 4);
}
void b_m1(b_t *b, int x) {
    b->m1(b, x);
}
*/
