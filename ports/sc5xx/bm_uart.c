/*
 * Copyright (c) 2018 Analog Devices, Inc.  All rights reserved.
 *
 * UART Simple provides a simple, bare-metal UART driver
 */

#include "bm_uart.h"

#include <services/int/adi_int.h>
#include <sys/platform.h>
#include "mphalport.h"

// Static function prototypes
static BM_UART_RESULT uart_read_from_tx_buffer(BM_UART *device, uint8_t *val);
static BM_UART_RESULT uart_write_to_rx_buffer(BM_UART *device, uint8_t val);
static bool uart_check_rx_status(BM_UART *device);
static bool uart_check_tx_status(BM_UART *device);
static uint8_t uart_read_rx_value(BM_UART *device);
static void uart_write_tx_value(BM_UART *device, uint8_t val);
static void uart_clear_tx_interrupt(BM_UART *device);

/**
 * @brief      Custom handler for any FIFO errors
 *
 * Users can place additional code here to determine the behavior when the
 * UART's FIFO is full or empty
 *
 * @param[in]  result  The type of failure
 */
static void uart_fifo_error_handler(BM_UART_RESULT result) {
    switch (result) {
        case UART_RX_FIFO_FULL:
            // add custom behavior here for when the UART RX FIFO is full
            break;

        case UART_TX_FIFO_EMPTY:
            // add custom behavior here for when the UART TX FIFO is empty
            break;

        default:
            break;
    }
}

/**
 * @brief      This is a single handler for both UART transmit and receive.
 *
 * Because we're not using DMA mode, we're mapping the both TX and RX interrupts
 * to the status interrupt via the ELSI flag.  Within this routine, we check
 * interrupt via the ELSI flag.  Within this routine, we check if we have data to
 * send, and if any data has arrived.
 *
 * @param[in]  SID         The interrupt id
 * @param      device_ptr  The pointer to the device driver instance
 */
static void uart_status_handler(uint32_t SID,
                                void *device_ptr) {

    static int uart_words_received = 0,
               uart_words_transmitted = 0,
               uart_fifo_overruns = 0;

    BM_UART_RESULT result;
    uint8_t curRxVal, nextTxVal = 0;

    BM_UART *device = (BM_UART *)device_ptr;

    /*
     ********************************************************************************
     * RX - Check if data is ready that has been received
     ********************************************************************************
     */

    bool new_bytes_received = false;

    if ((*device->pREG_UART_STAT) & BITM_UART_STAT_OE) {
        // UART has experienced FIFO overrun
        // Optionally add code here to deal with the UART hardware overflow
        // You may also end up here if you're single stepping through this handler

        // Clear the error and move on
        *device->pREG_UART_STAT = BITM_UART_STAT_OE;

        uart_fifo_overruns++;
    }

    // copy any bytes that have arrived into our RX FIFO
    while (uart_check_rx_status(device)) {

        // read value from UART
        curRxVal = uart_read_rx_value(device);
        uart_words_received++;

        // write this value to our receive FIFO
        result = uart_write_to_rx_buffer(device, curRxVal);

        if (result != UART_SUCCESS) {
            uart_fifo_error_handler(result);
        }

        // Call port specific rx interrupt process function
        mp_hal_uart_interrupt(curRxVal);

        new_bytes_received = true;
    }
    // Trigger RX callback if we have one set if new bytes were received
    if (new_bytes_received) {
        if (device->rx_callback_function != NULL) device->rx_callback_function();
    }

    /*
     ********************************************************************************
     * TX - Check if we have data to send
     ********************************************************************************
     */

    // Clear the TX interrupt if it's set
    uart_clear_tx_interrupt(device);

    // check if the transmit buffer is empty and we can add another byte
    if (uart_check_tx_status(device)) {

        // Fetch next byte from the tx buffer.  We know there is data in the FIFO.
        result = uart_read_from_tx_buffer(device, &nextTxVal);

        // Handle any errors that may arise; however, we shouldn't be in this section
        // of code if the TX FIFO is empty.
        if (result != UART_TX_FIFO_EMPTY) {
            // Write this byte to UART peripheral
            uart_write_tx_value(device, nextTxVal);
            uart_words_transmitted++;
        }
    }
}

/**
 * @brief      Initializes an instance of the UART driver
 *
 * @param      device      pointer to the driver instance
 * @param[in]  baud        The baud rate (using #defines in .h file)
 * @param[in]  config      The configuration (using #defines in .h file)
 * @param[in]  device_num  The peripheral number (e.g. UART0, UART1)
 *
 * @return     The result of the operation
 */
BM_UART_RESULT uart_initialize(BM_UART *device,
                               BM_UART_BAUD_RATE baud,
                               BM_UART_CONFIG config,
                               BM_UART_PERIPHERAL_NUMBER device_num){

    // Control registers for UART0
    if (device_num == 0) {
        device->pREG_UART_CTL       = (volatile uint32_t *)pREG_UART0_CTL;
        device->pREG_UART_CLK       = (volatile uint32_t *)pREG_UART0_CLK;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART0_STAT;
        device->pREG_UART_THR       = (volatile uint32_t *)pREG_UART0_THR;
        device->pREG_UART_RBR       = (volatile uint32_t *)pREG_UART0_RBR;
        device->pREG_UART_IMSK_CLR  = (volatile uint32_t *)pREG_UART0_IMSK_CLR;
        device->pREG_UART_IMSK_SET  = (volatile uint32_t *)pREG_UART0_IMSK_SET;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART0_STAT;
    }

    // Control registers for UART1
    else if (device_num == 1) {
        device->pREG_UART_CTL       = (volatile uint32_t *)pREG_UART1_CTL;
        device->pREG_UART_CLK       = (volatile uint32_t *)pREG_UART1_CLK;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART1_STAT;
        device->pREG_UART_THR       = (volatile uint32_t *)pREG_UART1_THR;
        device->pREG_UART_RBR       = (volatile uint32_t *)pREG_UART1_RBR;
        device->pREG_UART_IMSK_CLR  = (volatile uint32_t *)pREG_UART1_IMSK_CLR;
        device->pREG_UART_IMSK_SET  = (volatile uint32_t *)pREG_UART1_IMSK_SET;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART1_STAT;
    }

    // Control registers for UART2
    else if (device_num == 2) {
        device->pREG_UART_CTL       = (volatile uint32_t *)pREG_UART2_CTL;
        device->pREG_UART_CLK       = (volatile uint32_t *)pREG_UART2_CLK;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART2_STAT;
        device->pREG_UART_THR       = (volatile uint32_t *)pREG_UART2_THR;
        device->pREG_UART_RBR       = (volatile uint32_t *)pREG_UART2_RBR;
        device->pREG_UART_IMSK_CLR  = (volatile uint32_t *)pREG_UART2_IMSK_CLR;
        device->pREG_UART_IMSK_SET  = (volatile uint32_t *)pREG_UART2_IMSK_SET;
        device->pREG_UART_STAT      = (volatile uint32_t *)pREG_UART2_STAT;
    }

    else {
        return UART_INVALID_DEVICE;
    }

    // store init variables
    device->device_num = device_num;
    device->baud       = baud;
    device->config     = config;

    // Set BAUD rate from presets
    *device->pREG_UART_CLK = baud;

    // Configure parity, start/stop bits and word length, enable
    *device->pREG_UART_CTL |= (uint32_t)((config << 8) & 0x0000FF00);
    *device->pREG_UART_CTL |= BITM_UART_CTL_EN;

    // Initialize buffers and pointers
    device->rx_buffer_readptr = 0;
    device->rx_buffer_writeptr = 0;
    device->tx_buffer_readptr = 0;
    device->tx_buffer_writeptr = 0;

    // Use this configuration to interrupt after every word is received (e.g. MIDI mode)
    *device->pREG_UART_IMSK_CLR = BITM_UART_IMSK_CLR_ERXS | BITM_UART_IMSK_CLR_ERBFI | BITM_UART_IMSK_CLR_ETFI |  BITM_UART_IMSK_CLR_ELSI;
    *device->pREG_UART_IMSK_SET = BITM_UART_IMSK_SET_ERXS | BITM_UART_IMSK_SET_ERBFI | BITM_UART_IMSK_SET_ETFI | BITM_UART_IMSK_SET_ELSI;

    //  Set up interrupts
    if (device->device_num == 0) {
        adi_int_InstallHandler(INTR_UART0_STAT,  (ADI_INT_HANDLER_PTR)uart_status_handler, (void *)device, true);
    }
    else if (device->device_num == 1) {
        adi_int_InstallHandler(INTR_UART1_STAT,  (ADI_INT_HANDLER_PTR)uart_status_handler, (void *)device, true);
    }
    else if (device->device_num == 2) {
        adi_int_InstallHandler(INTR_UART2_STAT,  (ADI_INT_HANDLER_PTR)uart_status_handler, (void *)device, true);
    }
    return UART_SUCCESS;
}

/**
 * @brief      Writes a single byte to the UART
 *
 * @param      device   The pointer to the driver instance
 * @param[in]  tx_byte  The transmit byte
 *
 * @return     The result of the operation - will fail if not enough memory in the
 *             TX FIFO.
 */
BM_UART_RESULT uart_write_byte(BM_UART *device,
                               uint8_t tx_byte) {

    // First check if write buffer is full
    if (((device->tx_buffer_writeptr + 1) % UART_BUFFER_SIZE) == device->tx_buffer_readptr) {
        return UART_TX_FIFO_FULL;
    }

    // add tx_byte to the buffer
    device->tx_buffer[device->tx_buffer_writeptr++] = tx_byte;

    // wrap pointer if necessary
    if (device->tx_buffer_writeptr >= UART_BUFFER_SIZE) device->tx_buffer_writeptr = 0;

    // if the TX register is empty, move an transmit byte in there from our buffer
    // (otherwise this will get sent when byte being the currently transmitted is done)
    if ((*device->pREG_UART_STAT & BITM_UART_STAT_TEMT) != 0) {

        // Transmit to UART,
        uart_write_tx_value(device, device->tx_buffer[device->tx_buffer_readptr++]);

        // wrap pointer
        if (device->tx_buffer_readptr >= UART_BUFFER_SIZE) device->tx_buffer_readptr = 0;
    }

    return UART_SUCCESS;
}

/**
 * @brief     Writes a block of bytes to the UART
 *
 * @param      device    The pointer to the driver instance
 * @param      tx_bytes  A pointer to an array of data we will transmit
 * @param[in]  len       The length of the array (in bytes / chars)
 *
 * @return     The result of the operation - will fail if not enough room in the
 *             TX FIFO
 */
BM_UART_RESULT uart_write_block(BM_UART *device,
                                uint8_t *tx_bytes,
                                uint16_t len) {
    int i;

    for (i = 0; i < len; i++) {

        // Copy bytes to UART transmit FIFO
        BM_UART_RESULT res;
        res = uart_write_byte(device, tx_bytes[i]);

        if (res != UART_SUCCESS) {
            return res;
        }
    }

    return UART_SUCCESS;
}

/**
 * @brief      Reads a byte from the receive buffer / FIFO and stores it to a pointer value
 *
 * @param      device  The pointer to the driver instance
 * @param      value   The byte / char read
 *
 * @return     The result of the operation - will fail if the read FIFO is empty.
 */
BM_UART_RESULT uart_read_byte(BM_UART *device,
                              uint8_t *value) {

    if (device->rx_buffer_writeptr == device->rx_buffer_readptr) {
        return UART_RX_FIFO_EMPTY;
    }

    // fetch latest value from rx buffer
    *value = device->rx_buffer[device->rx_buffer_readptr++];

    // wrap index if necessary
    if (device->rx_buffer_readptr >= UART_BUFFER_SIZE) {
        device->rx_buffer_readptr = 0;
    }

    return UART_SUCCESS;
}

/**
 * @brief      Adds a user function callback that is called when new data arrives
 *
 * A pointer to the driver instance will be passed to the callback as an argument
 *
 * @param      device    The pointer to the driver instance
 * @param[in]  callback  The function callback
 */
void uart_set_rx_callback(BM_UART *device,
                          void (*callback)(void)) {
    device->rx_callback_function = callback;
}

/**
 * @brief      Clears a user function callback that is called when new data arrives
 *
 * @param      device  The pointer to the driver instance
 */
void uart_clear_rx_callback(BM_UART *device) {
    device->rx_callback_function = NULL;
}

/**
 * @brief      Checks to see the number of bytes available in the receive FIFO / buffer
 *
 * @param      device  The pointer to the driver instance
 *
 * @return     The number of bytes available in the UART receive FIFO
 */
uint16_t uart_available(BM_UART *device) {

    if (device->rx_buffer_writeptr > device->rx_buffer_readptr) {
        return device->rx_buffer_writeptr - device->rx_buffer_readptr;
    }
    else if (device->rx_buffer_readptr > device->rx_buffer_writeptr) {
        return (UART_BUFFER_SIZE - device->rx_buffer_readptr - 1) + device->rx_buffer_writeptr;
    }
    else return 0;
}

/**
 * @brief     Checks to see the number of bytes available in the transmit FIFO / buffer
 *
 * @param      device  The pointer to the driver instance
 *
 * @return     The number of bytes available for writing in the TX FIFO
 */
uint16_t uart_available_for_write(BM_UART *device) {

    if (device->tx_buffer_writeptr == device->tx_buffer_readptr) {
        return UART_BUFFER_SIZE;
    }
    else if (device->tx_buffer_writeptr > device->tx_buffer_readptr) {
        return UART_BUFFER_SIZE - (device->tx_buffer_writeptr - device->tx_buffer_readptr);
    }
    else if (device->tx_buffer_readptr > device->tx_buffer_writeptr) {
        return (device->tx_buffer_readptr - device->tx_buffer_writeptr - 1);
    }
    return 0;
}

/*
 *******************************************************************************
 *  Supporting functions
 *******************************************************************************
 */

/**
 * @brief      Reads a byte from the transmit buffer / FIFO
 *
 * @param      device  The pointer to the driver instance
 * @param      val     A pointer to char where result is stored
 *
 * @return     The result of the operation - will fail if TX FIFO is empty
 */
static BM_UART_RESULT uart_read_from_tx_buffer(BM_UART *device,
                                               uint8_t *val) {

    // First check if write buffer is empty
    if (device->tx_buffer_writeptr == device->tx_buffer_readptr) {
        return UART_TX_FIFO_EMPTY;
    }

    // fetch latest value from TX buffer
    *val = device->tx_buffer[device->tx_buffer_readptr++];

    // wrap pointer
    if (device->tx_buffer_readptr >= UART_BUFFER_SIZE) {
        device->tx_buffer_readptr = 0;
    }

    return UART_SUCCESS;
}

/**
 * @brief      Writes a byte to the receive buffer / FIFO
 *
 * @param      device  The pointer to the driver instance
 * @param[in]  val     The value to be written
 *
 * @return      The result of the operation - will fail if RX FIFO is full
 */
static BM_UART_RESULT uart_write_to_rx_buffer(BM_UART *device,
                                              uint8_t val) {

    // First check if RX buffer is full
    if (((device->rx_buffer_writeptr + 1) % UART_BUFFER_SIZE) == device->rx_buffer_readptr) {
        return UART_RX_FIFO_FULL;
    }

    // Write value into FIFO
    device->rx_buffer[device->rx_buffer_writeptr++] = val;

    // wrap pointer if necessary
    if (device->rx_buffer_writeptr >= UART_BUFFER_SIZE) {
        device->rx_buffer_writeptr = 0;
    }

    return UART_SUCCESS;
}

/**
 * @brief      Checks the UART Status register to see if there is new data
 *
 * @param      device  The pointer to the driver instance
 *
 * @return     true if new data, false if no new data
 */
static bool uart_check_rx_status(BM_UART *device) {
    if ((*device->pREG_UART_STAT) & BITM_UART_STAT_DR) return true;
    else return false;
}

/**
 * @brief      Checks the UART status register to see if there is space in the transmitter
 *
 * @param      device  The pointer to the driver instance
 *
 * @return     true if room in transmitter (hardware) FIFO, false if not
 */
static bool uart_check_tx_status(BM_UART *device) {
    if ((*device->pREG_UART_STAT) & BITM_UART_STAT_THRE) return true;
    else return false;
}

/**
 * @brief      Reads a value from the RX FIFO
 *
 * @param      device  The pointer to the driver instance
 *
 * @return     The read value (char / byte)
 */
static uint8_t uart_read_rx_value(BM_UART *device) {
    return (uint8_t)(*device->pREG_UART_RBR);
}

/**
 * @brief      Writes a value to the UART
 *
 * @param      device  The pointer to the driver instance
 * @param[in]  val     The byte / char to be written
 */
static void uart_write_tx_value(BM_UART *device,
                                uint8_t val) {
    (*device->pREG_UART_THR) = val;
}

/**
 * @brief     Clears the UART TX interrupt
 *
 * @param      device  The pointer to the driver instance
 */
static void uart_clear_tx_interrupt(BM_UART *device) {
    (*device->pREG_UART_STAT) = BITM_UART_STAT_TFI;
}
