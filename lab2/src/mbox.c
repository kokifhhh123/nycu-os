#include "mbox.h"
#include "mini_uart.h"

volatile unsigned int __attribute__((aligned(16))) mailbox[8];

int mailbox_call(unsigned char ch) {
    // unsigned int r = mbox&~0xF
    unsigned int r = (((unsigned int)((unsigned long)&mailbox)&~0xF) | (ch&0xF));    
    while (*MAILBOX_STATUS & MAILBOX_FULL) asm volatile("nop");

    *MAILBOX_WRITE = r;
    while(1) {
        while (*MAILBOX_STATUS & MAILBOX_EMPTY) asm volatile("nop");
        if(*MAILBOX_READ == r) 
            return mailbox[1]==REQUEST_SUCCEED;
    }
    return 0;
}

void get_board_revision(){
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;

    uart_send_string("In get_board_revision\n");
    mailbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
    //   printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    uart_send_hex(mailbox[5]);
    uart_send_string("\n");
}

void get_arm_memory() {
    mailbox[0] = 8 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_ARM_MEM; // tag identifier
    mailbox[3] = 8; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    mailbox[6] = 0; // value buffer
    // tags end
    mailbox[7] = END_TAG;

    mailbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
    //   printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    uart_send_string("ARM mem base address\n");
    uart_send_hex(mailbox[5]);
    uart_send_string("\n");

    uart_send_string("ARM mem size\n");
    uart_send_hex(mailbox[6]);
    uart_send_string("\n");
}