.. _rp2_pio:

RP2040 has standard hardware support like I2C, SPI and UART for communication
protocols with standard devices. For devices where we do not have hardware
support for such protocols or where there is a requirement of more I/O
channels, PIO(Programmable Input Ouput) comes intp play. Also, typical
micropython devices make use of a technique called bit banging in which pins
are rapidly turned on and off to transmit data. This makes the entire process
very slow as the processor concentrates on bit banging rather than exceuting
any other process. However, PIO allows it to carry on with the bit banging
process while the processor resources are used for executing some other work. 

Along with the two main cotex - MO+ processing cores, RP2040 has two PIO
blocks each of which has four state machines. These state machines take
process data in FIFO (First-In-First-Out) format. When these state machines
are free, they will put in data from the queue and execute the instruction.
This way, the PIO state machines and the main process works simultaneously
without being much dependant on the other. 

Each FIFO has four words (each of 32 bits) which can be linked to the DMA to
transmit larger amounts of data to the PIO state machine and allows it to
carry on execution without the main program keeping a track of it at each
instant of time.

The state machines have 9 instructions which can perform the following tasks:-

* IN : shifts the bits from a source (scratch register or set of pins) to the
* input shift register.

* OUT : shifts the bits from the output shift register to a destination

* PUSH : sends data to RX FIFO

* PULL : recieves data from TX FIFO

* MOV : moves data from a souce to a destination

* IRQ : sets or clears the input flag

* WAIT : pauses until a particular action happens

* JMP : moves to some target locations in differnt parts of code.





