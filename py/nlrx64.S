#ifdef __x86_64__
/* x64 callee save: bx, bp, sp, r12, r14, r14, r15 */

    .file   "nlr.s"
    .text

/* uint nlr_push(rdi=nlr_buf_t *nlr) */
    .globl  nlr_push
    .type   nlr_push, @function
nlr_push:
    movq    (%rsp), %rax            # load return %rip
    movq    %rax, 16(%rdi)          # store %rip into nlr_buf
    movq    %rbp, 24(%rdi)          # store %rbp into nlr_buf
    movq    %rsp, 32(%rdi)          # store %rsp into nlr_buf
    movq    %rbx, 40(%rdi)          # store %rbx into nlr_buf
    movq    %r12, 48(%rdi)          # store %r12 into nlr_buf
    movq    %r13, 56(%rdi)          # store %r13 into nlr_buf
    movq    %r14, 64(%rdi)          # store %r14 into nlr_buf
    movq    %r15, 72(%rdi)          # store %r15 into nlr_buf
    movq    nlr_top(%rip), %rax     # get last nlr_buf
    movq    %rax, (%rdi)            # store it
    movq    %rdi, nlr_top(%rip)     # stor new nlr_buf (to make linked list)
    xorq    %rax, %rax              # return 0, normal return
    ret                             # return
    .size   nlr_push, .-nlr_push

/* void nlr_pop() */
    .globl  nlr_pop
    .type   nlr_pop, @function
nlr_pop:
    movq    nlr_top(%rip), %rax     # get nlr_top into %rax
    movq    (%rax), %rax            # load prev nlr_buf
    movq    %rax, nlr_top(%rip)     # store prev nlr_buf (to unlink list)
    ret                             # return
    .size   nlr_pop, .-nlr_pop

/* void nlr_jump(rdi=uint val) */
    .globl  nlr_jump
    .type   nlr_jump, @function
nlr_jump:
    movq    %rdi, %rax              # put return value in %rax
    movq    nlr_top(%rip), %rdi     # get nlr_top into %rdi
    movq    %rax, 8(%rdi)           # store return value
    movq    (%rdi), %rax            # load prev nlr_buf
    movq    %rax, nlr_top(%rip)     # store prev nlr_buf (to unlink list)
    movq    72(%rdi), %r15          # load saved %r15
    movq    64(%rdi), %r14          # load saved %r14
    movq    56(%rdi), %r13          # load saved %r13
    movq    48(%rdi), %r12          # load saved %r12
    movq    40(%rdi), %rbx          # load saved %rbx
    movq    32(%rdi), %rsp          # load saved %rsp
    movq    24(%rdi), %rbp          # load saved %rbp
    movq    16(%rdi), %rax          # load saved %rip
    movq    %rax, (%rsp)            # store saved %rip to stack
    xorq    %rax, %rax              # clear return register
    inc     %al                     # increase to make 1, non-local return
    ret                             # return
    .size   nlr_jump, .-nlr_jump

    .local  nlr_top
    .comm   nlr_top,8,8
#endif
