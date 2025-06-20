from machine import Pin
import micropython,pyb,gc
pyb.info()
freqs = machine.freq() # Get the system clock frequencies

sysclk, hclk, pclk1, pclk2 = freqs # Unpack the tuple into separate clock variables

print('-----------------------------')
print(f"System Clock (SYSCLK): {sysclk / 1_000_000} MHz")
print(f"AHB Clock (HCLK): {hclk / 1_000_000} MHz")
print(f"APB1 Peripheral Clock (PCLK1): {pclk1 / 1_000_000} MHz")
print(f"APB2 Peripheral Clock (PCLK2): {pclk2 / 1_000_000} MHz")
print('-----------------------------')
gc.collect()
micropython.mem_info()
print('-----------------------------')
print('Initial free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
def func():
    a = bytearray(10000)
gc.collect()
print('Func definition: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
func()
print('Func run free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
gc.collect()
print('Garbage collect free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
print('-----------------------------')
micropython.mem_info(1)