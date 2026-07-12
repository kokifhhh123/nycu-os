#include "mini_uart.h"

void uart_send_size(unsigned int size) {
    unsigned char *ptr = (unsigned char *)&size;
    uart_send(ptr[0]);
    uart_send(ptr[1]);
    uart_send(ptr[2]);
    uart_send(ptr[3]);
}

int main() {
    uart_init();
    // uart_send_string("gg\n");

    unsigned int size = 0;
    size |= (unsigned int)uart_recv()<<0;
    size |= (unsigned int)uart_recv()<<8;
    size |= (unsigned int)uart_recv()<<16;
    size |= (unsigned int)uart_recv()<<24;
    
    // char *kernel_dest = (char*)0x80000;
    // for (unsigned int i=0; i<size; i++) {
    //     kernel_dest[i] = uart_recv();
    // }
    // uart_send_string("kernel loaded! start jumping...\n");
    uart_send_string("gg\n");
    uart_send_size(size);
}