    .global start
    .global stack
    .global reset

    .text
    .thumb
    .align 0

start:
    @
    @ Insert code
    @
    
    @ Fix for SAM-BA stack bug
    ldr     r0, reset
    cmp     r0, #0
    bne     return
    ldr     r0, stack
    mov     sp, r0

return:
    bx      lr

    .align  0
stack:
    .word   0
reset:
    .word   0
    @
    @ Insert variables
    @
    