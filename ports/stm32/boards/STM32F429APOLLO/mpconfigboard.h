#define MICROPY_HW_BOARD_NAME       "STM32F429APOLLO"
#define MICROPY_HW_MCU_NAME         "STM32F429"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
// #define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
//#define MICROPY_HW_ENABLE_SERVO     (1)

//时钟系统配置函数
//Fvco=Fs*(plln/pllm);
//SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//SYSCLK:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

//外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
//得到:Fvco=25*(360/25)=360Mhz
//     SYSCLK=360/2=180Mhz
//     Fusb=360/8=45Mhz
// HSE is 8MHz -> 25MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (360)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (8)

#define MICROPY_HW_UART_REPL        PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD   115200

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
#define MICROPY_HW_UART3_TX     (pin_B10)
#define MICROPY_HW_UART3_RX     (pin_B11)
//#define MICROPY_HW_UART6_TX     (pin_C6)
//#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C buses
#define MICROPY_HW_I2C2_SCL (pin_H4)
#define MICROPY_HW_I2C2_SDA (pin_H5)

// SPI buses
// #define MICROPY_HW_SPI1_NSS     (pin_A4)
// #define MICROPY_HW_SPI1_SCK     (pin_A5)
// #define MICROPY_HW_SPI1_MISO    (pin_A6)
// #define MICROPY_HW_SPI1_MOSI    (pin_A7)
#if defined(USE_USB_HS_IN_FS)
// The HS USB uses B14 & B15 for D- and D+
#else
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)
#endif
// #define MICROPY_HW_SPI4_NSS     (pin_E11)
// #define MICROPY_HW_SPI4_SCK     (pin_E12)
// #define MICROPY_HW_SPI4_MISO    (pin_E13)
// #define MICROPY_HW_SPI4_MOSI    (pin_E14)
#define MICROPY_HW_SPI5_NSS     (pin_F6)
#define MICROPY_HW_SPI5_SCK     (pin_F7)
#define MICROPY_HW_SPI5_MISO    (pin_F8)
#define MICROPY_HW_SPI5_MOSI    (pin_F9)
// #define MICROPY_HW_SPI6_NSS     (pin_G8)
// #define MICROPY_HW_SPI6_SCK     (pin_G13)
// #define MICROPY_HW_SPI6_MISO    (pin_G12)
// #define MICROPY_HW_SPI6_MOSI    (pin_G14)

// CAN buses
#if defined(USE_USB_HS_IN_FS)
// The HS USB uses B14 & B15 for D- and D+
#else
#define MICROPY_HW_CAN1_TX (pin_A12)
#define MICROPY_HW_CAN1_RX (pin_A11)
//#define MICROPY_HW_CAN2_TX (pin_B13)
//#define MICROPY_HW_CAN2_RX (pin_B12)
#endif

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_INPUT)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B1) // red
#define MICROPY_HW_LED2             (pin_B0) // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config
// FS 相关：
// MICROPY_HW_USB_FS
// HS 相关：
// MICROPY_HW_USB_HS
// MICROPY_HW_USB_HS_IN_FS
// MICROPY_HW_USB_HS_ULPI_NXT
// MICROPY_HW_USB_HS_ULPI_DIR
// 与 FS/HS 无直接关系：
// MICROPY_HW_USB_IS_MULTI_OTG
// MICROPY_HW_USB_VBUS_DETECT_PIN
// MICROPY_HW_USB_OTG_ID_PIN
// MICROPY_HW_USB_CDC_NUM

// 参考micropython/ports/stm32/usbd_conf.c的 HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
#define MICROPY_HW_USB_FS              (1)

// SDRAM
// W9825G6KH : 256 / 8 = 32M byte ,  1 byte = 8 bit.
#define MICROPY_HW_SDRAM_SIZE  (256 / 8 * 1024 * 1024)  // 256 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST             (1)
#define MICROPY_HEAP_START  ((sdram_valid) ? sdram_start() : &_heap_start)
#define MICROPY_HEAP_END    ((sdram_valid) ? sdram_end() : &_heap_end)

// Timing configuration for 90 Mhz (11.90ns) of SD clock frequency (180Mhz/2)

// SDRAM时钟为HCLK/2=180M/2=90M=11.1ns
// 加载模式寄存器到激活时间的延迟为2个时钟周期
// 退出自刷新延迟为8个时钟周期
// 自刷新时间为6个时钟周期                  
// 行循环延迟为6个时钟周期
// 恢复延迟为2个时钟周期
// 行预充电延迟为2个时钟周期
// 行到列延迟为2个时钟周期
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (8)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (6)
#define MICROPY_HW_SDRAM_TIMING_TRC         (6)
#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

// SDRAM_Handler.Instance=FMC_SDRAM_DEVICE;                             //SDRAM在BANK5,6  
// SDRAM_Handler.Init.SDBank=FMC_SDRAM_BANK1;                           //第一个SDRAM BANK
// SDRAM_Handler.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_9;     //列数量
// SDRAM_Handler.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_13;          //行数量
// SDRAM_Handler.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;       //数据宽度为16位
// SDRAM_Handler.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;  //一共4个BANK
// SDRAM_Handler.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_3;               //CAS为3
// SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;//失能写保护
// SDRAM_Handler.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;           //SDRAM时钟为HCLK/2=180M/2=90M=11.1ns
// SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;                //使能突发
// SDRAM_Handler.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_1;            //读通道延时

#define MICROPY_HW_SDRAM_BURST_LENGTH       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        3
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    9
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       13
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      16
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_RPIPE_DELAY        1
// SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;                //使能突发
#define MICROPY_HW_SDRAM_RBURST             (1)
// SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;//失能写保护
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)
// SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//设置自刷新次数 
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)

// #define MICROPY_HW_FMC_SDCKE1   (pin_B5)
// #define MICROPY_HW_FMC_SDNE1    (pin_B6)

#define MICROPY_HW_FMC_SDCKE0   (pin_C3)
#define MICROPY_HW_FMC_SDNE0    (pin_C2)
#define MICROPY_HW_FMC_SDCLK    (pin_G8)
#define MICROPY_HW_FMC_SDNCAS   (pin_G15)
#define MICROPY_HW_FMC_SDNRAS   (pin_F11)
#define MICROPY_HW_FMC_SDNWE    (pin_C0)
#define MICROPY_HW_FMC_BA0      (pin_G4)
#define MICROPY_HW_FMC_BA1      (pin_G5)
#define MICROPY_HW_FMC_NBL0     (pin_E0)
#define MICROPY_HW_FMC_NBL1     (pin_E1)
#define MICROPY_HW_FMC_A0       (pin_F0)
#define MICROPY_HW_FMC_A1       (pin_F1)
#define MICROPY_HW_FMC_A2       (pin_F2)
#define MICROPY_HW_FMC_A3       (pin_F3)
#define MICROPY_HW_FMC_A4       (pin_F4)
#define MICROPY_HW_FMC_A5       (pin_F5)
#define MICROPY_HW_FMC_A6       (pin_F12)
#define MICROPY_HW_FMC_A7       (pin_F13)
#define MICROPY_HW_FMC_A8       (pin_F14)
#define MICROPY_HW_FMC_A9       (pin_F15)
#define MICROPY_HW_FMC_A10      (pin_G0)
#define MICROPY_HW_FMC_A11      (pin_G1)
#define MICROPY_HW_FMC_A12      (pin_G2)
#define MICROPY_HW_FMC_D0       (pin_D14)
#define MICROPY_HW_FMC_D1       (pin_D15)
#define MICROPY_HW_FMC_D2       (pin_D0)
#define MICROPY_HW_FMC_D3       (pin_D1)
#define MICROPY_HW_FMC_D4       (pin_E7)
#define MICROPY_HW_FMC_D5       (pin_E8)
#define MICROPY_HW_FMC_D6       (pin_E9)
#define MICROPY_HW_FMC_D7       (pin_E10)
#define MICROPY_HW_FMC_D8       (pin_E11)
#define MICROPY_HW_FMC_D9       (pin_E12)
#define MICROPY_HW_FMC_D10      (pin_E13)
#define MICROPY_HW_FMC_D11      (pin_E14)
#define MICROPY_HW_FMC_D12      (pin_E15)
#define MICROPY_HW_FMC_D13      (pin_D8)
#define MICROPY_HW_FMC_D14      (pin_D9)
#define MICROPY_HW_FMC_D15      (pin_D10)

