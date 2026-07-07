
### inspect ELF
```shell
aarch64-linux-gnu-nm -n kernel8.elf
aarch64-linux-gnu-readelf -S kernel8.elf
```

### inspect bss
```shell
aarch64-linux-gnu-nm -n kernel8.elf | grep -E '__bss_start|__bss_end|__bss_size'
grep -A20 '\.bss' kernel8.map
```

### run QEMU
```shell
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio
```

## Makefile usuage
```shell
$@	target name
$<	first prerequisite
$^	all prerequisites
$?	newer prerequisites
```
ex:  
```makefile
kernel.elf: boot.o main.o uart.o
	ld $^ -o $@
```

call the Make function wildcard and find all .c files in the current directory.  
`SRCS = $(wildcard *.c)`  

`SRCS = $(*.c)`
does not mean “all .c files”.

---

Take the value of SRCS, and replace the suffix .c with .o for each word.  
`OBJS = $(SRCS:.c=.o)`
ex:  
```shell
SRCS = main.c uart.c mbox.c
OBJS = $(SRCS:.c=.o)
```

---

```shell
aarch64-linux-gnu-ld start.o $(OBJS) -T link.ld -o kernel8.elf  
-T link.ld      means Use link.ld as the linker script.
-c              means compile only, do not link
```


`aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img`  
```shell
-O means                : output format
-O binary               : raw binary output
-O elf64-littleaarch64  : 64-bit little-endian AArch64 ELF
-O ihex                 : Intel HEX format
-O srec                 : Motorola S-record format
```

`rm kernel8.img kernel8.elf *.o >/dev/null 2>/dev/null || true`  
`rm kernel8.img kernel8.elf *.o`  
```shell
rm -f kernel8.img kernel8.elf *.o      -f means force
```