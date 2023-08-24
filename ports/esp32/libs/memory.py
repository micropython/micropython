from machine import mem32


ULP_MEM_BASE = 0x50000000
ULP_DATA_MASK = 0xffff


def write_rtc_mem(address, value):
    mem32[ULP_MEM_BASE+address] = value & 0xffff

def read_rtc_mem(address):
    return mem32[ULP_MEM_BASE+address] & 0xffff
