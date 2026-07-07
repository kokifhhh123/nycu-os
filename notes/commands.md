
# Inspect bss
```shell
aarch64-linux-gnu-nm -n kernel8.elf | grep -E '__bss_start|__bss_end|__bss_size'
```

`-Map=kernel8.map` generates a linker map file.  
The map file shows where sections and symbols are placed after linking.
```shell
aarch64-linux-gnu-ld start.o $(OBJS) -T link.ld -Map=kernel8.map -o kernel8.elf
grep -A20 '\.bss' kernel8.map
```

# Run QEMU
```shell
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio
```
`Ctrl-A then X, quit qemu`  

`-d int` can help inspect exceptions/interrupts  
```shell
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio -d int
```

```shell
-S      pause CPU at startup
-s      open GDB server at tcp::1234
```


# Makefile usuage
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
-O means output format
-O binary               : raw binary output
-O elf64-littleaarch64  : 64-bit little-endian AArch64 ELF
-O ihex                 : Intel HEX format
-O srec                 : Motorola S-record format
```
Checking whether my final image is unexpectedly huge.
```shell
ls -lh kernel8.img
stat kernel8.img
```

`rm kernel8.img kernel8.elf *.o >/dev/null 2>/dev/null || true`  
`rm kernel8.img kernel8.elf *.o`  
```shell
rm -f kernel8.img kernel8.elf *.o      -f means force
```

# Inspect link layout

```shell
aarch64-linux-gnu-objdump
aarch64-linux-gnu-readelf
aarch64-linux-gnu-nm
```

## Use `objdump -h` to see section layout
```
after
aarch64-linux-gnu-as start.S -o start.o
aarch64-linux-gnu-ld start.o -T link.ld -o kernel8.elf
```
use
```
aarch64-linux-gnu-objdump -h kernel8.elf
```
get
```
Name	        section name, for example .text
Size	        section size
VMA	            virtual memory address, where the section expects to run in memory.
LMA	            load memory address, where the section is loaded from.
File off	    offset inside the ELF file
```
## Use `objdump -d` to see actual linked instructions
disassembles executable sections, usually enough for .text.  
`aarch64-linux-gnu-objdump -d kernel8.elf`  
disassembles all sections.  
`aarch64-linux-gnu-objdump -D kernel8.elf`  



## Use `readelf -S` to see section headers
```shell
aarch64-linux-gnu-readelf -S kernel8.elf
```
## Use `readelf -l` to see loadable memory segments
`LOAD` segment containing `.text`
```shell
aarch64-linux-gnu-readelf -l kernel8.elf
```
## Use `readelf -h` to show the ELF header, including:
```
Entry point address
Machine architecture
ELF class
Endianness
```
```shell
aarch64-linux-gnu-readelf -h kernel8.elf
```


## Use `nm` to see symbol addresses
```shell
aarch64-linux-gnu-nm kernel8.elf
```
the symbol _start is located at address 0x80000  

In summary
```shell
aarch64-linux-gnu-as start.S -o start.o
aarch64-linux-gnu-ld start.o -T link.ld -o kernel8.elf

aarch64-linux-gnu-objdump -h kernel8.elf
aarch64-linux-gnu-objdump -d kernel8.elf
aarch64-linux-gnu-readelf -S kernel8.elf
aarch64-linux-gnu-readelf -l kernel8.elf
aarch64-linux-gnu-nm kernel8.elf
```

### inspect ELF
```shell
aarch64-linux-gnu-nm -n kernel8.elf
aarch64-linux-gnu-readelf -S kernel8.elf
```

### `file` command
```shell
file kernel8.elf
file kernel8.img
```