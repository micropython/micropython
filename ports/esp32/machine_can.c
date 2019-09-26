/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#include "soc/dport_reg.h" //FIXME: check if path is found. full path is ESPIDF/components/soc/esp32/include/soc/dport_reg.h

#include "driver/can.h"
#include "machine_can.h"

#if MICROPY_HW_ENABLE_CAN

STATIC const mp_obj_type_t machine_hw_can_type;
STATIC machine_hw_can_obj_t machine_hw_can_obj;

bool can_init(machine_hw_can_obj_t *self, gpio_num_t tx, gpio_num_t rx, CAN_speed_t baudrate, uint32_t mode) {

    can_driver_install(
            CAN_GENERAL_CONFIG_DEFAULT(rx, rx, mode),
            CAN_TIMING_CONFIG_1MBITS(),
            CAN_FILTER_CONFIG_ACCEPT_ALL()
        );
    
    double __tq; //Time quantum

    //enable module
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_CAN_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_CAN_RST);

    //configure TX pin
    gpio_set_level(tx, 1);
    gpio_set_direction(tx,GPIO_MODE_OUTPUT);
    gpio_matrix_out(tx,CAN_TX_IDX,0,0);
    gpio_pad_select_gpio(tx);

    //configure RX pin
	gpio_set_direction(rx,GPIO_MODE_INPUT);
	gpio_matrix_in(rx,CAN_RX_IDX,0);
	gpio_pad_select_gpio(rx);

    //set to PELICAN mode #FIXME: ability to change the mode to LOOPBACK
	MODULE_CAN->CDR.B.CAN_M = 0x1;

	//synchronization jump width is the same for all baud rates
	MODULE_CAN->BTR0.B.SJW = 0x1;

	//TSEG2 is the same for all baud rates
	MODULE_CAN->BTR1.B.TSEG2 = 0x1;

	//select time quantum and set TSEG1
	switch(baudrate){
		case CAN_SPEED_1000KBPS:
			MODULE_CAN->BTR1.B.TSEG1 = 0x4;
			__tq = 0.125;
			break;

		case CAN_SPEED_800KBPS:
			MODULE_CAN->BTR1.B.TSEG1	=0x6;
			__tq = 0.125;
			break;
		default:
			MODULE_CAN->BTR1.B.TSEG1 = 0xc;
			__tq = ((float)1000/baudrate) / 16;
	}

	//set baud rate prescaler FIXME: APB_CLK_FREQ in soc/soc.h
	MODULE_CAN->BTR0.B.BRP=(uint8_t)round((((APB_CLK_FREQ * __tq) / 2) - 1)/1000000)-1;

    /* Set sampling
     * 1 -> triple; the bus is sampled three times; recommended for low/medium speed buses     (class A and B) where filtering spikes on the bus line is beneficial
     * 0 -> single; the bus is sampled once; recommended for high speed buses (SAE class C)*/
    MODULE_CAN->BTR1.B.SAM	=0x1;

    //enable all interrupts
    MODULE_CAN->IER.U = 0xff;

    //no acceptance filtering, as we want to fetch all messages
    MODULE_CAN->MBX_CTRL.ACC.CODE[0] = 0;
    MODULE_CAN->MBX_CTRL.ACC.CODE[1] = 0;
    MODULE_CAN->MBX_CTRL.ACC.CODE[2] = 0;
    MODULE_CAN->MBX_CTRL.ACC.CODE[3] = 0;
    MODULE_CAN->MBX_CTRL.ACC.MASK[0] = 0xff;
    MODULE_CAN->MBX_CTRL.ACC.MASK[1] = 0xff;
    MODULE_CAN->MBX_CTRL.ACC.MASK[2] = 0xff;
    MODULE_CAN->MBX_CTRL.ACC.MASK[3] = 0xff;

    //set to normal mode
    MODULE_CAN->OCR.B.OCMODE=__CAN_OC_NOM;

    //clear error counters
    MODULE_CAN->TXERR.U = 0;
    MODULE_CAN->RXERR.U = 0;
    (void)MODULE_CAN->ECC;

    //clear interrupt flags
    (void)MODULE_CAN->IR.U;

    //install CAN ISR
    esp_intr_alloc(ETS_CAN_INTR_SOURCE,0,CAN_isr,NULL,NULL);

    //Showtime. Release Reset Mode.
    MODULE_CAN->MOD.B.RM = 0;

    return 0;
}

int can_deinit(machine_hw_can_obj_t *self){
	return can_driver_uninstall();
}

#endif // MICROPY_HW_ENABLE_CAN