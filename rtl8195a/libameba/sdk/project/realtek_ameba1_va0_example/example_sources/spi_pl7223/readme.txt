Example Description

This example describes how to get data from pl7223 by SPI conneciton

The SPI Interface provides a "Serial Peripheral Interface" Master.

Hardware connection:
    Connect SPI0_MOSI (PC_2) to PL7223 MOSI
    Connect SPI0_MISO (PC_3) to PL7223 MISO
    Connect SPI0_SCLK (PC_1) to PL7223 SCLK
    Connect GPIOB_5   (PB_5) to PL7223 CS
    Connect GPIOB_4   (PB_4) to PL7223 RESET
    Connect GROUND together

    Connect to LOG UART with configuration 38400 8bits, 1 stopbit, no parity


After boot up, the ameba will reset pl7223 into MCU mode and get data from pl7223.
After Gatherin and calculating, program will show information to UART.