#include "cpio.h"

// #define cpio_address 0x20000000
// #define cpio_address_hard 0x8000000

static void print() {
    unsigned char *ptr = (unsigned char*)0x8000000;
    unsigned char *p2 = ptr+94;
    unsigned char *p1 = ptr+54;
    char data_size[9];
    char path_size[9];
    data_size[8] = '\0';
    path_size[8] = '\0';
    for (int i=0; i<8; i++) {
        data_size[i] = *p1;
        p1++;
    }
    for (int i=0; i<8; i++) {
        path_size[i] = *p2;
        p2++;
    }
    unsigned int x = hex_to_dec(data_size);
    unsigned int y = hex_to_dec(path_size);
    int num = 16;
    int kk = ((110+y)/num)*num + ((110+y)%num==0 ? 0 : num);
    int hh = ((kk+x)/num)*num + ((kk+x)%num==0 ? 0 : num);
    // int kk = 110+x+y;
    uart_send_hex(y);
    uart_send(' ');
    
    uart_send_hex(x);
    uart_send('\n');
    
    uart_send_hex(kk);
    uart_send(' ');

    uart_send_hex(hh);
    uart_send('\n');

    unsigned char *pp = ptr;
    for (int i=0; i<hh; i++) {
        uart_send(*pp);
        pp++;
    }
    uart_send('\n');

    unsigned char *next = ptr+kk;
    for (int i=0; i<30; i++) {
        uart_send(*next);
        next++;
    }
    uart_send('\n');
    // uart_send_hex(y);
    // uart_send(' ');
    
    // uart_send_hex(x);
    // uart_send('\n');

    // uart_send_hex(hh);
    // uart_send('\n');
}

void cpio_ls(unsigned char* cpio_address) {
    unsigned char *ptr = (unsigned char *)cpio_address;
    const int BUF_SIZE = 128;
    const int num = 4;
    while(1) {
        unsigned char *p2 = ptr+94;
        unsigned char *p1 = ptr+54;
        char path_size[9];
        char data_size[9];
        char buffer[BUF_SIZE];
        
        path_size[8] = '\0';
        data_size[8] = '\0';
        for (int i=0; i<8; i++) {
            path_size[i] = *p2;
            p2++;
        }
        for (int i=0; i<8; i++) {
            data_size[i] = *p1;
            p1++;
        }
        
        unsigned int y = hex_to_dec(path_size);
        unsigned int x = hex_to_dec(data_size);
        
        
        // int kk = ((110+y)/num)*num + ((110+y)%num==0 ? 0 : num);
        // int hh = ((kk+x)/num)*num + ((kk+x)%num==0 ? 0 : num);
        int kk = (110 + y + 3) & ~3;
        int hh = (kk + x + 3) & ~3;

        unsigned char *pp = ptr+110;
        int id = 0;
        for (int i=0; i<y; i++) {
            char cc = *pp;
            buffer[id++] = cc;
            pp++;
        }
        buffer[id] = '\0';
        if (cmp_string(buffer, "TRAILER!!!")==0) {
            break;
        }
        uart_send_string(buffer);
        uart_send_string("\n");
        ptr = ptr+hh;
    }
}

static void cpio_find_file(const char *name, unsigned char* cpio_address) {
    unsigned char *ptr = (unsigned char *)cpio_address;
    const int BUF_SIZE = 128;
    const int num = 4;
    while(1) {
        unsigned char *p2 = ptr+94;
        unsigned char *p1 = ptr+54;
        char path_size[9];
        char data_size[9];
        char buffer[BUF_SIZE];
        
        path_size[8] = '\0';
        data_size[8] = '\0';
        for (int i=0; i<8; i++) {
            path_size[i] = *p2;
            p2++;
        }
        for (int i=0; i<8; i++) {
            data_size[i] = *p1;
            p1++;
        }
        
        unsigned int y = hex_to_dec(path_size);
        unsigned int x = hex_to_dec(data_size);
        
        
        // int kk = ((110+y)/num)*num + ((110+y)%num==0 ? 0 : num);
        // int hh = ((kk+x)/num)*num + ((kk+x)%num==0 ? 0 : num);
        int kk = (110 + y + 3) & ~3;
        int hh = (kk + x + 3) & ~3;

        unsigned char *pp = ptr+110;
        int id = 0;
        for (int i=0; i<y; i++) {
            char cc = *pp;
            buffer[id++] = cc;
            pp++;
        }
        buffer[id] = '\0';
        if (cmp_string(buffer, "TRAILER!!!")==0) {
            break;
        } else if(cmp_string(buffer, name)==0) {
            pp = ptr+kk;
            for (int i=0; i<x; i++) {
                char cc = *pp;
                if (cc=='\n') 
                    uart_send('\r');
                uart_send(cc);
                pp++;
            }
            break;
        }
        ptr = ptr+hh;
    }
    uart_send_string("\n");
}

void cpio_cat(unsigned char* cpio_address) {
    const int BUF_SIZE = 128;
    char buffer[BUF_SIZE];
    uart_send_string("Filename: ");
    int i = 0;
    while(i<BUF_SIZE-1) {
        char c = uart_recv();
        if (c=='\n'||c=='\r') {
            uart_send_string("\n");
            break;
        }
        uart_send(c);
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    cpio_find_file(buffer, cpio_address);
}