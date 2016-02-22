Example Description

This example describes how to use SPI read/write by mbed api.


The SPI Interface provides a "Serial Peripheral Interface" Master.

This interface can be used for communication with SPI slave devices,
such as FLASH memory, LCD screens and other modules or integrated circuits.

In this example, it use 2 sets of SPI. One is master, the other is slave.
By default it use SPI0 as slave, and use SPI2 as master.
So we connect them as below:
    Connect SPI0_MOSI (PC_2) to SPI2_MOSI (PA_1)
    Connect SPI0_MISO (PC_3) to SPI2_MISO (PA_0)
    Connect SPI0_SCLK (PC_1) to SPI2_SCLK (PA_2)
    Connect SPI0_CS   (PC_0) to SPI2_CS   (PA_4)

Because some GPIOA are used as SDIO purpose which has higher priority.
So we need pull high PA_7 when device boot up.
    Connect PA_7 to 3V3

After boot up, the master will send data to slave and shows result on LOG_OUT.