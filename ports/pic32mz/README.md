# PIC32MZ port

## Initial Config

The PIC32MZ uses fuses to set the initial state of the code, such as oscillator config (internal or external), the type of oscillator, Watchdog, Deadman timers, ..etc. All these config are set using `#pragma config XXX=XXX` in the file `main.c`

i added a commented line `#define CONFIG_CPU_USE_FRC`, to facilitate the use of internal / external oscillator

the default config tested in a board with the following config:

- PIC32MZ2048EFM (Revision A3)
- external crystal for primary oscillator **24Mhz**
- config is set for the MCU to run at **200Mhz** speed
- WDT, DMT are all disabled

## Linker script

i used a custom linker script named `generic_pic32mz.ld` to upload my code into a specifig starting location using a bootloader. You can modify it, or you can remove completely the custom linker and use the standard linker script that comes with xc32 compiler. To do so, go to the Makefile of the pic32mz and remove the line `LDFLAGS+= -Wl,--script="$(LD_FILE)"`.


## Hardware configuration

The gpio and alternate function are set in the header file `pic32_config.h`.

### GPIO

- set the MCU pin count of the pic32mz
- set the gpios as PIC32_GPIO_INPUT or PIC32_GPIO_OUTPUT, for now no analog function in implimented.

__Note__: defining a gpio direction in the pic32_config.h sets the initial config of the gpio. the pins can be set using functions from `gpio. file`

__Note__: any alternate function, such as UART will override the GPIO setting.

### UART and Repl

- the uart to be used with the repl must be enabled (default is UART_4)
- set the UART to be used by the REPL  in `UART_ATTACHED_TO_REPL`
- set the pin mapping for the uart. (possible mapping are found in datasheet of the MCU)
- set baudrate of repl.

### TIMER

By default Timer one is always enabled (can not be disabled), it is used as the system tick.

## Source Files

- `sysclk` : set and get the clock speed of the system and peripherals
- `board`  : set routines and defines specific to the board, such as LEDs
- `delay`  : create us or ms delays
- `gpio`   : Setters and getters for gpio settings, state write and state read
- `interrupt` : interrupt routines, for timer and uart (for now)