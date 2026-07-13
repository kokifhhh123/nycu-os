SECTIONS
{
    . = 0x80000;
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    . = ALIGN(16);
    __bss_start = .;
    .bss : {
        *(.bss);
    }
    __bss_end = .;
    __bss_size = __bss_end - __bss_start;
    __stack_bottom = 0x70000;
    __stack_top = 0x78000;
}