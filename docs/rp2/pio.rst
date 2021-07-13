.. _rp2_pio:

RP2040 has standard hardware support like I2C, SPI and UART for communication
protocols with standard devices. For devices where we do not have hardware
support for such protocols or where there is a requirement of more I/O
channels, PIO(Programmable Input Ouput) comes intp play. Also, typical
micropython devices make use of a technique called bit banging which is pins
are rapidly turned on and off to transmit data. This makes the entire process
very slow as the processor concentrates on bit banging rather than exceuting
any other process. However, PIO sllows it to carry on with the bit banging
process while the processor resources are used for executing some other work. 

Along with the two main cotex - MO+ processing cores, RP2040 has two PIO
blocks each of which has four state machines. These state machines take
process data in FIFO (FIrst-In-First-Out) format. When these state machines
are free, they will put in data from the queue and execute the instruction.
This way, the PIO state machines and the main process works simultaneously
without being much dependant on the other. 

Eaxh FIFO has four words (each of 32 bits) which can be linked to the DMA toin assemby
transmit larger amounts of data to the PIO state machine and allows it to
carry on execution without the main program keeping a track of it at each
instant of time.

Each state machine has a set of instructions- which allow for a wide range of
behaviourial or location changes to the data. They are written in assembly
language which is later used in a MicroPython program to perform specific
tasks. These instructions are -

jmp(cond, lebel=None)
	condition : 0-7
	<delay_value> : 0-31
	target : instruction offset of label within the program
    see sec 3.4.2 of RPi docs for details

wait(polarity, src, index)
     polarity: 0-1
     src: gpio, pin, irq
     index: 0-31
     wait for high/low on pin 0-31 or irq 0-31
     see sec 3.4.3 of RPi docs for details

in(src, data)
	src : pin, X, Y, ISR, OSR(all 0-31)
	data : bit count(0-31)
    see sec 3.4.4 of RPi docs for details

out(dist,data)
	dist : pin, X, Y, PINDIRS, AC, ISR, OSR(all 0-31)
	data : bit count(0-31)
    see sec 3.4.5 of RPi docs for details

push(value = 0, value = 2)
	block = 0 : no block
    block = 1 : block
    see sec 3.4.6 of RPi docs for details

pull(value = 0, value = 2)
	pull(if empty) : 1 (default = 0)
	block : 1 (default = 1)  
	no-block = 0
    see sec 3.4.7 of RPi docs for details

mov(dist, src)
	dist : pin, X, Y, ISR, OSR, PC (all 0-31)
	src : pin, X, Y, ISR, OSR (all 0-31)
    see sec 3.4.8 of RPi docs for details

set(dest, data)
	dest : X, Y, PINS, PINDIRS
	data : value (0-31)
    see sec 3.4.10 of RPi docs for details

irq(mod, index = None)
	These are the following forms of this instruction(index is an integer value 
	from 0-7)
        * irq(index)
	* irq(rel(index))
	* irq(block, index)
	* irq(block, rel(index))
	* irq(clear, rel(index))
	* irq(block | clear, index)
	* irq(block | clear, rel(index))
	
	These can be summarised as two forms:
	* form 1: irq(index)
	* form 2: irq(mode,index)
	  where
	  * index can be an integer or rel(integer)
	  * mode can be: block or clear or block | clear
	  
	Allowed IRQ numbers are 0-7 (0-3 are visible from to the processor, 4-7 are 
	internal to the state machines).          
   see sec 3.4.9 of RPi docs for details




