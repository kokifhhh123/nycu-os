#include "mini_uart.h"
#include "mbox.h"
#include "cpio.h"
#include "dtb.h"

void shell(unsigned char* cpio_addr);
void get_string(char *buffer);
void parse_string(char *buffer, unsigned char* cpio_addr);