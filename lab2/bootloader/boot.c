#include "mini_uart.h"

extern unsigned long _dtb_ptr;

void uart_send_size(unsigned int size) {
    unsigned char *ptr = (unsigned char *)&size;
    uart_send_string("2 start send number\n");
    uart_send(ptr[0]);
    uart_send(ptr[1]);
    uart_send(ptr[2]);
    uart_send(ptr[3]);
    uart_send_string("2 end send number\n");
}

void complete_sending() {
    uart_send_string("kernel loaded! start jumping...\n");
    return;
}

static void wait_for_magic(void) {
    const unsigned char magic[4] = {'K', 'E', 'R', 'N'};
    unsigned int matched = 0;
    while (matched < 4) {
        unsigned char c = uart_recv();
        if (c == magic[matched]) {
            matched++;
        } else {
            matched = 0;
            if (c == magic[0]) {
                matched = 1;
            }
        }
    }
}
int main() {
    uart_init();
    unsigned int size = 0;
    
    
    void *dtb_ptr = (void*)_dtb_ptr;
    unsigned long addr = _dtb_ptr;
    uart_send_string("dtb address: ");
    uart_send_hex(addr);
    uart_send_string("\n");


    wait_for_magic();
    // uart_send_string("READY\n");
    size |= (unsigned int)uart_recv()<<0;
    size |= (unsigned int)uart_recv()<<8;
    size |= (unsigned int)uart_recv()<<16;
    size |= (unsigned int)uart_recv()<<24;

    unsigned char *ptr = (unsigned char *)&size;
    uart_send(ptr[0]);
    uart_send(ptr[1]);
    uart_send(ptr[2]);
    uart_send(ptr[3]);
    
    wait_for_magic();
    char *kernel_dest = (char*)0x80000;
    for (unsigned int i=0; i<size; i++) {
        kernel_dest[i] = uart_recv();
    }
    uart_send_string("BOOT");
    return 0;
}