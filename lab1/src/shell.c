#include "shell.h"
#include "reboot.h"

int cmp_string(const char *a, const char *b) {
    while(*a && (*a==*b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

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

void parse_string(char *buffer) {
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
    } else {
        uart_send_string(buffer);
        uart_send_string(" not found, use 'help' for help.\n");
    }
    buffer[0] = '\0';
}

void shell() {
    while(1) {
        char buffer[1024];
        uart_send_string("# ");
        get_string(buffer);
        parse_string(buffer);
        buffer[0] = '\0';
    }
}