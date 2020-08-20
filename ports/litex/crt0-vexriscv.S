.global main
.global isr

.section .text.start
.global _start

_start:
  j crt_init
  # This sentinal ensures that this program is loaded
  # to RAM when loaded using dfu-util.
  #.word 0x17ab0f23
  #.word 0x10001000

.section .ramtext
.global  trap_entry
.align 4
trap_entry:
  sw x1,  - 1*4(sp)
  sw x5,  - 2*4(sp)
  sw x6,  - 3*4(sp)
  sw x7,  - 4*4(sp)
  sw x10, - 5*4(sp)
  sw x11, - 6*4(sp)
  sw x12, - 7*4(sp)
  sw x13, - 8*4(sp)
  sw x14, - 9*4(sp)
  sw x15, -10*4(sp)
  sw x16, -11*4(sp)
  sw x17, -12*4(sp)
  sw x28, -13*4(sp)
  sw x29, -14*4(sp)
  sw x30, -15*4(sp)
  sw x31, -16*4(sp)
  addi sp,sp,-16*4
  call isr
  lw x1 , 15*4(sp)
  lw x5,  14*4(sp)
  lw x6,  13*4(sp)
  lw x7,  12*4(sp)
  lw x10, 11*4(sp)
  lw x11, 10*4(sp)
  lw x12,  9*4(sp)
  lw x13,  8*4(sp)
  lw x14,  7*4(sp)
  lw x15,  6*4(sp)
  lw x16,  5*4(sp)
  lw x17,  4*4(sp)
  lw x28,  3*4(sp)
  lw x29,  2*4(sp)
  lw x30,  1*4(sp)
  lw x31,  0*4(sp)
  addi sp,sp,16*4
  mret

.text

crt_init:
  # # Flush the caches
  # .word 16399
  # .word 19
  # .word 19
  # .word 19
  la sp, _estack - 4
  la a0, trap_entry
  csrw mtvec, a0

bss_init:
  la a0, _sbss
  la a1, _ebss
bss_loop:
  beq a0,a1,bss_done
  sw zero,0(a0)
  add a0,a0,4
  j bss_loop
bss_done:

  /* Load DATA */
  la t0, _sidata
  la t1, _sdata
  la t2, _edata
3:
  lw t3, 0(t0)
  sw t3, 0(t1)
  /* _edata is aligned to 16 bytes. Use word-xfers. */
  addi t0, t0, 4
  addi t1, t1, 4
  bltu t1, t2, 3b

  li a0, 0x880  //880 enable timer + external interrupt sources (until mstatus.MIE is set, they will never trigger an interrupt)
  csrw mie,a0

  call main
infinite_loop:
  j infinite_loop
