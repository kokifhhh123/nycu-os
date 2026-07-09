#include "mini_uart.h"
#include "base.h"

void uart_init() {
    unsigned int r;
    *AUX_ENABLE |= 1;
    *AUX_MU_IER = 0;
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 0b11;  // 8 bits mode
    *AUX_MU_MCR = 0;
    *AUX_MU_IIR = 0b11000110;
    *AUX_MU_BAUD = 270;
    
    r = *GPFSEL1;
    r &= ~((0b111<<12)|(0b111<<15));
    r |= (2<<12)|(2<<15);
    *GPFSEL1 = r;
    *GPPUD = 0;
    r=150; while(r--) {asm volatile("nop");}
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) {asm volatile("nop");}
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL = 0b11;
}

void uart_send(unsigned int c) {
    do {asm volatile("nop");} while(!(*AUX_MU_LSR & 0x20));
    *AUX_MU_IO = c;
}

char uart_recv() {
    char r;
    do {asm volatile("nop");} while(!(*AUX_MU_LSR & 0x01));
    r = (char)(*AUX_MU_IO);
    return r == '\r' ? '\n' : r;
}

void uart_send_string(char *s) {
    while(*s) {
        if (*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

void uart_send_hex(unsigned int d) {
    unsigned int n;
    int c;
    for (c=28; c>=0; c-=4) {
        n = (d>>c)&0xF;
        n += n>9 ? 0x37 : 0x30;
        uart_send(n);
    }
}