#include "mini_uart.h"

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

static void wait_for_magic(void)
{
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
    // uart_send_string("gg\n");
    // uart_send_string("READY_SIZE\n");

    unsigned int size = 0;
    // char *tmp = (char*)0x20000;
    // int corr = 0;
    // int i = 0;
    // while(1) {
    //     tmp[i] = uart_recv();
    //     if(i>=3) {
    //         if(tmp[i-3]=='K' && tmp[i-1]=='E' && tmp[i-2]=='R' && tmp[i-3]=='N') {
    //             break;
    //         }
    //     }
    //     i++;
    // }
    wait_for_magic();
    // uart_send_string("READY\n");
    size |= (unsigned int)uart_recv()<<0;
    size |= (unsigned int)uart_recv()<<8;
    size |= (unsigned int)uart_recv()<<16;
    size |= (unsigned int)uart_recv()<<24;
    // size = 2278;

    // uart_send_string("READY\n");

    unsigned char *ptr = (unsigned char *)&size;
    uart_send(ptr[0]);
    uart_send(ptr[1]);
    uart_send(ptr[2]);
    uart_send(ptr[3]);
    
    wait_for_magic();
    char *kernel_dest = (char*)0x20000;
    for (unsigned int i=0; i<size; i++) {
        kernel_dest[i] = uart_recv();
        // kernel_dest[i] = 0;
    }
    // // complete_sending();
    uart_send_string("BOOT");
    // uart_send_string("kernel loaded! start jumping...\n");
    return 0;
    // uart_send_string("gg\n");
}