"""
This implementation offloads the quadrature sensor continuous pooling to the StateMachine pio_quadrature,
pio_quadrature will detect state changes and push the values then raises an interrupt that is handled by
encoder_state_changed_irq_handler

encoder_state_changed_irq_handler was inspired from 
https://electronics.stackexchange.com/questions/360637/quadrature-encoder-most-efficient-software-implementation

"""

from machine import Pin
from rp2 import PIO, StateMachine
import utime
import array


state_look_up_table = array.array("b", [
        #Direction = 1
        0, # 00 to 00
       -1, # 00 to 01
       +1, # 00 to 10
       +2, # 00 to 11

       +1, # 01 to 00
        0, # 01 to 01
       +2, # 01 to 10
       -1, # 01 to 11

       -1, # 10 to 00
       +2, # 10 to 01
        0, # 10 to 10
       +1, # 10 to 11

       +2, # 11 to 00
       +1, # 11 to 01
       -1, # 11 to 10
        0, # 11 to 11

        #Direction = 0
        0, # 00 to 00
       -1, # 00 to 01
       +1, # 00 to 10
       -2, # 00 to 11

       +1, # 01 to 00
        0, # 01 to 01
       -2, # 01 to 10
       -1, # 01 to 11

       -1, # 10 to 00
       -2, # 10 to 01
        0, # 10 to 10
       +1, # 10 to 11

       -2, # 11 to 00
       +1, # 11 to 01
       -1, # 11 to 10
        0, # 11 to 11
    ])


counter = 0
direction = 0
lut_index = 0

def encoder_state_changed_irq_handler(sm):
    global counter, direction, lut_index
    lut_index |= sm.get() & 3
    counter += state_look_up_table[lut_index]
    if state_look_up_table[lut_index] != 0:
        direction = 1 if (state_look_up_table[lut_index] > 0) else 0
    lut_index = ((lut_index << 2) & 0b1100) | (direction << 4)
 
 
@rp2.asm_pio()
def pio_quadrature(in_init=rp2.PIO.IN_LOW):
    wrap_target()
    label("again")
    in_(pins, 2)
    mov(x, isr)
    jmp(x_not_y, "push_data")
    mov(isr, null)
    jmp("again")
    label("push_data")    
    push()
    irq(block, rel(0))
    mov(y, x)
    wrap()
    
    
 
sm = StateMachine(0, pio_quad, freq=160000, in_base=Pin(2))
sm.irq(encoder_state_changed_irq_handler)
sm.exec("set(y, 99)")  # add a last value for y that would be always different then the input
sm.active(1)

while True:    
    utime.sleep(2)
    print(counter)
