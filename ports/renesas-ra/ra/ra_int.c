/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include "hal_data.h"
#include "ra_config.h"
#include "ra_int.h"

/*
 * store channel no or irq_no for specified irq_type(irq index)
 */
uint8_t irq_to_ch[IRQ_MAX];

void ra_int_init(void) {
    uint32_t i;
    for (i = 0; i < IRQ_MAX; i++) {
        irq_to_ch[i] = 0;
    }
    #if defined(VECTOR_NUMBER_AGT0_INT)
    irq_to_ch[VECTOR_NUMBER_AGT0_INT] = 0;
    #endif
    #if defined(VECTOR_NUMBER_AGT1_INT)
    irq_to_ch[VECTOR_NUMBER_AGT1_INT] = 1;
    #endif
    #if defined(VECTOR_NUMBER_IIC0_RXI)
    irq_to_ch[VECTOR_NUMBER_IIC0_RXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_IIC0_TXI)
    irq_to_ch[VECTOR_NUMBER_IIC0_TXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_IIC0_TEI)
    irq_to_ch[VECTOR_NUMBER_IIC0_TEI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_IIC0_ERI)
    irq_to_ch[VECTOR_NUMBER_IIC0_ERI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_IIC1_RXI)
    irq_to_ch[VECTOR_NUMBER_IIC1_RXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_IIC1_TXI)
    irq_to_ch[VECTOR_NUMBER_IIC1_TXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_IIC1_TEI)
    irq_to_ch[VECTOR_NUMBER_IIC1_TEI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_IIC1_ERI)
    irq_to_ch[VECTOR_NUMBER_IIC1_ERI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_IIC2_RXI)
    irq_to_ch[VECTOR_NUMBER_IIC2_RXI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_IIC2_TXI)
    irq_to_ch[VECTOR_NUMBER_IIC2_TXI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_IIC2_TEI)
    irq_to_ch[VECTOR_NUMBER_IIC2_TEI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_IIC2_ERI)
    irq_to_ch[VECTOR_NUMBER_IIC2_ERI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_SPI0_RXI)
    irq_to_ch[VECTOR_NUMBER_SPI0_RXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SPI0_TXI)
    irq_to_ch[VECTOR_NUMBER_SPI0_TXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SPI0_TEI)
    irq_to_ch[VECTOR_NUMBER_SPI0_TEI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SPI0_ERI)
    irq_to_ch[VECTOR_NUMBER_SPI0_ERI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SPI1_RXI)
    irq_to_ch[VECTOR_NUMBER_SPI1_RXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SPI1_TXI)
    irq_to_ch[VECTOR_NUMBER_SPI1_TXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SPI1_TEI)
    irq_to_ch[VECTOR_NUMBER_SPI1_TEI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SPI1_ERI)
    irq_to_ch[VECTOR_NUMBER_SPI1_ERI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI0_RXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SCI0_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI0_TXI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SCI0_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI0_TEI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SCI0_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI0_ERI] = 0;
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI1_RXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SCI1_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI1_TXI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SCI1_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI1_TEI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SCI1_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI1_ERI] = 1;
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI2_RXI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_SCI2_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI2_TXI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_SCI2_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI2_TEI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_SCI2_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI2_ERI] = 2;
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI3_RXI] = 3;
    #endif
    #if defined(VECTOR_NUMBER_SCI3_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI3_TXI] = 3;
    #endif
    #if defined(VECTOR_NUMBER_SCI3_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI3_TEI] = 3;
    #endif
    #if defined(VECTOR_NUMBER_SCI3_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI3_ERI] = 3;
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI4_RXI] = 4;
    #endif
    #if defined(VECTOR_NUMBER_SCI4_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI4_TXI] = 4;
    #endif
    #if defined(VECTOR_NUMBER_SCI4_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI4_TEI] = 4;
    #endif
    #if defined(VECTOR_NUMBER_SCI4_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI4_ERI] = 4;
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI5_RXI] = 5;
    #endif
    #if defined(VECTOR_NUMBER_SCI5_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI5_TXI] = 5;
    #endif
    #if defined(VECTOR_NUMBER_SCI5_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI5_TEI] = 5;
    #endif
    #if defined(VECTOR_NUMBER_SCI5_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI5_ERI] = 5;
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI6_RXI] = 6;
    #endif
    #if defined(VECTOR_NUMBER_SCI6_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI6_TXI] = 6;
    #endif
    #if defined(VECTOR_NUMBER_SCI6_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI6_TEI] = 6;
    #endif
    #if defined(VECTOR_NUMBER_SCI6_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI6_ERI] = 6;
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI7_RXI] = 7;
    #endif
    #if defined(VECTOR_NUMBER_SCI7_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI7_TXI] = 7;
    #endif
    #if defined(VECTOR_NUMBER_SCI7_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI7_TEI] = 7;
    #endif
    #if defined(VECTOR_NUMBER_SCI7_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI7_ERI] = 7;
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI8_RXI] = 8;
    #endif
    #if defined(VECTOR_NUMBER_SCI8_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI8_TXI] = 8;
    #endif
    #if defined(VECTOR_NUMBER_SCI8_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI8_TEI] = 8;
    #endif
    #if defined(VECTOR_NUMBER_SCI8_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI8_ERI] = 8;
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    irq_to_ch[VECTOR_NUMBER_SCI9_RXI] = 9;
    #endif
    #if defined(VECTOR_NUMBER_SCI9_TXI)
    irq_to_ch[VECTOR_NUMBER_SCI9_TXI] = 9;
    #endif
    #if defined(VECTOR_NUMBER_SCI9_TEI)
    irq_to_ch[VECTOR_NUMBER_SCI9_TEI] = 9;
    #endif
    #if defined(VECTOR_NUMBER_SCI9_ERI)
    irq_to_ch[VECTOR_NUMBER_SCI9_ERI] = 9;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ0] = 0;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ1] = 1;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ2] = 2;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ3] = 3;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ4] = 4;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ5] = 5;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ6] = 6;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ7] = 7;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ8] = 8;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ9] = 9;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ10] = 10;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ11] = 11;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ12] = 12;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ13] = 13;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ14] = 14;
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    irq_to_ch[VECTOR_NUMBER_ICU_IRQ15] = 15;
    #endif
}
