#include "shell.h"
#include "reboot.h"
#include "utils.h"

void get_string(char *buffer) {
    int i = 0;
    char c;
    while(1) {
        c = uart_recv();
        if(c=='\n') {
            uart_send('\r');
	        uart_send('\n');
            break;
        } else uart_send(c);
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}

void parse_string(char *buffer, unsigned char* cpio_addr) {
    if (cmp_string(buffer, "help")==0) {
        uart_send_string("hello: Hello world!\n");
        uart_send_string("help : help\n");
        uart_send_string("info : system information\n");
    } else if(cmp_string(buffer, "hello")==0) {
        uart_send_string("Hello world!\n");
    } else if(cmp_string(buffer, "info")==0) {
        get_board_revision();
        get_arm_memory();
    } else if(cmp_string(buffer, "reboot")==0) {
        reset(1);
    } else if(cmp_string(buffer, "ls")==0) {
        cpio_ls(cpio_addr);
    } else if(cmp_string(buffer, "cat")==0) {
        cpio_cat(cpio_addr);
    } else {
        uart_send_string(buffer);
        uart_send_string(" not found, use 'help' for help.\n");
    }
    buffer[0] = '\0';
}
extern unsigned long _dtb_ptr;

void shell(unsigned char* cpio_addr) {
    while(1) {
        // void *dtb_ptr = (void*)_dtb_ptr;
        // unsigned long addr = _dtb_ptr;
        // uart_send_string("dtb address: ");
        // uart_send_hex(addr);
        // uart_send_string("\n");


        char buffer[1024];
        uart_send_string("# ");
        get_string(buffer);
        parse_string(buffer, cpio_addr);
        buffer[0] = '\0';
    }
}