// Implements the gprof profiler arc counting function.
// Created on: 06.08.2015
// Author: Erich Styger
// URL: https://github.com/ErichStyger/mcuoneclipse/tree/master/Examples/KDS/FRDM-K64F120M/FRDM-K64F_Profiling/Profiling
//
// Updated for MicroPython: 12.04.2020
// Author: Jim Mussared

.syntax unified
.cpu cortex-m4
.thumb

.globl __gnu_mcount_nc
.type __gnu_mcount_nc, %function

// GCC's -pg flag will do:
//   push {lr}          (i.e. the caller's addr)
//   bl __gnu_mcount_nc (set {lr} to next instruction, branch to __gnu_mcount_nc)
// at the start of every function.
//

__gnu_mcount_nc:
  push {r0, r1, r2, r3, lr} // And {caller_lr} on the end.
  bic r1, lr, #1            // R1 contains callee address (i.e. next instruction), with thumb bit cleared.
  ldr r0, [sp, #20]         // R0 contains caller address (i.e. the {lr} that was pushed before branch-linking here).
  bic r0, r0, #1            // Clear thumb bit.
  bl _mcount_internal       // Call internal _mcount() implementation.
  pop {r0, r1, r2, r3, ip, lr} // Restore saved registers and put callee address into {ip}, and restore LR from pushed value.
  bx ip                     // Branch directly to the next instruction

