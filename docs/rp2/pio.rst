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

We also have the feature of a pseudo-instruction provided by pioasm:-

* NOP : assembles to mov y and y; used for providing extra-delay

To understand the use of these instrcutions we can take the pio_1hz example for a sinple understanding. Below is the code
of pio_1hz.py for reference. 

.. code-block:: python3

    # Example using PIO to blink an LED and raise an IRQ at 1Hz.

    import time
    from machine import Pin
    import rp2


    @rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
    def blink_1hz():
      # fmt: off
      # Cycles: 1 + 1 + 6 + 32 * (30 + 1) = 1000
      irq(rel(0))
      set(pins, 1)
      set(x, 31)                  [5]
      label("delay_high")
      nop()                       [29]
      jmp(x_dec, "delay_high")

      # Cycles: 1 + 7 + 32 * (30 + 1) = 1000
      set(pins, 0)
      set(x, 31)                  [6]
      label("delay_low")
      nop()                       [29]
      jmp(x_dec, "delay_low")
      # fmt: on


    # Create the StateMachine with the blink_1hz program, outputting on Pin(25).
    sm = rp2.StateMachine(0, blink_1hz, freq=2000, set_base=Pin(25))

    # Set the IRQ handler to print the millisecond timestamp.
    sm.irq(lambda p: print(time.ticks_ms()))

    # Start the StateMachine.
    sm.active(1)

We are creating an object of class StateMachine which will display the output
on pin 25. The 'blink_1hz' program uses the PIO to blink an LED and raise the
IRQ at 1 HZ. Turn the LED on with the help of the set instruction. Set value
31 on register X. Then wait for 30 cycles with the help of nop() instruction.
Then with the help of jmp, go to the code part which has label as 'delay high'
for the instruction offset. The next part of the code teaches us to turn the
LED off by turning the LED off with the help of set instruction. Here the
label is 'delay_low' for the instruction offset part of the jmp instruction. 

We then print the millisecond timestamp by setting the IRQ handler.


