#include "base.h"

#ifndef MOBX_H
#define MOBX_H

extern volatile unsigned int mailbox[8];

#define MAILBOX_READ    ((volatile unsigned int*)(MAILBOX_BASE))
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))
#define MBOX_CH_PROP    8

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_SERIAL          0x00010004
#define GET_ARM_MEM         0x00010005
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

int mailbox_call(unsigned char ch);
void get_board_revision();
void get_arm_memory();

#endif