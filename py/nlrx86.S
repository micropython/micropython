#ifdef __i386__
/* x86 callee save: bx, di, si, bp, sp */

    .file   "nlr.s"
    .text

/* uint nlr_push(4(%esp)=nlr_buf_t *nlr) */
#ifdef _WIN32
    .globl  _nlr_push
    .def    _nlr_push; .scl 2; .type 32; .endef
_nlr_push:
#else
    .globl  nlr_push
    .type   nlr_push, @function
nlr_push:
#endif
    mov     4(%esp), %edx           # load nlr_buf
    mov     (%esp), %eax            # load return %ip
    mov     %eax, 8(%edx)           # store %ip into nlr_buf+8
    mov     %ebp, 12(%edx)          # store %bp into nlr_buf+12
    mov     %esp, 16(%edx)          # store %sp into nlr_buf+16
    mov     %ebx, 20(%edx)          # store %bx into nlr_buf+20
    mov     %edi, 24(%edx)          # store %di into nlr_buf
    mov     %esi, 28(%edx)          # store %si into nlr_buf
    mov     nlr_top, %eax           # load nlr_top
    mov     %eax, (%edx)            # store it
    mov     %edx, nlr_top           # stor new nlr_buf (to make linked list)
    xor     %eax, %eax              # return 0, normal return
    ret                             # return
#ifndef _WIN32
    .size   nlr_push, .-nlr_push
#endif

/* void nlr_pop() */
#ifdef _WIN32
    .globl  _nlr_pop
    .def    _nlr_pop; .scl 2; .type 32; .endef
_nlr_pop:
#else
    .globl  nlr_pop
    .type   nlr_pop, @function
nlr_pop:
#endif
    mov     nlr_top, %eax           # load nlr_top
    mov     (%eax), %eax            # load prev nlr_buf
    mov     %eax, nlr_top           # store nlr_top (to unlink list)
    ret                             # return
#ifndef _WIN32
    .size   nlr_pop, .-nlr_pop
#endif

/* void nlr_jump(4(%esp)=uint val) */
#ifdef _WIN32
    .globl  _nlr_jump
    .def    _nlr_jump; .scl 2; .type 32; .endef
_nlr_jump:
#else
    .globl  nlr_jump
    .type   nlr_jump, @function
nlr_jump:
#endif
    mov     nlr_top, %edx           # load nlr_top
    mov     4(%esp), %eax           # load return value
    mov     %eax, 4(%edx)           # store return value
    mov     (%edx), %eax            # load prev nlr_top
    mov     %eax, nlr_top           # store nlr_top (to unlink list)
    mov     28(%edx), %esi          # load saved %si
    mov     24(%edx), %edi          # load saved %di
    mov     20(%edx), %ebx          # load saved %bx
    mov     16(%edx), %esp          # load saved %sp
    mov     12(%edx), %ebp          # load saved %bp
    mov     8(%edx), %eax           # load saved %ip
    mov     %eax, (%esp)            # store saved %ip to stack
    xor     %eax, %eax              # clear return register
    inc     %al                     # increase to make 1, non-local return
    ret                             # return
#ifndef _WIN32
    .size   nlr_jump, .-nlr_jump
#endif

#ifndef _WIN32
    .local  nlr_top
#endif
    .comm   nlr_top,4,4

#endif /* __i386__ */
