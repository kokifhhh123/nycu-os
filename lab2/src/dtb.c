#include "dtb.h"

extern unsigned long _dtb_ptr;

void initramfs_callback() {
}

void fdt_traverse() {
    void *dtb_ptr = (void*)_dtb_ptr;
    unsigned long addr = _dtb_ptr;
    uart_send_string("dtb address: ");
    uart_send_hex(addr);
    uart_send_string("\n");
}