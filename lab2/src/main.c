#include "shell.h"

extern unsigned long _dtb_ptr;

int main() {
    uart_init();
    unsigned char* cpio_addr = fdt_traverse();
    // unsigned char* cpio_addr = (unsigned char*)0x90;
    shell(cpio_addr);
}