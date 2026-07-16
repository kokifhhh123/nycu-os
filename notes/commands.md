
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

## clangd command
```shell
bear -- make
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


# Armv8-A Assembly
```
ldr x3, [x1]

ldr x4, =0x60000
ldr x1, =__bss_start
```
---
```
.byte 0   // 1 byte
.hword 0  // 2 bytes
.word 0   // 4 bytes
.quad 0   // 8 bytes
```
```
ex:
.word 1, 2, 3, 4        // 1 2 3 4
.ascii "hello"          // h e l l o
.asciz "hello"          // h e l l o \0

.space 64
.space 16, 0xff
.fill 8, 4, 0
```

`.align 3`

# gdb uauage

```
break main
break _start
break *0x80000
info breakpoints
delete 1
disable 1
enable 1
hbreak *0x60000
```
```
differences: break usually sets a software breakpoint, hbreak sets a hardware breakpoint
hbreak *0x60000 
break *0x60000
```
```
c
s
n
si
ni
finish // Continue until the current function returns.


// Registers
info registers
info registers x0 x1 x2 sp pc
p/x $x0
p/x $sp
p/x $pc
set $x0 = 0x80000


// Memory inspection
x/4bx $x0       // Display four bytes starting at the address in x0.
x/8wx $x0       // Display eight 32-bit words in hexadecimal.
x/4gx $x0       // Display four 64-bit values in hexadecimal.
x/s $x0         // Interpret memory at x0 as a null-terminated string.
x/i $pc         // 0x60048:     bl      0x601e8
x/gx &_dtb_ptr  // 0x80840:        0x0000000000000000

// The general format is:
x/<count><format><size> address    // convention: count + size + format
// Common formats:
x    hexadecimal
d    decimal
u    unsigned decimal
c    character
s    string
i    instruction
// Common sizes:
b    byte, 1 byte
h    halfword, 2 bytes
w    word, 4 bytes
g    giant word, 8 bytes



// Symbols and variables
p/x &_dtb_ptr           // Print the address of _dtb_ptr.
x/gx &_dtb_ptr          // Read the 8-byte value stored at _dtb_ptr.
info address _dtb_ptr   // Show where the symbol is located.
info variables          // List known global and static variables.
info functions          // List known functions.


// Disassembly
disassemble _start                  // Disassemble the symbol _start.
disassemble /r _start               // Disassemble _start and show instruction bytes.
disassemble 0x80000, 0x80200        // Disassemble an explicit address range.
disassemble /r 0x80000, 0x80400
x/20i $pc                           // Show 50 instructions starting from the current program counter.
x/20i 0x60000                       // Show 20 instructions starting from 0x60000.
x/100i _start                       // Show 100 instructions starting from _start.
set disassemble-next-line on        // Automatically show assembly near the current instruction.
```

`*` means: Treat this expression as an exact instruction address.
```
break *0x60000
With *:
explicitly mean: Set a breakpoint at memory address 0x60000.

x/20i 0x60000
x/20i does not need *
x command already means examine memory at an address:
```


```
// Source code
list                           // Show source code near the current location.
list main                      // Show source code for main.
info line main                 // Show the address range corresponding to a source line or function.


// Call stack
backtrace
bt                      // Show the call stack.
frame 1                 // Switch to stack frame 1.
info frame              // Show details about the current stack frame.


// Automatic display
display/i $pc           // Automatically display the current instruction after every step.
display/x $x0           // Automatically display x0.
info display            // List automatic displays.
undisplay 1             // Remove display number 1.


// Useful commands
info registers x0 x1 sp pc
p/x &_dtb_ptr
x/gx &_dtb_ptr
x/4bx $x0
x/20i $pc

```

## difference between p/x and x/x in GDB
### p/x prints the value of an expression in hexadecimal
```
p/x $x0         // prints the value stored in register x0.
p/x variable
p/x &variable   
p/x &_dtb_ptr   // prints the address of _dtb_ptr.
p/x 10 + 20
```
### x/x examines memory at an address and displays the memory contents in hexadecimal
```
x/x $x0         // treats the value in x0 as a memory address and reads memory from that location.
ex:
x0 = 0x80000
memory at 0x80000 = 0x12345678
```
```
p/x $x0 -> 0x80000
x/x $x0 -> 0x80000: 0x12345678
```
```
p/x expression   → print the expression's value
x/x address      → read memory at the address
```
```
p/x &_dtb_ptr           // shows where _dtb_ptr is stored.
x/gx &_dtb_ptr          // reads the 8-byte value stored inside _dtb_ptr.
```
