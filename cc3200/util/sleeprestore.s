    .syntax unified
    .cpu cortex-m4
    .thumb
    .text
    .align  2

@ global variable with the backup registers
    .extern vault_arm_registers
@ global function that performs the wake up actions
    .extern pyb_sleep_suspend_exit

@ uint sleep_store(void)
    .global sleep_store
    .thumb
    .thumb_func
    .type   sleep_store, %function
sleep_store:
    dsb
    isb
    push {r0-r12, lr}
    ldr  r1, =vault_arm_registers
    mrs  r0, msp
    str  r0, [r1]
    mrs  r0, psp
    str  r0, [r1, #4]
    mrs  r0, primask
    str  r0, [r1, #12]
    mrs  r0, faultmask
    str  r0, [r1, #16]
    mrs  r0, basepri
    str  r0, [r1, #20]
    mrs  r0, control
    str  r0, [r1, #24]
    dsb
    isb
    bx   lr

@ uint sleep_restore(void)
    .global sleep_restore
    .thumb
    .thumb_func
    .type   sleep_restore, %function
sleep_restore:
    dsb
    isb
    mrs  r0, msp
    msr  psp, r0
    ldr  r1, =vault_arm_registers
    ldr  r0, [r1, #24]
    msr  control, r0
    ldr  r0, [r1]
    msr  msp, r0
    ldr  r0, [r1, #12]
    msr  primask, r0
    ldr  r0, [r1, #16]
    msr  faultmask, r0
    ldr  r0, [r1, #20]
    msr  basepri, r0
    dsb
    isb
    bl   pyb_sleep_suspend_exit
