
```shell
AUX     = Auxiliary peripheral block
MU      = Mini UART
IO      = Input/Output data register
IER     = Interrupt Enable Register
IIR     = Interrupt Identify Register
LCR     = Line Control Register
MCR     = Modem Control Register
LSR     = Line Status Register
MSR     = Modem Status Register
CNTL    = Control Register
STAT    = Status Register
BAUD    = Baud rate register
```
```shell
AUX_ENABLE      // enable mini UART
AUX_MU_IO       // send/receive byte
AUX_MU_LSR      // check TX/RX readiness
AUX_MU_CNTL     // enable transmitter/receiver
AUX_MU_BAUD     // set baud rate
AUX_MU_LCR      // set data size
AUX_MU_IIR      // FIFO control
AUX_MU_IER      // disable/enable interrupts
```
```shell
AUX_ENABLE      0x215004    Enables AUX peripherals. For mini UART, you set bit 0.
AUX_MU_IO       0x215040    Mini UART I/O data register. Write here to transmit a byte; read here to receive a byte
AUX_MU_IER      0x215044    Interrupt Enable Register. Controls mini UART RX/TX interrupts.
AUX_MU_IIR      0x215048    Interrupt Identify Register / FIFO control. Often used to clear/enable FIFOs.
AUX_MU_LCR      0x21504C    Line Control Register. Sets data size, commonly 8-bit mode.
AUX_MU_MCR      0x215050    Modem Control Register. Controls modem-related signals such as RTS. Often not important.
AUX_MU_LSR      0x215054    Line Status Register. Tells you whether RX data is ready or TX can accept data
AUX_MU_MSR      0x215058    Modem Status Register. Shows modem signal status. Usually not needed for basic serial.
AUX_MU_SCRATCH  0x21505C    Scratch register. General-purpose temporary register; usually not needed.
AUX_MU_CNTL     0x215060    Extra Control Register. Enables/disables mini UART transmitter and receiver.
AUX_MU_STAT     0x215064    Extra Status Register. Gives more detailed UART status, such as FIFO state.
AUX_MU_BAUD     0x215068    Baud rate register. Sets UART speed, for example 115200 baud.
```