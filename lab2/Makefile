CFLAG = -Iinclude -fno-stack-protector
K8=kernel8
LINKER=link.ld
START=start

SRC_C:=$(shell find src/ -name '*.c')
SRC_S:=$(shell find src/ -name '*.S')

OBJS:= $(SRC_C:%.c=%.o) \
		$(SRC_S:%.S=%.o) \
		start.o

all:$(K8).img

start.o: start.S
	aarch64-linux-gnu-gcc -o $@ -c -O0 $< 

%.o: %.S
	aarch64-linux-gnu-gcc -o $@ -c $<

%.o: %.c
	aarch64-linux-gnu-gcc -o $@ $(CFLAG) -c $<

$(K8).img: $(OBJS)
	aarch64-linux-gnu-ld -T $(LINKER) -o $(K8).elf $^
	aarch64-linux-gnu-objcopy -O binary $(K8).elf $@

clean:
	$(RM) $(K8:=.elf) $(OBJS)

run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none \
									-serial null -serial stdio -initrd archive/initramfs.cpio \
									-dtb archive/bcm2710-rpi-3-b-plus.dtb

debug:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio -S -s

on:
	sudo screen /dev/ttyUSB1 115200

onqe:
	sudo screen /dev/pts/11

py:
	python3 loadimg/loadimg.py

gdb:
	gdb-multiarch kernel8.elf -ex "target remote localhost:1234"

umnt:
	sync && sudo umount ../../sdcard