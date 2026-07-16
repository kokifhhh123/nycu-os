.section ".text.boot"
.global _start

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, compare
1:  wfe
    b 1b

compare:
    adr x2, _start
    ldr x1, =_start
    cmp x2, x1
    b.eq relocate

2:  
    ldr x1, =__stack_top
    mov sp, x1

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    lsr x2, x2, #3
3:  
    cbz x2, 4f
    str xzr, [x1], #8
    sub x2, x2, #1
    b   3b

4:  bl main

    # ldr x2, =_dtb_ptr
    # ldr x0, [x2]
    mov x0, x19

    mov x1, #0x80000
    br x1


relocate:
    # ldr x1, =_dtb_ptr
    # str x0, [x1]
    mov x19, x0
    
    ldr x1, =__rel_start
    ldr x2, =__rel_size
    mov x0, #0x60000
    lsr x2, x2, #3
kk:
    ldr x3, [x1], #8
    str x3, [x0], #8
    sub x2, x2, #1
    cbnz x2, kk
    ldr x4, =0x60000
    br x4

.section .data
.align 3
.global _dtb_ptr
_dtb_ptr: .quad 0