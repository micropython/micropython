/*
 * panic.h
 *
 *  Created on: Jan 31, 2014
 *      Author: david_s5
 */
// Add a panic code def_panic_codes(_class, led, code)
// the panic module will send SOS followed by 900 ms delay
// followed by 300 blinks of the value of code
/// ...---...    code   ...---...

def_panic_codes(Faults,RGB_COLOR_RED,HardFault)
def_panic_codes(Faults,RGB_COLOR_RED,NMIFault)
def_panic_codes(Faults,RGB_COLOR_RED,MemManage)
def_panic_codes(Faults,RGB_COLOR_RED,BusFault)
def_panic_codes(Faults,RGB_COLOR_RED,UsageFault)

def_panic_codes(Cloud,RGB_COLOR_RED,InvalidLenth)

def_panic_codes(System,RGB_COLOR_RED,Exit)
def_panic_codes(System,RGB_COLOR_RED,OutOfHeap)

def_panic_codes(System,RGB_COLOR_RED,SPIOverRun)

def_panic_codes(Software,RGB_COLOR_RED,AssertionFailure)
def_panic_codes(Software,RGB_COLOR_RED,InvalidCase)
def_panic_codes(Software,RGB_COLOR_RED,PureVirtualCall)

