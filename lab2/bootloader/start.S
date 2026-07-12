.section ".text.boot"
.global _start

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f

1:  wfe
    b 1b
2:  
    ldr x1, =_start
    mov sp, x1

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    lsr x2, x2, #3
3:  cbz x2, 4f
    str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, 3b

4:  bl main
    mov x0, #0x80000
    br x0

    b 1b