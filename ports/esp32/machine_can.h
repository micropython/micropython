/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_ESP32_CAN_H
#define MICROPY_INCLUDED_ESP32_CAN_H

#if MICROPY_HW_ENABLE_CAN

typedef enum  {
	CAN_SPEED_100KBPS=100, 				/**< \brief CAN Node runs at 100kBit/s. */
	CAN_SPEED_125KBPS=125, 				/**< \brief CAN Node runs at 125kBit/s. */
	CAN_SPEED_250KBPS=250, 				/**< \brief CAN Node runs at 250kBit/s. */
	CAN_SPEED_500KBPS=500, 				/**< \brief CAN Node runs at 500kBit/s. */
	CAN_SPEED_800KBPS=800, 				/**< \brief CAN Node runs at 800kBit/s. */
	CAN_SPEED_1000KBPS=1000				/**< \brief CAN Node runs at 1000kBit/s. */
}CAN_speed_t;

enum {
    CAN_STATE_STOPPED,
    CAN_STATE_ERROR_ACTIVE,
    CAN_STATE_ERROR_WARNING,
    CAN_STATE_ERROR_PASSIVE,
    CAN_STATE_BUS_OFF,
};

typedef enum _rx_state_t {
    RX_STATE_FIFO_EMPTY = 0,
    RX_STATE_MESSAGE_PENDING,
    RX_STATE_FIFO_FULL,
    RX_STATE_FIFO_OVERFLOW,
} rx_state_t;



/** \brief CAN configuration structure */
typedef struct  {
	CAN_speed_t			speed;			/**< \brief CAN speed. */
    gpio_num_t 			tx_pin_id;		/**< \brief TX pin. */
    gpio_num_t 			rx_pin_id;		/**< \brief RX pin. */
    QueueHandle_t 		rx_queue;		/**< \brief Handler to FreeRTOS RX queue. */
}CAN_device_t;

typedef struct _machine_hw_can_obj_t {
    mp_obj_base_t base;
    mp_obj_t rxcallback0;
    mp_obj_t rxcallback1;
    mp_uint_t can_id : 8;
    bool is_enabled : 1;
    bool extframe : 1;
    byte rx_state0;
    byte rx_state1;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
    CAN_HandleTypeDef can;
} esp32_can_obj_t;

extern const mp_obj_type_t machine_can_type;

//void can_init0(void); #TODO:
//void can_deinit_all(void); #TODO:
bool can_init(machine_hw_can_obj_t *self, gpio_num_t tx, gpio_num_t rx, CAN_speed_t baudrate, uint32_t mode);
void can_deinit(machine_hw_can_obj_t *self);

void can_clearfilter(pyb_can_obj_t *self, uint32_t f, uint8_t bank);
int can_receive(CAN_HandleTypeDef *can, int fifo, CanRxMsgTypeDef *msg, uint8_t *data, uint32_t timeout_ms);
HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef *hcan, uint32_t Timeout);
void pyb_can_handle_callback(pyb_can_obj_t *self, uint fifo_id, mp_obj_t callback, mp_obj_t irq_reason);

#endif // MICROPY_HW_ENABLE_CAN

#endif // MICROPY_INCLUDED_ESP32_CAN_H
