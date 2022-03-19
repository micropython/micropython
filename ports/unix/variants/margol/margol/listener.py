from binascii import hexlify  
from ctypes import addressof  
from socket import socket, SOCK_RAW, SOL_SOCKET  
from struct import pack, unpack  
  
  
AF_PACKET = 17

# A subset of Berkeley Packet Filter constants and macros, as defined in  
# linux/filter.h.  
  
# Instruction classes  
BPF_LD = 0x00  
BPF_JMP = 0x05  
BPF_RET = 0x06  
  
# ld/ldx fields  
BPF_H = 0x08  
BPF_B = 0x10  
BPF_ABS = 0x20  
  
# alu/jmp fields  
BPF_JEQ = 0x10  
BPF_K = 0x00  
  
def bpf_jump(code, k, jt, jf):  
    return pack('HBBI', code, jt, jf, k)  
  
def bpf_stmt(code, k):  
    return bpf_jump(code, k, 0, 0)  
  
  
# Ordering of the filters is backwards of what would be intuitive for   
# performance reasons: the check that is most likely to fail is first.  
filters_list = [  
    # Must have dst port 9999. Load (BPF_LD) a half word value (BPF_H) in   
    # ethernet frame at absolute byte offset 36 (BPF_ABS). If value is equal to  
    # 9999 then do not jump, else jump 5 statements.  
    bpf_stmt(BPF_LD | BPF_H | BPF_ABS, 36),  
    bpf_jump(BPF_JMP | BPF_JEQ | BPF_K, 9999, 0, 5),  
  
    # Must be UDP (check protocol field at byte offset 23)  
    bpf_stmt(BPF_LD | BPF_B | BPF_ABS, 23),   
    bpf_jump(BPF_JMP | BPF_JEQ | BPF_K, 0x11, 0, 3),  
  
    # Must be IPv4 (check ethertype field at byte offset 12)  
    bpf_stmt(BPF_LD | BPF_H | BPF_ABS, 12),   
    bpf_jump(BPF_JMP | BPF_JEQ | BPF_K, 0x0800, 0, 1),  
  
    bpf_stmt(BPF_RET | BPF_K, 0x0fffffff), # pass  
    bpf_stmt(BPF_RET | BPF_K, 0), # reject  
]  
  
# Create filters struct and fprog struct to be used by SO_ATTACH_FILTER, as  
# defined in linux/filter.h.
filters = b''.join(filters_list)  
mem_addr_of_filters = addressof(filters)  
fprog = pack('HL', len(filters_list), mem_addr_of_filters)  

# As defined in asm/socket.h  
SO_ATTACH_FILTER = 26
  
# Create listening socket with filters  
s = socket(AF_PACKET, SOCK_RAW, 0x0800)
s.setsockopt(SOL_SOCKET, SO_ATTACH_FILTER, fprog)  
s.bind(('ens33', 0x0800))  
  
while True:  
    data, addr = s.recvfrom(65565)  
    print('got data from', addr, ':', hexlify(data))
