//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__

//*****************************************************************************
//
// The following are defines for the GPIO register offsets.
//
//*****************************************************************************
#define GPIO_O_GPIO_DATA        0x00000000  // 0x4000 5000 0x4000 6000 0x4000
                                            // 7000 0x4002 4000 GPIO Data
                                            // (GPIODATA)@@ offset 0x000 The
                                            // GPIODATA register is the data
                                            // register. In software control
                                            // mode@@ values written in the
                                            // GPIODATA register are transferred
                                            // onto the GPIO port pins if the
                                            // respective pins have been
                                            // configured as outputs through the
                                            // GPIO Direction (GPIODIR) register
                                            // (see page 653). In order to write
                                            // to GPIODATA@@ the corresponding
                                            // bits in the mask@@ resulting from
                                            // the address bus bits [9:2]@@ must
                                            // be set. Otherwise@@ the bit
                                            // values remain unchanged by the
                                            // write. Similarly@@ the values
                                            // read from this register are
                                            // determined for each bit by the
                                            // mask bit derived from the address
                                            // used to access the data
                                            // register@@ bits [9:2]. Bits that
                                            // are set in the address mask cause
                                            // the corresponding bits in
                                            // GPIODATA to be read@@ and bits
                                            // that are clear in the address
                                            // mask cause the corresponding bits
                                            // in GPIODATA to be read as 0@@
                                            // regardless of their value. A read
                                            // from GPIODATA returns the last
                                            // bit value written if the
                                            // respective pins are configured as
                                            // outputs@@ or it returns the value
                                            // on the corresponding input pin
                                            // when these are configured as
                                            // inputs. All bits are cleared by a
                                            // reset.
#define GPIO_O_GPIO_DIR         0x00000400  // 0x4000 5400 0x4000 6400 0x4000
                                            // 7400 0x4002 4400 GPIO Direction
                                            // (GPIODIR)@@ offset 0x400 The
                                            // GPIODIR register is the data
                                            // direction register. Setting a bit
                                            // in the GPIODIR register
                                            // configures the corresponding pin
                                            // to be an output@@ while clearing
                                            // a bit configures the
                                            // corresponding pin to be an input.
                                            // All bits are cleared by a reset@@
                                            // meaning all GPIO pins are inputs
                                            // by default.
#define GPIO_O_GPIO_IS          0x00000404  // 0x4000 5404 0x4000 6404 0x4000
                                            // 7404 0x4002 4404 GPIO Interrupt
                                            // Sense (GPIOIS)@@ offset 0x404 The
                                            // GPIOIS register is the interrupt
                                            // sense register. Setting a bit in
                                            // the GPIOIS register configures
                                            // the corresponding pin to detect
                                            // levels@@ while clearing a bit
                                            // configures the corresponding pin
                                            // to detect edges. All bits are
                                            // cleared by a reset.
#define GPIO_O_GPIO_IBE         0x00000408  // 0x4000 5408 0x4000 6408 0x4000
                                            // 7408 0x4002 4408 GPIO Interrupt
                                            // Both Edges (GPIOIBE)@@ offset
                                            // 0x408 The GPIOIBE register allows
                                            // both edges to cause interrupts.
                                            // When the corresponding bit in the
                                            // GPIO Interrupt Sense (GPIOIS)
                                            // register is set to detect edges@@
                                            // setting a bit in the GPIOIBE
                                            // register configures the
                                            // corresponding pin to detect both
                                            // rising and falling edges@@
                                            // regardless of the corresponding
                                            // bit in the GPIO Interrupt Event
                                            // (GPIOIEV) register . Clearing a
                                            // bit configures the pin to be
                                            // controlled by the GPIOIEV
                                            // register. All bits are cleared by
                                            // a reset.
#define GPIO_O_GPIO_IEV         0x0000040C  // 0x4000 540C 0x4000 640C 0x4000
                                            // 740C 0x4002 440C GPIO Interrupt
                                            // Event (GPIOIEV)@@ offset 0x40C
                                            // The GPIOIEV register is the
                                            // interrupt event register. Setting
                                            // a bit in the GPIOIEV register
                                            // configures the corresponding pin
                                            // to detect rising edges or high
                                            // levels@@ depending on the
                                            // corresponding bit value in the
                                            // GPIO Interrupt Sense (GPIOIS)
                                            // register . Clearing a bit
                                            // configures the pin to detect
                                            // falling edges or low levels@@
                                            // depending on the corresponding
                                            // bit value in the GPIOIS register.
                                            // All bits are cleared by a reset.
#define GPIO_O_GPIO_IM          0x00000410  // 0x4000 5410 0x4000 6410 0x4000
                                            // 7410 0x4002 4410 GPIO Interrupt
                                            // Mask (GPIOIM)@@ offset 0x410 The
                                            // GPIOIM register is the interrupt
                                            // mask register. Setting a bit in
                                            // the GPIOIM register allows
                                            // interrupts that are generated by
                                            // the corresponding pin to be sent
                                            // to the interrupt controller on
                                            // the combined interrupt signal.
                                            // Clearing a bit prevents an
                                            // interrupt on the corresponding
                                            // pin from being sent to the
                                            // interrupt controller. All bits
                                            // are cleared by a reset.
#define GPIO_O_GPIO_RIS         0x00000414  // 0x4000 5414 0x4000 6414 0x4000
                                            // 7414 0x4002 4414 GPIO Raw
                                            // Interrupt Status (GPIORIS)@@
                                            // offset 0x414 The GPIORIS register
                                            // is the raw interrupt status
                                            // register. A bit in this register
                                            // is set when an interrupt
                                            // condition occurs on the
                                            // corresponding GPIO pin. If the
                                            // corresponding bit in the GPIO
                                            // Interrupt Mask (GPIOIM) register
                                            // is set@@ the interrupt is sent to
                                            // the interrupt controller. Bits
                                            // read as zero indicate that
                                            // corresponding input pins have not
                                            // initiated an interrupt. A bit in
                                            // this register can be cleared by
                                            // writing a 1 to the corresponding
                                            // bit in the GPIO Interrupt Clear
                                            // (GPIOICR) register.
#define GPIO_O_GPIO_MIS         0x00000418  // 0x4000 5418 0x4000 6418 0x4000
                                            // 7418 0x4002 4418 GPIO Masked
                                            // Interrupt Status (GPIOMIS)@@
                                            // offset 0x418 The GPIOMIS register
                                            // is the masked interrupt status
                                            // register. If a bit is set in this
                                            // register@@ the corresponding
                                            // interrupt has triggered an
                                            // interrupt to the interrupt
                                            // controller. If a bit is clear@@
                                            // either no interrupt has been
                                            // generated@@ or the interrupt is
                                            // masked. If no port pin@@ other
                                            // than the one that is being used
                                            // as an ADC trigger@@ is being used
                                            // to generate interrupts@@ the
                                            // appropriate Interrupt Set Enable
                                            // (ENn) register can disable the
                                            // interrupts for the port@@ and the
                                            // ADC interrupt can be used to read
                                            // back the converted data.
                                            // Otherwise@@ the port interrupt
                                            // handler must ignore and clear
                                            // interrupts on the port pin and
                                            // wait for the ADC interrupt@@ or
                                            // the ADC interrupt must be
                                            // disabled in the EN0 register and
                                            // the port interrupt handler must
                                            // poll the ADC registers until the
                                            // conversion is completed. If no
                                            // port pin@@ other than the one
                                            // that is being used as an ADC
                                            // trigger@@ is being used to
                                            // generate interrupts@@ the
                                            // appropriate Interrupt Set Enable
                                            // (ENn) register can disable the
                                            // interrupts for the port@@ and the
                                            // ADC interrupt can be used to read
                                            // back the converted data.
                                            // Otherwise@@ the port interrupt
                                            // handler must ignore and clear
                                            // interrupts on the port pin and
                                            // wait for the ADC interrupt@@ or
                                            // the ADC interrupt must be
                                            // disabled in the EN0 register and
                                            // the port interrupt handler must
                                            // poll the ADC registers until the
                                            // conversion is completed. Note
                                            // that if the Port B GPIOADCCTL
                                            // register is cleared@@ PB4 can
                                            // still be used as an external
                                            // trigger for the ADC. This is a
                                            // legacy mode which allows code
                                            // written for previous Stellaris
                                            // devices to operate on this
                                            // microcontroller. GPIOMIS is the
                                            // state of the interrupt after
                                            // masking.
#define GPIO_O_GPIO_ICR         0x0000041C  // 0x4000 541C 0x4000 641C 0x4000
                                            // 741C 0x4002 441C GPIO Interrupt
                                            // Clear (GPIOICR)@@ offset 0x41C
                                            // The GPIOICR register is the
                                            // interrupt clear register. Writing
                                            // a 1 to a bit in this register
                                            // clears the corresponding
                                            // interrupt bit in the GPIORIS and
                                            // GPIOMIS registers. Writing a 0
                                            // has no effect.
#define GPIO_O_GPIO_AFSEL       0x00000420  // 0x4000 5420 0x4000 6420 0x4000
                                            // 7420 0x4002 4420 GPIO Alternate
                                            // Function Select (GPIOAFSEL)@@
                                            // offset 0x420 The GPIOAFSEL
                                            // register is the mode control
                                            // select register. If a bit is
                                            // clear@@ the pin is used as a GPIO
                                            // and is controlled by the GPIO
                                            // registers. Setting a bit in this
                                            // register configures the
                                            // corresponding GPIO line to be
                                            // controlled by an associated
                                            // peripheral. Several possible
                                            // peripheral functions are
                                            // multiplexed on each GPIO. The
                                            // GPIO Port Control (GPIOPCTL)
                                            // register is used to select one of
                                            // the possible functions.
#define GPIO_O_GPIO_DR2R        0x00000500  // 0x4000 5500 0x4000 6500 0x4000
                                            // 7500 0x4002 4500 GPIO 2-mA Drive
                                            // Select (GPIODR2R)@@ offset 0x500
                                            // The GPIODR2R register is the 2-mA
                                            // drive control register. Each GPIO
                                            // signal in the port can be
                                            // individually configured without
                                            // affecting the other pads. When
                                            // setting the DRV2 bit for a GPIO
                                            // signal@@ the corresponding DRV4
                                            // bit in the GPIODR4R register and
                                            // DRV8 bit in the GPIODR8R register
                                            // are automatically cleared by
                                            // hardware. By default@@ all GPIO
                                            // pins have 2-mA drive.
#define GPIO_O_GPIO_DR4R        0x00000504  // 0x4000 5504 0x4000 6504 0x4000
                                            // 7504 0x4002 4504 GPIO 4-mA Drive
                                            // Select (GPIODR4R)@@ offset 0x504
                                            // The GPIODR4R register is the 4-mA
                                            // drive control register. Each GPIO
                                            // signal in the port can be
                                            // individually configured without
                                            // affecting the other pads. When
                                            // setting the DRV4 bit for a GPIO
                                            // signal@@ the corresponding DRV2
                                            // bit in the GPIODR2R register and
                                            // DRV8 bit in the GPIODR8R register
                                            // are automatically cleared by
                                            // hardware.
#define GPIO_O_GPIO_DR8R        0x00000508  // 0x4000 5508 0x4000 6508 0x4000
                                            // 7508 0x4002 4508 GPIO 8-mA Drive
                                            // Select (GPIODR8R)@@ offset 0x508
                                            // The GPIODR8R register is the 8-mA
                                            // drive control register. Each GPIO
                                            // signal in the port can be
                                            // individually configured without
                                            // affecting the other pads. When
                                            // setting the DRV8 bit for a GPIO
                                            // signal@@ the corresponding DRV2
                                            // bit in the GPIODR2R register and
                                            // DRV4 bit in the GPIODR4R register
                                            // are automatically cleared by
                                            // hardware. The 8-mA setting is
                                            // also used for high-current
                                            // operation. Note: There is no
                                            // configuration difference between
                                            // 8-mA and high-current operation.
                                            // The additional current capacity
                                            // results from a shift in the
                                            // VOH/VOL levels.
#define GPIO_O_GPIO_ODR         0x0000050C  // 0x4000 550C 0x4000 650C 0x4000
                                            // 750C 0x4002 450C GPIO Open Drain
                                            // Select (GPIOODR)@@ offset 0x50C
                                            // The GPIOODR register is the open
                                            // drain control register. Setting a
                                            // bit in this register enables the
                                            // open-drain configuration of the
                                            // corresponding GPIO pad. When
                                            // open-drain mode is enabled@@ the
                                            // corresponding bit should also be
                                            // set in the GPIO Digital Input
                                            // Enable (GPIODEN) register .
                                            // Corresponding bits in the drive
                                            // strength and slew rate control
                                            // registers (GPIODR2R@@ GPIODR4R@@
                                            // GPIODR8R@@ and GPIOSLR) can be
                                            // set to achieve the desired rise
                                            // and fall times. The GPIO acts as
                                            // an open-drain input if the
                                            // corresponding bit in the GPIODIR
                                            // register is cleared. If open
                                            // drain is selected while the GPIO
                                            // is configured as an input@@ the
                                            // GPIO will remain an input and the
                                            // open-drain selection has no
                                            // effect until the GPIO is changed
                                            // to an output. When using the I2C
                                            // module@@ in addition to
                                            // configuring the pin to open
                                            // drain@@ the GPIO Alternate
                                            // Function Select (GPIOAFSEL)
                                            // register bits for the I2C clock
                                            // and data pins should be set
#define GPIO_O_GPIO_PUR         0x00000510  // 0x4000 5510 0x4000 6510 0x4000
                                            // 7510 0x4002 4510 GPIO Pull-Up
                                            // Select (GPIOPUR)@@ offset 0x510
                                            // The GPIOPUR register is the
                                            // pull-up control register. When a
                                            // bit is set@@ a weak pull-up
                                            // resistor on the corresponding
                                            // GPIO signal is enabled. Setting a
                                            // bit in GPIOPUR automatically
                                            // clears the corresponding bit in
                                            // the GPIO Pull-Down Select
                                            // (GPIOPDR) register . Write access
                                            // to this register is protected
                                            // with the GPIOCR register. Bits in
                                            // GPIOCR that are cleared prevent
                                            // writes to the equivalent bit in
                                            // this register.
#define GPIO_O_GPIO_PDR         0x00000514  // 0x4000 5514 0x4000 6514 0x4000
                                            // 7514 0x4002 4514 GPIO Pull-Down
                                            // Select (GPIOPDR)@@ offset 0x514
                                            // The GPIOPDR register is the
                                            // pull-down control register. When
                                            // a bit is set@@ a weak pull-down
                                            // resistor on the corresponding
                                            // GPIO signal is enabled. Setting a
                                            // bit in GPIOPDR automatically
                                            // clears the corresponding bit in
                                            // the GPIO Pull-Up Select (GPIOPUR)
                                            // register
#define GPIO_O_GPIO_SLR         0x00000518  // 0x4000 5518 0x4000 6518 0x4000
                                            // 7518 0x4002 4518 The GPIOSLR
                                            // register is the slew rate control
                                            // register. Slew rate control is
                                            // only available when using the
                                            // 8-mA drive strength option via
                                            // the GPIO 8-mA Drive Select
                                            // (GPIODR8R) register
#define GPIO_O_GPIO_DEN         0x0000051C  // 0x4000 551C 0x4000 651C 0x4000
                                            // 751C 0x4002 451C GPIO Digital
                                            // Enable (GPIODEN)@@ offset 0x51C
                                            // Note: Pins configured as digital
                                            // inputs are Schmitt-triggered. The
                                            // GPIODEN register is the digital
                                            // enable register. By default@@ all
                                            // GPIO signals except those listed
                                            // below are configured out of reset
                                            // to be undriven (tristate). Their
                                            // digital function is disabled;
                                            // they do not drive a logic value
                                            // on the pin and they do not allow
                                            // the pin voltage into the GPIO
                                            // receiver. To use the pin as a
                                            // digital input or output (either
                                            // GPIO or alternate function)@@ the
                                            // corresponding GPIODEN bit must be
                                            // set.
#define GPIO_O_GPIO_LOCK        0x00000520  // 0x4000 5520 0x4000 6520 0x4000
                                            // 7520 0x4002 4520 GPIO Lock
                                            // (GPIOLOCK)@@ offset 0x520 The
                                            // GPIOLOCK register enables write
                                            // access to the GPIOCR register .
                                            // Writing 0x4C4F.434B to the
                                            // GPIOLOCK register unlocks the
                                            // GPIOCR register. Writing any
                                            // other value to the GPIOLOCK
                                            // register re-enables the locked
                                            // state. Reading the GPIOLOCK
                                            // register returns the lock status
                                            // rather than the 32-bit value that
                                            // was previously written.
                                            // Therefore@@ when write accesses
                                            // are disabled@@ or locked@@
                                            // reading the GPIOLOCK register
                                            // returns 0x0000.0001. When write
                                            // accesses are enabled@@ or
                                            // unlocked@@ reading the GPIOLOCK
                                            // register returns 0x0000.0000.
#define GPIO_O_GPIO_CR          0x00000524  // 0x4000 5524 0x4000 6524 0x4000
                                            // 7524 0x4002 4524 GPIO Commit
                                            // (GPIOCR)@@ offset 0x524 The
                                            // GPIOCR register is the commit
                                            // register. The value of the GPIOCR
                                            // register determines which bits of
                                            // the GPIOAFSEL@@ GPIOPUR@@
                                            // GPIOPDR@@ and GPIODEN registers
                                            // are committed when a write to
                                            // these registers is performed. If
                                            // a bit in the GPIOCR register is
                                            // cleared@@ the data being written
                                            // to the corresponding bit in the
                                            // GPIOAFSEL@@ GPIOPUR@@ GPIOPDR@@
                                            // or GPIODEN registers cannot be
                                            // committed and retains its
                                            // previous value. If a bit in the
                                            // GPIOCR register is set@@ the data
                                            // being written to the
                                            // corresponding bit of the
                                            // GPIOAFSEL@@ GPIOPUR@@ GPIOPDR@@
                                            // or GPIODEN registers is committed
                                            // to the register and reflects the
                                            // new value. The contents of the
                                            // GPIOCR register can only be
                                            // modified if the status in the
                                            // GPIOLOCK register is unlocked.
                                            // Writes to the GPIOCR register are
                                            // ignored if the status in the
                                            // GPIOLOCK register is locked.
#define GPIO_O_GPIO_AMSEL       0x00000528  // 0x4000 5528 0x4000 6528 0x4000
                                            // 7528 0x4002 4528 The GPIOAMSEL
                                            // register controls isolation
                                            // circuits to the analog side of a
                                            // unified I/O pad. Because the
                                            // GPIOs may be driven by a 5-V
                                            // source and affect analog
                                            // operation@@ analog circuitry
                                            // requires isolation from the pins
                                            // when they are not used in their
                                            // analog function. Each bit of this
                                            // register controls the isolation
                                            // circuitry for the corresponding
                                            // GPIO signal.
#define GPIO_O_GPIO_PCTL        0x0000052C  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) 0x4000 552C
                                            // 0x4000 652C 0x4000 752C 0x4002
                                            // 452C GPIO Port Control
                                            // (GPIOPCTL)@@ offset 0x52C The
                                            // GPIOPCTL register is used in
                                            // conjunction with the GPIOAFSEL
                                            // register and selects the specific
                                            // peripheral signal for each GPIO
                                            // pin when using the alternate
                                            // function mode. Most bits in the
                                            // GPIOAFSEL register are cleared on
                                            // reset@@ therefore most GPIO pins
                                            // are configured as GPIOs by
                                            // default. When a bit is set in the
                                            // GPIOAFSEL register@@ the
                                            // corresponding GPIO signal is
                                            // controlled by an associated
                                            // peripheral. The GPIOPCTL register
                                            // selects one out of a set of
                                            // peripheral functions for each
                                            // GPIO@@ providing additional
                                            // flexibility in signal definition.
#define GPIO_O_GPIO_ADCCTL      0x00000530  // This register is not used in
                                            // cc3xx. ADC trigger via GPIO is
                                            // not supported. 0x4000 5530 0x4000
                                            // 6530 0x4000 7530 0x4002 4530 GPIO
                                            // ADC Control (GPIOADCCTL)@@ offset
                                            // 0x530 This register is used to
                                            // configure a GPIO pin as a source
                                            // for the ADC trigger. Note that if
                                            // the Port B GPIOADCCTL register is
                                            // cleared@@ PB4 can still be used
                                            // as an external trigger for the
                                            // ADC. This is a legacy mode which
                                            // allows code written for previous
                                            // Stellaris devices to operate on
                                            // this microcontroller.
#define GPIO_O_GPIO_DMACTL      0x00000534  // 0x4000 5534 0x4000 6534 0x4000
                                            // 7534 0x4002 4534 GPIO DMA Control
                                            // (GPIODMACTL)@@ offset 0x534 This
                                            // register is used to configure a
                                            // GPIO pin as a source for the ?DMA
                                            // trigger.
#define GPIO_O_GPIO_SI          0x00000538  // 0x4000 5538 0x4000 6538 0x4000
                                            // 7538 0x4002 4538 GPIO Select
                                            // Interrupt (GPIOSI)@@ offset 0x538
                                            // This register is used to enable
                                            // individual interrupts for each
                                            // pin. Note: This register is only
                                            // available on Port P and Port Q.
#define GPIO_O_GPIO_PERIPHID4   0x00000FD0  // 0x4000 5FD0 0x4000 6FD0 0x4000
                                            // 7FD0 0x4002 4FD0 GPIO Peripheral
                                            // Identification 4
                                            // (GPIOPeriphID4)@@ offset 0xFD0
                                            // The GPIOPeriphID4@@
                                            // GPIOPeriphID5@@ GPIOPeriphID6@@
                                            // and GPIOPeriphID7 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID5   0x00000FD4  // 0x4000 5FD4 0x4000 6FD4 0x4000
                                            // 7FD4 0x4002 4FD4 GPIO Peripheral
                                            // Identification 5
                                            // (GPIOPeriphID5)@@ offset 0xFD4
                                            // The GPIOPeriphID4@@
                                            // GPIOPeriphID5@@ GPIOPeriphID6@@
                                            // and GPIOPeriphID7 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID6   0x00000FD8  // 0x4000 5FD8 0x4000 6FD8 0x4000
                                            // 7FD8 0x4002 4FD8 GPIO Peripheral
                                            // Identification 6
                                            // (GPIOPeriphID6)@@ offset 0xFD8
                                            // The GPIOPeriphID4@@
                                            // GPIOPeriphID5@@ GPIOPeriphID6@@
                                            // and GPIOPeriphID7 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID7   0x00000FDC  // 0x4000 5FDC 0x4000 6FDC 0x4000
                                            // 7FDC 0x4002 4FDC GPIO Peripheral
                                            // Identification 7
                                            // (GPIOPeriphID7)@@ offset 0xFDC
                                            // The GPIOPeriphID4@@
                                            // GPIOPeriphID5@@ GPIOPeriphID6@@
                                            // and GPIOPeriphID7 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID0   0x00000FE0  // 0x4000 5FE0 0x4000 6FE0 0x4000
                                            // 7FE0 0x4002 4FE0 GPIO Peripheral
                                            // Identification 0
                                            // (GPIOPeriphID0)@@ offset 0xFE0
                                            // The GPIOPeriphID0@@
                                            // GPIOPeriphID1@@ GPIOPeriphID2@@
                                            // and GPIOPeriphID3 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID1   0x00000FE4  // 0x4000 5FE4 0x4000 6FE4 0x4000
                                            // 7FE4 0x4002 4FE4 GPIO Peripheral
                                            // Identification 1
                                            // (GPIOPeriphID1)@@ offset 0xFE4
                                            // The GPIOPeriphID0@@
                                            // GPIOPeriphID1@@ GPIOPeriphID2@@
                                            // and GPIOPeriphID3 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID2   0x00000FE8  // 0x4000 5FE8 0x4000 6FE8 0x4000
                                            // 7FE8 0x4002 4FE8 GPIO Peripheral
                                            // Identification 2
                                            // (GPIOPeriphID2)@@ offset 0xFE8
                                            // The GPIOPeriphID0@@
                                            // GPIOPeriphID1@@ GPIOPeriphID2@@
                                            // and GPIOPeriphID3 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PERIPHID3   0x00000FEC  // 0x4000 5FEC 0x4000 6FEC 0x4000
                                            // 7FEC 0x4002 4FEC GPIO Peripheral
                                            // Identification 3
                                            // (GPIOPeriphID3)@@ offset 0xFEC
                                            // The GPIOPeriphID0@@
                                            // GPIOPeriphID1@@ GPIOPeriphID2@@
                                            // and GPIOPeriphID3 registers can
                                            // conceptually be treated as one
                                            // 32-bit register; each register
                                            // contains eight bits of the 32-bit
                                            // register@@ used by software to
                                            // identify the peripheral.
#define GPIO_O_GPIO_PCELLID0    0x00000FF0  // 0x4000 5FF0 0x4000 6FF0 0x4000
                                            // 7FF0 0x4002 4FF0 GPIO PrimeCell
                                            // Identification 0 (GPIOPCellID0)@@
                                            // offset 0xFF0 The GPIOPCellID0@@
                                            // GPIOPCellID1@@ GPIOPCellID2@@ and
                                            // GPIOPCellID3 registers are four
                                            // 8-bit wide registers@@ that can
                                            // conceptually be treated as one
                                            // 32-bit register. The register is
                                            // used as a standard
                                            // cross-peripheral identification
                                            // system.
#define GPIO_O_GPIO_PCELLID1    0x00000FF4  // 0x4000 5FF4 0x4000 6FF4 0x4000
                                            // 7FF4 0x4002 4FF4 GPIO PrimeCell
                                            // Identification 1 (GPIOPCellID1)@@
                                            // offset 0xFF4 The GPIOPCellID0@@
                                            // GPIOPCellID1@@ GPIOPCellID2@@ and
                                            // GPIOPCellID3 registers are four
                                            // 8-bit wide registers@@ that can
                                            // conceptually be treated as one
                                            // 32-bit register. The register is
                                            // used as a standard
                                            // cross-peripheral identification
                                            // system.
#define GPIO_O_GPIO_PCELLID2    0x00000FF8  // 0x4000 5FF8 0x4000 6FF8 0x4000
                                            // 7FF8 0x4002 4FF8 GPIO PrimeCell
                                            // Identification 2 (GPIOPCellID2)@@
                                            // offset 0xFF8 The GPIOPCellID0@@
                                            // GPIOPCellID1@@ GPIOPCellID2@@ and
                                            // GPIOPCellID3 registers are four
                                            // 8-bit wide registers@@ that can
                                            // conceptually be treated as one
                                            // 32-bit register. The register is
                                            // used as a standard
                                            // cross-peripheral identification
                                            // system.
#define GPIO_O_GPIO_PCELLID3    0x00000FFC  // 0x4000 5FFC 0x4000 6FFC 0x4000
                                            // 7FFC 0x4002 4FFC GPIO PrimeCell
                                            // Identification 3 (GPIOPCellID3)@@
                                            // offset 0xFFC The GPIOPCellID0@@
                                            // GPIOPCellID1@@ GPIOPCellID2@@ and
                                            // GPIOPCellID3 registers are four
                                            // 8-bit wide registers@@ that can
                                            // conceptually be treated as one
                                            // 32-bit register. The register is
                                            // used as a standard
                                            // cross-peripheral identification
                                            // system.0xb1



//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DATA register.
//
//******************************************************************************
#define GPIO_GPIO_DATA_DATA_M   0x000000FF  // GPIO Data This register is
                                            // virtually mapped to 256 locations
                                            // in the address space. To
                                            // facilitate the reading and
                                            // writing of data to these
                                            // registers by independent
                                            // drivers@@ the data read from and
                                            // written to the registers are
                                            // masked by the eight address lines
                                            // [9:2]. Reads from this register
                                            // return its current state. Writes
                                            // to this register only affect bits
                                            // that are not masked by ADDR[9:2]
                                            // and are configured as outputs.
#define GPIO_GPIO_DATA_DATA_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DIR register.
//
//******************************************************************************
#define GPIO_GPIO_DIR_DIR_M     0x000000FF  // GPIO Data Direction Value
                                            // Description 0 Corresponding pin
                                            // is an input. 1 Corresponding pins
                                            // is an output.
#define GPIO_GPIO_DIR_DIR_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_IS register.
//
//******************************************************************************
#define GPIO_GPIO_IS_IS_M       0x000000FF  // GPIO Interrupt Sense Value
                                            // Description 0 The edge on the
                                            // corresponding pin is detected
                                            // (edge-sensitive). 1 The level on
                                            // the corresponding pin is detected
                                            // (level-sensitive).
#define GPIO_GPIO_IS_IS_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_IBE register.
//
//******************************************************************************
#define GPIO_GPIO_IBE_IBE_M     0x000000FF  // GPIO Interrupt Both Edges Value
                                            // Description 0 Interrupt
                                            // generation is controlled by the
                                            // GPIO Interrupt Event (GPIOIEV)
                                            // register. 1 Both edges on the
                                            // corresponding pin trigger an
                                            // interrupt.
#define GPIO_GPIO_IBE_IBE_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_IEV register.
//
//******************************************************************************
#define GPIO_GPIO_IEV_IEV_M     0x000000FF  // GPIO Interrupt Event Value
                                            // Description 1 A falling edge or a
                                            // Low level on the corresponding
                                            // pin triggers an interrupt. 0 A
                                            // rising edge or a High level on
                                            // the corresponding pin triggers an
                                            // interrupt.
#define GPIO_GPIO_IEV_IEV_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_IM register.
//
//******************************************************************************
#define GPIO_GPIO_IM_IME_M      0x000000FF  // GPIO Interrupt Mask Enable Value
                                            // Description 0 The interrupt from
                                            // the corresponding pin is masked.
                                            // 1 The interrupt from the
                                            // corresponding pin is sent to the
                                            // interrupt controller.
#define GPIO_GPIO_IM_IME_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_RIS register.
//
//******************************************************************************
#define GPIO_GPIO_RIS_RIS_M     0x000000FF  // GPIO Interrupt Raw Status Value
                                            // Description 1 An interrupt
                                            // condition has occurred on the
                                            // corresponding pin. 0 interrupt
                                            // condition has not occurred on the
                                            // corresponding pin. A bit is
                                            // cleared by writing a 1 to the
                                            // corresponding bit in the GPIOICR
                                            // register.
#define GPIO_GPIO_RIS_RIS_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_MIS register.
//
//******************************************************************************
#define GPIO_GPIO_MIS_MIS_M     0x000000FF  // GPIO Masked Interrupt Status
                                            // Value Description 1 An interrupt
                                            // condition on the corresponding
                                            // pin has triggered an interrupt to
                                            // the interrupt controller. 0 An
                                            // interrupt condition on the
                                            // corresponding pin is masked or
                                            // has not occurred. A bit is
                                            // cleared by writing a 1 to the
                                            // corresponding bit in the GPIOICR
                                            // register.
#define GPIO_GPIO_MIS_MIS_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_ICR register.
//
//******************************************************************************
#define GPIO_GPIO_ICR_IC_M      0x000000FF  // GPIO Interrupt Clear Value
                                            // Description 1 The corresponding
                                            // interrupt is cleared. 0 The
                                            // corresponding interrupt is
                                            // unaffected.
#define GPIO_GPIO_ICR_IC_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_AFSEL register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DR2R register.
//
//******************************************************************************
#define GPIO_GPIO_DR2R_DRV2_M   0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Output Pad
                                            // 2-mA Drive Enable Value
                                            // Description 1 The corresponding
                                            // GPIO pin has 2-mA drive. The
                                            // drive for the corresponding GPIO
                                            // pin is controlled by the GPIODR4R
                                            // or GPIODR8R register. 0 Setting a
                                            // bit in either the GPIODR4
                                            // register or the GPIODR8 register
                                            // clears the corresponding 2-mA
                                            // enable bit. The change is
                                            // effective on the second clock
                                            // cycle after the write if
                                            // accessing GPIO via the APB memory
                                            // aperture. If using AHB access@@
                                            // the change is effective on the
                                            // next clock cycle.
#define GPIO_GPIO_DR2R_DRV2_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DR4R register.
//
//******************************************************************************
#define GPIO_GPIO_DR4R_DRV4_M   0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Output Pad
                                            // 4-mA Drive Enable Value
                                            // Description 1 The corresponding
                                            // GPIO pin has 4-mA drive. The
                                            // drive for the corresponding GPIO
                                            // pin is controlled by the GPIODR2R
                                            // or GPIODR8R register. 0 Setting a
                                            // bit in either the GPIODR2
                                            // register or the GPIODR8 register
                                            // clears the corresponding 4-mA
                                            // enable bit. The change is
                                            // effective on the second clock
                                            // cycle after the write if
                                            // accessing GPIO via the APB memory
                                            // aperture. If using AHB access@@
                                            // the change is effective on the
                                            // next clock cycle.
#define GPIO_GPIO_DR4R_DRV4_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DR8R register.
//
//******************************************************************************
#define GPIO_GPIO_DR8R_DRV8_M   0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Output Pad
                                            // 8-mA Drive Enable Value
                                            // Description 1 The corresponding
                                            // GPIO pin has 8-mA drive. The
                                            // drive for the corresponding GPIO
                                            // pin is controlled by the GPIODR2R
                                            // or GPIODR4R register. 0 Setting a
                                            // bit in either the GPIODR2
                                            // register or the GPIODR4 register
                                            // clears the corresponding 8-mA
                                            // enable bit. The change is
                                            // effective on the second clock
                                            // cycle after the write if
                                            // accessing GPIO via the APB memory
                                            // aperture. If using AHB access@@
                                            // the change is effective on the
                                            // next clock cycle.
#define GPIO_GPIO_DR8R_DRV8_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_ODR register.
//
//******************************************************************************
#define GPIO_GPIO_ODR_ODE_M     0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Output Pad
                                            // Open Drain Enable Value
                                            // Description 1 The corresponding
                                            // pin is configured as open drain.
                                            // 0 The corresponding pin is not
                                            // configured as open drain.
#define GPIO_GPIO_ODR_ODE_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_PUR register.
//
//******************************************************************************
#define GPIO_GPIO_PUR_PUE_M     0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Pad Weak
                                            // Pull-Up Enable Value Description
                                            // 1 The corresponding pin has a
                                            // weak pull-up resistor. 0 The
                                            // corresponding pin is not
                                            // affected. Setting a bit in the
                                            // GPIOPDR register clears the
                                            // corresponding bit in the GPIOPUR
                                            // register. The change is effective
                                            // on the second clock cycle after
                                            // the write if accessing GPIO via
                                            // the APB memory aperture. If using
                                            // AHB access@@ the change is
                                            // effective on the next clock
                                            // cycle.
#define GPIO_GPIO_PUR_PUE_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_PDR register.
//
//******************************************************************************
#define GPIO_GPIO_PDR_PDE_M     0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Pad Weak
                                            // Pull-Down Enable Value
                                            // Description 1 The corresponding
                                            // pin has a weak pull-down
                                            // resistor. 0 The corresponding pin
                                            // is not affected. Setting a bit in
                                            // the GPIOPUR register clears the
                                            // corresponding bit in the GPIOPDR
                                            // register. The change is effective
                                            // on the second clock cycle after
                                            // the write if accessing GPIO via
                                            // the APB memory aperture. If using
                                            // AHB access@@ the change is
                                            // effective on the next clock
                                            // cycle.
#define GPIO_GPIO_PDR_PDE_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_SLR register.
//
//******************************************************************************
#define GPIO_GPIO_SLR_SRL_M     0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Slew Rate
                                            // Limit Enable (8-mA drive only)
                                            // Value Description 1 Slew rate
                                            // control is enabled for the
                                            // corresponding pin. 0 Slew rate
                                            // control is disabled for the
                                            // corresponding pin.
#define GPIO_GPIO_SLR_SRL_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_DEN register.
//
//******************************************************************************
#define GPIO_GPIO_DEN_DEN_M     0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Digital Enable
                                            // Value Description 0 The digital
                                            // functions for the corresponding
                                            // pin are disabled. 1 The digital
                                            // functions for the corresponding
                                            // pin are enabled.
#define GPIO_GPIO_DEN_DEN_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_LOCK register.
//
//******************************************************************************
#define GPIO_GPIO_LOCK_LOCK_M   0xFFFFFFFF  // This register is not used in
                                            // cc3xx. GPIO Lock A write of the
                                            // value 0x4C4F.434B unlocks the
                                            // GPIO Commit (GPIOCR) register for
                                            // write access.A write of any other
                                            // value or a write to the GPIOCR
                                            // register reapplies the lock@@
                                            // preventing any register updates.
                                            // A read of this register returns
                                            // the following values: Value
                                            // Description 0x1 The GPIOCR
                                            // register is locked and may not be
                                            // modified. 0x0 The GPIOCR register
                                            // is unlocked and may be modified.
#define GPIO_GPIO_LOCK_LOCK_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_CR register.
//
//******************************************************************************
#define GPIO_GPIO_CR_CR_M       0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) GPIO Commit
                                            // Value Description The
                                            // corresponding GPIOAFSEL@@
                                            // GPIOPUR@@ GPIOPDR@@ or GPIODEN
                                            // bits can be written. 1 The
                                            // corresponding GPIOAFSEL@@
                                            // GPIOPUR@@ GPIOPDR@@ or GPIODEN
                                            // bits cannot be written. 0 Note:
                                            // The default register type for the
                                            // GPIOCR register is RO for all
                                            // GPIO pins with the exception of
                                            // the NMI pin and the four JTAG/SWD
                                            // pins (PD7@@ PF0@@ and PC[3:0]).
                                            // These six pins are the only GPIOs
                                            // that are protected by the GPIOCR
                                            // register. Because of this@@ the
                                            // register type for GPIO Port D7@@
                                            // GPIO Port F0@@ and GPIO Port
                                            // C[3:0] is R/W. The default reset
                                            // value for the GPIOCR register is
                                            // 0x0000.00FF for all GPIO pins@@
                                            // with the exception of the NMI pin
                                            // and the four JTAG/SWD pins (PD7@@
                                            // PF0@@ and PC[3:0]). To ensure
                                            // that the JTAG port is not
                                            // accidentally programmed as GPIO
                                            // pins@@ the PC[3:0] pins default
                                            // to non-committable. Similarly@@
                                            // to ensure that the NMI pin is not
                                            // accidentally programmed as a GPIO
                                            // pin@@ the PD7 and PF0 pins
                                            // default to non-committable.
                                            // Because of this@@ the default
                                            // reset value of GPIOCR for GPIO
                                            // Port C is 0x0000.00F0@@ for GPIO
                                            // Port D is 0x0000.007F@@ and for
                                            // GPIO Port F is 0x0000.00FE.
#define GPIO_GPIO_CR_CR_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_AMSEL register.
//
//******************************************************************************
#define GPIO_GPIO_AMSEL_GPIO_AMSEL_M \
                                0x000000FF  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) GPIO Analog
                                            // Mode Select Value Description 1
                                            // The analog function of the pin is
                                            // enabled@@ the isolation is
                                            // disabled@@ and the pin is capable
                                            // of analog functions. 0 The analog
                                            // function of the pin is disabled@@
                                            // the isolation is enabled@@ and
                                            // the pin is capable of digital
                                            // functions as specified by the
                                            // other GPIO configuration
                                            // registers. Note: This register
                                            // and bits are only valid for GPIO
                                            // signals that share analog
                                            // function through a unified I/O
                                            // pad. The reset state of this
                                            // register is 0 for all signals.

#define GPIO_GPIO_AMSEL_GPIO_AMSEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_PCTL register.
//
//******************************************************************************
#define GPIO_GPIO_PCTL_PMC7_M   0xF0000000  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 7 This field controls the
                                            // configuration for GPIO pin 7.
#define GPIO_GPIO_PCTL_PMC7_S   28
#define GPIO_GPIO_PCTL_PMC6_M   0x0F000000  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 6 This field controls the
                                            // configuration for GPIO pin 6.
#define GPIO_GPIO_PCTL_PMC6_S   24
#define GPIO_GPIO_PCTL_PMC5_M   0x00F00000  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 5 This field controls the
                                            // configuration for GPIO pin 5.
#define GPIO_GPIO_PCTL_PMC5_S   20
#define GPIO_GPIO_PCTL_PMC4_M   0x000F0000  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 4 This field controls the
                                            // configuration for GPIO pin 4.
#define GPIO_GPIO_PCTL_PMC4_S   16
#define GPIO_GPIO_PCTL_PMC3_M   0x0000F000  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 43 This field controls
                                            // the configuration for GPIO pin 3.
#define GPIO_GPIO_PCTL_PMC3_S   12
#define GPIO_GPIO_PCTL_PMC1_M   0x00000F00  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 1 This field controls the
                                            // configuration for GPIO pin 1.
#define GPIO_GPIO_PCTL_PMC1_S   8
#define GPIO_GPIO_PCTL_PMC2_M   0x000000F0  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 2 This field controls the
                                            // configuration for GPIO pin 2.
#define GPIO_GPIO_PCTL_PMC2_S   4
#define GPIO_GPIO_PCTL_PMC0_M   0x0000000F  // This register is not used in
                                            // cc3xx. equivalant register exsist
                                            // outside GPIO IP (refer
                                            // PAD*_config register in the
                                            // shared comn space) Port Mux
                                            // Control 0 This field controls the
                                            // configuration for GPIO pin 0.
#define GPIO_GPIO_PCTL_PMC0_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_ADCCTL register.
//
//******************************************************************************
#define GPIO_GPIO_ADCCTL_ADCEN_M \
                                0x000000FF  // This register is not used in
                                            // cc3xx. ADC trigger via GPIO is
                                            // not supported. ADC Trigger Enable
                                            // Value Description 1 The
                                            // corresponding pin is used to
                                            // trigger the ADC. 0 The
                                            // corresponding pin is not used to
                                            // trigger the ADC.

#define GPIO_GPIO_ADCCTL_ADCEN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_DMACTL register.
//
//******************************************************************************
#define GPIO_GPIO_DMACTL_DMAEN_M \
                                0x000000FF  // This register is not used in the
                                            // cc3xx. Alternate register to
                                            // support this feature is coded in
                                            // the APPS_NWP_CMN space. refer
                                            // register as offset 0x400F70D8
                                            // ?DMA Trigger Enable Value
                                            // Description 1 The corresponding
                                            // pin is used to trigger the ?DMA.
                                            // 0 The corresponding pin is not
                                            // used to trigger the ?DMA.

#define GPIO_GPIO_DMACTL_DMAEN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIO_SI register.
//
//******************************************************************************
#define GPIO_GPIO_SI_SUM        0x00000001  // Summary Interrupt Value
                                            // Description 1 Each pin has its
                                            // own interrupt vector. 0 All port
                                            // pin interrupts are OR'ed together
                                            // to produce a summary interrupt.
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID4 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID4_PID4_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [7:0]

#define GPIO_GPIO_PERIPHID4_PID4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID5 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID5_PID5_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [15:8]

#define GPIO_GPIO_PERIPHID5_PID5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID6 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID6_PID6_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [23:16]

#define GPIO_GPIO_PERIPHID6_PID6_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID7 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID7_PID7_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [31:24]

#define GPIO_GPIO_PERIPHID7_PID7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID0 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID0_PID0_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [7:0] Can be used by
                                            // software to identify the presence
                                            // of this peripheral.

#define GPIO_GPIO_PERIPHID0_PID0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID1 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID1_PID1_M \
                                0x000000FF  // GPIO Peripheral ID Register
                                            // [15:8] Can be used by software to
                                            // identify the presence of this
                                            // peripheral.

#define GPIO_GPIO_PERIPHID1_PID1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID2 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID2_PID2_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX.v GPIO Peripheral ID
                                            // Register [23:16] Can be used by
                                            // software to identify the presence
                                            // of this peripheral.

#define GPIO_GPIO_PERIPHID2_PID2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PERIPHID3 register.
//
//******************************************************************************
#define GPIO_GPIO_PERIPHID3_PID3_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO Peripheral ID
                                            // Register [31:24] Can be used by
                                            // software to identify the presence
                                            // of this peripheral.

#define GPIO_GPIO_PERIPHID3_PID3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PCELLID0 register.
//
//******************************************************************************
#define GPIO_GPIO_PCELLID0_CID0_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO PrimeCell ID Register
                                            // [7:0] Provides software a
                                            // standard cross-peripheral
                                            // identification system.

#define GPIO_GPIO_PCELLID0_CID0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PCELLID1 register.
//
//******************************************************************************
#define GPIO_GPIO_PCELLID1_CID1_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO PrimeCell ID Register
                                            // [15:8] Provides software a
                                            // standard cross-peripheral
                                            // identification system.

#define GPIO_GPIO_PCELLID1_CID1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PCELLID2 register.
//
//******************************************************************************
#define GPIO_GPIO_PCELLID2_CID2_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO PrimeCell ID Register
                                            // [23:16] Provides software a
                                            // standard cross-peripheral
                                            // identification system.

#define GPIO_GPIO_PCELLID2_CID2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPIO_O_GPIO_PCELLID3 register.
//
//******************************************************************************
#define GPIO_GPIO_PCELLID3_CID3_M \
                                0x000000FF  // This register is not used in
                                            // CC3XX. GPIO PrimeCell ID Register
                                            // [31:24] Provides software a
                                            // standard cross-peripheral
                                            // identification system.

#define GPIO_GPIO_PCELLID3_CID3_S 0



#endif // __HW_GPIO_H__
