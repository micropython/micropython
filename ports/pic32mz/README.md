## Porting micropython to pic32mz

The port is a code (program) with its main, startup-code and its linker script, its own drivers hardware access implimentations irqs,etc.. The Micropython routines, or the REPL is called from within the main function after all the low level initializations.

All ports in micropython should impliment at least the following functions;

```C
/**
 * @brief initialise the gpio of led mp_hal_gpio_clock
 * - set it of by implimentin mp_hal_gpio_high or low this is called throug
 * a define called MP_HW_xxx
 * - set the direction of the pin
 */
void led_init(void);

/**
 * @brief set the state of an led
 */
void led_state(int led, int state);
/**
 * @brief its implementtation is based on the previous ones
 * mp_hal_gpio_high , get, low, etc....
 */
void led_toggle(int led);

/**
 * @brief enables the clock
 * sets which UART to be configutred always check if it is defined
 * or checks which one is defined uart2 or 3 or whatever
 * direction of the pin
 * define the structure of data, baude rate, parity etc....
 * set interrupt periority
 * enable the periority
 */
void uart_init(void);

/**
 * @brief reads from a uart, takes either the uart number
 * or an object like in stm32, returns if the uart rx is enable or if
 * there is data available?? not sure though,
 */
int uart_rx_any(uint8_t uart);

/** 
 * @brief returns one byte of data, or an int of data.
 * it may take no argument if defined locally without any relations
 * or may take a reference to an self object, like stm32
 * the reception of data happens in the backroung in an interrupt
 * then read rx is a simple read of the buffer available, or queue
 */
int uart_rx_char(object_t  obj);

/**
 * @brief send one char of data return success or not, take the char and 
 * maybe the self object.
 * the stm32 doesn't impliment this one, it implement uart_tx_data
 * sends array of data, uses uart_wait_ to wait for available
 * and uart_available to check if uart_tx is available
 */
void uart_tx_char(int chr);
```
Always with the interface needed, esp32 and stm32 impliments a board directory, where it is structured as such:

- they include 2 files, at least
- `mpconfigboard.h` : includes a list of defines starting with MICROPY_HW_
- it tells which uart is in what pin, it defines also specific routines for 
- special boards like MICROPY_HW_LED_ON, etc...
- the use of defines is, to be able to test during compilation if a such functionality exisit or not.

- `mpconfigboard.mk`, is config file for makefile, associated with make, to specify the liker script, the mcu category, csv file for AF, is there FAT system or not, etc... FROZEN,???

- Other function that are used throughout the code is the machine_* modules, this module lets you use the hardware directly from the python REPL, by calling the machine module, you can set led off, on, read uart, spi, etc....
- help.c to print the help command on REPL
- implement specific functions like eps_partitions.c...
- modmachine.c
- modnetwork.c
- modutime.c
- moduos.c
- modbyp.c : i encounterd this quite a lot, not for esp though
- modstm.c
- modesp.c for esp32 and so, as we said, specific routine implementation for specific processors is used to.
- mphalport.c
- mpnimbleport.c
- mpthreadport.c
- 
- machine_uart.c diffrent from uart.c, the uart is used by the REPL 

- fatfs_port.c if there is a fatfs in the system
- gccollect.c garbage collector

So finally interstootd that the modxx files are modules for python. these files make use of mp_objects fro the python utility, along with the machine module too.

And every possible hardware in the board.

### Note on the mphalport.c

This function impliment the interface with the python library, for example 
`mp_hal_stdin_rx_chr`, is used by the util/pyexec  module to print out and read data from console. this routine mp_hal_stdin_rx_chr is the one that uses `uart_rx_chr` mentioned before, but not only this routine.

### In details

`mphalport.c` : implement the following routines

- mphal_stdio_poll()
- mphal_stdin_rx_chr()
- mphal_stdout_tx_strn()
- mphal_stdout_tx_strn_cooked()
- mphal_ticks_ or mphal_delay_us or delay_ms or only one
- mphal_raise or mphal_esp_err for raising an error

`mpnimbleport.c` : is a portable Bleutooth stack

`mpthreadport.c` : this uses mutex_ to control access to the linked list, the file implements

- mp_thread_init()
- mp_thread_gc_others()
- mp_thread_create()
- other routines might be implimented, like set_state, get_state of the thread
- the esp32 thread is implimented better than the stm32 and it is based on freertos 

Note the threadings, depend on the host using it. for example the esp32 is based on the freertos
- and even the micropython is implimented above the freertos, so its internal routines uses
- mp_thread_mutex_lock and unlock mp_thread_mutex_init, the esp impliment that using xSemaphoreCreate for init
- xSemaphoreGive and Take() for lock unlock
- the stm32 uses pyb_mutex_init(), lock and lock. this system is made for the pyboard and uses stm32 CIMSIS calls
- it is a simple allocating deallocating system made in c
- when the micropython usese freertos for example like in the esp32,the freertos needs to be downloaded

These are the minimal to start, the pic16 demo uses less than that and it seems to work normally.

### Additional Notes

The MakeFile seems uncomprehensible, but a lot of compilation is done behind the scenes.

the compilation needs some files though apart from the mentionned ones:

- `qstrdefports.h` : contains non **C** lines like Q(/flash) the same $Q is present in the makefile
- `mpconfigport.h` have defines as options to control how MicroPython is built, typedef for types,etc..
- `uintstd.h`: it seems that microchip compilers needs doesn't have basic definitions like SEEK
- `mphalport.h` and `mphalport.c` : mentioned earlier