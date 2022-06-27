/*
Github: junhuanchen
Copyright (c) 2018 Juwan
Licensed under the MIT license:
http://www.opensource.org/licenses/mit-license.php
*/

#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>

#include <esp_clk.h>
#include <driver/gpio.h>
#include <soc/cpu.h>

#define SW_EOF -1 

typedef struct sw_serial
{
    gpio_num_t rxPin, txPin;
    uint32_t buffSize, bitTime, rx_start_time, rx_end_time;
    bool invert, overflow;
    volatile uint32_t inPos, outPos;
    uint8_t *buffer;
} SwSerial;

SwSerial *sw_new(gpio_num_t Tx, gpio_num_t Rx, bool Inverse, int buffSize)
{
    SwSerial *tmp = (SwSerial *)malloc(sizeof(SwSerial));

    if (NULL != tmp)
    {
        tmp->invert = Inverse;
        tmp->overflow = false;
        tmp->inPos = tmp->outPos = 0;
        tmp->buffSize = buffSize;
        tmp->buffer = (uint8_t *)malloc(buffSize);
        if (NULL != tmp->buffer)
        {
            tmp->rxPin = Rx;
            gpio_pad_select_gpio(Rx);
            gpio_set_direction(Rx, GPIO_MODE_INPUT);

            tmp->txPin = Tx;
            gpio_pad_select_gpio(Tx);
            gpio_set_direction(Tx, GPIO_MODE_OUTPUT);

            // For the TTL level of positive logic, the starting bit is the low level of one bit time.
            gpio_set_level(Tx, !Inverse); 
            // Too short leads to sticky bags
            // One byte of time 9600 104us * 10 115200 18us
            vTaskDelay(2 / portTICK_RATE_MS);

            return tmp;
        }
        free(tmp), tmp = NULL;
    }

    return tmp;
}

void sw_del(SwSerial *self)
{
    if (NULL != self->buffer)
    {
        free(self->buffer);
    }
    
    free(self);
}

uint32_t getCycleCount()
{
    return esp_cpu_get_ccount();
}

#define WaitBitTime(wait) \
    for (uint32_t start = getCycleCount(); getCycleCount() - start < wait;)

// The first byte will wrong, after normal
static void IRAM_ATTR sw_rx_handler(void *args)
{
    SwSerial *self = (SwSerial *)args;
    
    uint8_t rec = 0;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    // (self->invert) flag invert set Start 1 And Stop 0 invert
    // But myself not need, so need extra added by yourself

    // Wait Start Bit To Start
    WaitBitTime(self->rx_start_time);
    if (0 == gpio_get_level(self->rxPin))
    {
        for (uint8_t i = 0; i != 8; i++)
        {
            rec >>= 1;
            WaitBitTime(self->bitTime);
            if (gpio_get_level(self->rxPin))
            {
                rec |= 0x80;
            }
        }
        // Wait Start Bit To End
        WaitBitTime(self->rx_end_time);
        if (1 == gpio_get_level(self->rxPin))
        {
            // Stop bit Allow Into RecvBuffer
            // Store the received value in the buffer unless we have an overflow
            int next = (self->inPos + 1) % self->buffSize;
            if (next != self->outPos)
            {
                self->buffer[self->inPos] = (self->invert) ? ~rec : rec;
                self->inPos = next;
            }
            else
            {
                self->overflow = true;
            }
        }
    }
    
    portEXIT_CRITICAL(&mux);
    // Must clear this bit in the interrupt register,
    // it gets set even when interrupts are disabled

    // Esp32 GPIO.status_w1tc interrupt auto recovery
}

esp_err_t sw_enableRx(SwSerial *self, bool State)
{
    esp_err_t error = ESP_OK;
    if (State)
    {
        gpio_set_intr_type(self->rxPin, (self->invert) ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE);
        gpio_install_isr_service(0);
        error = gpio_isr_handler_add(self->rxPin, sw_rx_handler, (void*)self);

    }
    else
    {
        error = gpio_isr_handler_remove(self->rxPin);
        gpio_uninstall_isr_service();
    }
    
    return error;
}

int sw_write(SwSerial *self, uint8_t byte)
{
    if (self->invert)
    {
        byte = ~byte;
    }
    
    // Disable interrupts in order to get a clean transmit
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    // create tx interrupts to start bit.
    gpio_set_level(self->txPin, 1), gpio_set_level(self->txPin, 0);

    WaitBitTime(self->bitTime);

    for (uint8_t i = 0; i != 8; i++)
    {
        gpio_set_level(self->txPin, (byte & 1) ? 1 : 0);
        WaitBitTime(self->bitTime);

        byte >>= 1;
    }

    // Stop bit
    gpio_set_level(self->txPin, 1);
    WaitBitTime(self->bitTime);

    // re-enable interrupts
    portEXIT_CRITICAL(&mux);
    
    return 1;
}

int sw_read(SwSerial *self)
{
    if (self->inPos != self->outPos)
    {
        uint8_t ch = self->buffer[self->outPos];
        self->outPos = (self->outPos + 1) % self->buffSize;
        return ch;
    }
    return -1;
}

// suggest max datalen <= 256 and baudRate <= 115200
esp_err_t sw_open(SwSerial *self, uint32_t baudRate)
{
    // The oscilloscope told me
    self->bitTime = (esp_clk_cpu_freq() / baudRate);

    // Rx bit Timing Settings
    switch (baudRate)
    {
        case 115200:
            self->rx_start_time = (self->bitTime / 256);
            self->rx_end_time = (self->bitTime * 127 / 256);
            break;
        
        case 9600:
            self->rx_start_time = (self->bitTime / 9);
            self->rx_end_time = (self->bitTime * 8 / 9);
            break;
        
        default: // tested 57600 len 256
            self->rx_start_time = (self->bitTime / 9);
            self->rx_end_time = (self->bitTime * 8 / 9);
            break;
    }
    
    // printf("sw_open %u %d\n", self->rx_start_time, self->rx_end_time);

    sw_write(self, 0x00); // Initialization uart link

    return sw_enableRx(self, true);
}

esp_err_t sw_stop(SwSerial *self)
{
    return sw_enableRx(self, false);
}

int sw_any(SwSerial *self)
{
    int avail = self->inPos - self->outPos;
    return (avail < 0) ? avail + self->buffSize : avail;
}

void sw_flush(SwSerial *self)
{
    self->inPos = self->outPos = 0;
    self->overflow = false;
}

bool sw_overflow(SwSerial *self)
{
    return self->overflow;
}

int sw_peek(SwSerial *self)
{
    if (self->inPos != self->outPos)
    {
        return self->buffer[self->outPos];
    }
    return -1;
}
