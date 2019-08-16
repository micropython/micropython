#include "ets_sys.h"
#include "etshal.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "esp_mphal.h"
#include "user_interface.h"
#include "espmissingincludes.h"
#include "softuart.h"

//array of pointers to instances
Softuart *_Softuart_GPIO_Instances[SOFTUART_GPIO_COUNT];
uint8_t _Softuart_Instances_Count = 0;

//intialize list of gpio names and functions
softuart_reg_t softuart_reg[] =
{
	{ PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0 },	//gpio0
	{ PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1 },	//gpio1 (uart)
	{ PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2 },	//gpio2
	{ PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3 },	//gpio3 (uart)
	{ PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4 },	//gpio4
	{ PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5 },	//gpio5
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12 },	//gpio12
	{ PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13 },	//gpio13
	{ PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14 },	//gpio14
	{ PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15 },	//gpio15
	//@TODO TODO gpio16 is missing (?include)
};

uint8_t Softuart_Bitcount(uint32_t x)
{
	uint8_t count;
 
	for (count=0; x != 0; x>>=1) {
		if ( x & 0x01) {
			return count;
		}
		count++;
	}
	//error: no 1 found!
	return 0xFF;
}

uint8_t Softuart_IsGpioValid(uint8_t gpio_id)
{
	if ((gpio_id > 5 && gpio_id < 12) || gpio_id > 15)
	{
		return 0;
	}
	return 1;
}

void Softuart_SetPinRx(Softuart *s, uint8_t gpio_id)
{ 
	if(! Softuart_IsGpioValid(gpio_id)) {
		os_printf("SOFTUART GPIO not valid %d\r\n",gpio_id);
	} else {
		s->pin_rx.gpio_id = gpio_id;
		s->pin_rx.gpio_mux_name = softuart_reg[gpio_id].gpio_mux_name;
		s->pin_rx.gpio_func = softuart_reg[gpio_id].gpio_func;
	}
}

void Softuart_SetPinTx(Softuart *s, uint8_t gpio_id)
{ 
	if(! Softuart_IsGpioValid(gpio_id)) {
		os_printf("SOFTUART GPIO not valid %d\r\n",gpio_id);
	} else {
		s->pin_tx.gpio_id = gpio_id;
		s->pin_tx.gpio_mux_name = softuart_reg[gpio_id].gpio_mux_name;
		s->pin_tx.gpio_func = softuart_reg[gpio_id].gpio_func;
	}
}

void Softuart_EnableRs485(Softuart *s, uint8_t gpio_id)
{
	os_printf("SOFTUART RS485 init\r\n");

	//enable rs485
	s->is_rs485 = 1;

	//set pin in instance
	s->pin_rs485_tx_enable = gpio_id;

	//enable pin as gpio
	PIN_FUNC_SELECT(softuart_reg[gpio_id].gpio_mux_name,softuart_reg[gpio_id].gpio_func); 

	PIN_PULLUP_DIS(softuart_reg[gpio_id].gpio_mux_name);
	
	//set low for tx idle (so other bus participants can send)
	GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_id), 0);
	
	os_printf("SOFTUART RS485 init done\r\n");
}

void Softuart_Init(Softuart *s, uint32_t baudrate)
{
	//disable rs485
	s->is_rs485 = 0;

	if(! _Softuart_Instances_Count) {
		os_printf("SOFTUART initialize gpio\r\n");
		//Initilaize gpio subsystem
		gpio_init();
	}

	//set bit time
	if(baudrate <= 0) {
                os_printf("SOFTUART ERROR: Set baud rate (%d)\r\n",baudrate);
        } else {
            s->bit_time = system_get_cpu_freq() * 1000000 / baudrate;
            os_printf("SOFTUART bit_time is %d\r\n",s->bit_time);
        }


	//init tx pin
	if(!s->pin_tx.gpio_mux_name) {
		os_printf("SOFTUART ERROR: Set tx pin (%d)\r\n",s->pin_tx.gpio_mux_name);
	} else {
		//enable pin as gpio
    	PIN_FUNC_SELECT(s->pin_tx.gpio_mux_name, s->pin_tx.gpio_func);

		//set pullup (UART idle is VDD)
		PIN_PULLUP_EN(s->pin_tx.gpio_mux_name);
		
		//set high for tx idle
		GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_tx.gpio_id), 1);
                unsigned int delay = 100000;
		os_delay_us(delay);
		
		os_printf("SOFTUART TX INIT DONE\r\n");
	}

	//init rx pin
	if(!s->pin_rx.gpio_mux_name) {
		os_printf("SOFTUART ERROR: Set rx pin (%d)\r\n",s->pin_rx.gpio_mux_name);
	} else {
		//enable pin as gpio
    	PIN_FUNC_SELECT(s->pin_rx.gpio_mux_name, s->pin_rx.gpio_func);

		//set pullup (UART idle is VDD)
		PIN_PULLUP_EN(s->pin_rx.gpio_mux_name);
		
		//set to input -> disable output
		GPIO_DIS_OUTPUT(GPIO_ID_PIN(s->pin_rx.gpio_id));

		//set interrupt related things

		//disable interrupts by GPIO
		ETS_GPIO_INTR_DISABLE();

		//attach interrupt handler and a pointer that will be passed around each time
		ETS_GPIO_INTR_ATTACH(Softuart_Intr_Handler, s);

		//not sure what this does... (quote from example):
		//    void gpio_register_set(uint32 reg_id, uint32 value);
		//
		// From include file
		//   Set the specified GPIO register to the specified value.
		//   This is a very general and powerful interface that is not
		//   expected to be used during normal operation.  It is intended
		//   mainly for debug, or for unusual requirements.
		//
		// All people repeat this mantra but I don't know what it means
		//
		gpio_register_set(GPIO_PIN_ADDR(s->pin_rx.gpio_id),
							   GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)  |
							   GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE) |
							   GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
		
		//clear interrupt handler status, basically writing a low to the output
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(s->pin_rx.gpio_id));

		//enable interrupt for pin on any edge (rise and fall)
		//@TODO: should work with ANYEDGE (=3), but complie error
		gpio_pin_intr_state_set(GPIO_ID_PIN(s->pin_rx.gpio_id), 3);

		//globally enable GPIO interrupts
		ETS_GPIO_INTR_ENABLE();
	
		os_printf("SOFTUART RX INIT DONE\r\n");
	}

	//add instance to array of instances
	_Softuart_GPIO_Instances[s->pin_rx.gpio_id] = s;
	_Softuart_Instances_Count++;
		
	os_printf("SOFTUART INIT DONE\r\n");
}

//***********************************

#define RSR_CCOUNT(r)     __asm__ __volatile__("rsr %0,ccount":"=a" (r))
static inline uint32_t MP_FASTCODE(get_ccount)(void)
{
    uint32_t ccount;
    RSR_CCOUNT(ccount);
    return ccount;
}

//***********************************

void MP_FASTCODE(Softuart_Intr_Handler)(void *p)
{
    uint8_t level, gpio_id;
    unsigned start_time = get_ccount();
    
    //ignore void* pointer and make a new pointer
    Softuart *s;

    // disable all interrupts
    ets_intr_lock();

    // clear gpio status. Say ESP8266EX SDK Programming Guide in  5.1.6. GPIO interrupt handler

    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	gpio_id = Softuart_Bitcount(gpio_status);

	//if interrupt was by an attached rx pin
    if (gpio_id != 0xFF)
    {
		//load instance which has rx pin on interrupt pin attached
		s = _Softuart_GPIO_Instances[gpio_id];

// disable interrupt for GPIO0
        gpio_pin_intr_state_set(GPIO_ID_PIN(s->pin_rx.gpio_id), GPIO_PIN_INTR_DISABLE);

// Do something, for example, increment whatyouwant indirectly
		//check level
		level = GPIO_INPUT_GET(GPIO_ID_PIN(s->pin_rx.gpio_id));
		if(!level) {
			//pin is low
			//therefore we have a start bit

			//now sample bits
			unsigned i;
			unsigned d = 0;
                        unsigned elapsed_time = get_ccount() - start_time;
                        s->elapsed = elapsed_time;

			//wait till start bit is half over so we can sample the next one in the center
			//os_delay_us(s->bit_time/2);	
                        if (elapsed_time < s->bit_time / 2) {
                            unsigned wait_time = s->bit_time / 2 - elapsed_time;
                            while ((get_ccount() - start_time) < wait_time);
                            start_time += wait_time;
                        }

			for(i = 0; i < 8; i ++ )
			{
				while ((get_ccount() - start_time) < s->bit_time);
				//shift d to the right
				d >>= 1;

				//read bit
				if(GPIO_INPUT_GET(GPIO_ID_PIN(s->pin_rx.gpio_id))) {
					//if high, set msb of 8bit to 1
					d |= 0x80;
				}
                                // recalculate start time for next bit
                                start_time += s->bit_time;
			}

			//wait for stop bit
			//os_delay_us(s->bit_time);
                        while ((get_ccount() - start_time) < s->bit_time);
	
			//store byte in buffer

			// if buffer full, set the overflow flag and return
			uint8 next = (s->buffer.receive_buffer_tail + 1) % SOFTUART_MAX_RX_BUFF;
			if (next != s->buffer.receive_buffer_head)
			{
			  // save new data in buffer: tail points to where byte goes
			  s->buffer.receive_buffer[s->buffer.receive_buffer_tail] = d; // save new byte
			  s->buffer.receive_buffer_tail = next;
			} 
			else 
			{
			  s->buffer.buffer_overflow = 1;
			}	

			//done
		}

		//clear interrupt
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

// Reactivate interrupts for GPIO0
        gpio_pin_intr_state_set(GPIO_ID_PIN(s->pin_rx.gpio_id), 3);
	} else {
		//clear interrupt, no matter from which pin
		//otherwise, this interrupt will be called again forever
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	}
        // re-enable all interrupts
  
        ets_intr_unlock();
  
}



// Read data from buffer
uint8_t Softuart_Read(Softuart *s)
{
  // Empty buffer?
  if (s->buffer.receive_buffer_head == s->buffer.receive_buffer_tail)
    return 0;

  // Read from "head"
  uint8_t d = s->buffer.receive_buffer[s->buffer.receive_buffer_head]; // grab next byte
  s->buffer.receive_buffer_head = (s->buffer.receive_buffer_head + 1) % SOFTUART_MAX_RX_BUFF; 
  return d;
}

// Flush data from buffer
uint32_t Softuart_Flush(Softuart *s)
{
  uint32_t num_chars = s->buffer.receive_buffer_tail - s->buffer.receive_buffer_head;
  // Empty buffer
  s->buffer.receive_buffer_head = 0;
  s->buffer.receive_buffer_tail = 0;
  return num_chars;
}

// Is data in buffer available?
BOOL Softuart_Available(Softuart *s)
{
    return (s->buffer.receive_buffer_tail + SOFTUART_MAX_RX_BUFF - s->buffer.receive_buffer_head) % SOFTUART_MAX_RX_BUFF;
}

int Softuart_rx_any(Softuart *s) {
    return (Softuart_Available(s));
}

int Softuart_tx_any_room(Softuart *s) {
    // TODO always assume we have room... there is no send buffer
    return true;
}


// cversek:
// based on micropython/esp8266/uart.c bool uart_rx_wait(uint32_t timeout_us)
// Waits at most timeout microseconds for at least 1 char to become ready for reading.
// Returns true if something available, false if not.
BOOL Softuart_rxWait(Softuart *s, uint32_t timeout_us)
{
    //handles system_get_time() wrap-around
    int32_t when_timedout = (int32_t) system_get_time() + timeout_us;
    for (;;) {
        if (Softuart_Available(s)) {
            return true; // have at least 1 char ready for reading
        }
        //handles system_get_time()-wrap around
        if (when_timedout - (int32_t) system_get_time() <= 0) {
            return false; // timeout
        }
        ets_event_poll();
    }
}


static inline u8 chbit(u8 data, u8 bit)
{
    if ((data & bit) != 0)
    {
    	return 1;
    }
    else
    {
    	return 0;
    }
}

// Function for printing individual characters
void MP_FASTCODE(Softuart_Putchar)(Softuart *s, char data)
{
	unsigned i;
	unsigned start_time;

  // is this needed? delay(0);

	start_time = get_ccount();
  
        // disable all interrupts
  
        ets_intr_lock();
  
	//if rs485 set tx enable
	if(s->is_rs485 == 1)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_rs485_tx_enable), 1);
	}

	//Start Bit
	GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_tx.gpio_id), 0);
	for(i = 0; i < 8; i ++ )
	{
		while ((get_ccount() - start_time) < s->bit_time);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_tx.gpio_id), chbit(data,1<<i));
        
                // recalculate start time for next bit
                start_time += s->bit_time;
	}

	// Stop bit
	while ((get_ccount() - start_time) < s->bit_time);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_tx.gpio_id), 1);

	// Delay after byte, for new sync
	os_delay_us(s->bit_time*6/system_get_cpu_freq());

	//if rs485 set tx disable 
	if(s->is_rs485 == 1)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(s->pin_rs485_tx_enable), 0);
	}
  
        // re-enable all interrupts
  
        ets_intr_unlock();
}

void Softuart_Puts(Softuart *s, const char *c )
{
	while ( *c ) {
      	Softuart_Putchar(s,( u8 )*c++);
	}
}

uint8_t Softuart_Readline(Softuart *s, char* Buffer, uint8_t MaxLen )
{
	uint8_t NextChar;
	uint8_t len = 0;

	while( Softuart_Available(s) )
	{
		NextChar = Softuart_Read(s);

		if(NextChar == '\r')
		{
			continue;
		} else if(NextChar == '\n') 
		{
			//break only if we already found a character
			//if it was .e.g. only \r, we wait for the first useful character
			if(len > 0) {
				break;
			}
		} else if(len < MaxLen - 1 )
		{
			*Buffer++ = NextChar;
			len++;
		} else {
			break;
		}
	}
	//add string terminator
	*Buffer++ = '\0';

	return len;
}
