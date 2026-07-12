SRCS:= start.S boot.c ../src/mini_uart.c
# OBJS = $(SRCS: .c=.o)
OBJS = start.o boot.o ../src/mini_uart.o
INCLUDES = -I. -I../include

all: boot.img

start.o: start.S
	aarch64-linux-gnu-gcc -o $@ -c $<

%.o: %.c
	aarch64-linux-gnu-gcc $(INCLUDES) -o $@ $(CFLAGS) -c $<

../src/mini_uart.o: ../src/mini_uart.c
	aarch64-linux-gnu-gcc $(INCLUDES) -o $@ $(CFLAGS) -c $<

boot.img: $(OBJS)
	aarch64-linux-gnu-ld $(INCLUDES) $(OBJS) -T link.ld -o boot.elf
	aarch64-linux-gnu-objcopy -O binary boot.elf boot.img

clean:
	$(RM) $(OBJS) boot.elf boot.img

boot:
	qemu-system-aarch64 -M raspi3b -kernel boot.img -display none -serial null -serial pty

run:
	qemu-system-aarch64 -M raspi3b -kernel boot.img -display none -serial null -serial stdio
