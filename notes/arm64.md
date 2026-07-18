AArch64 assembly, also called:
- ARM64
- ARMv8-A assembly
- A64 instruction set

Do not mix it with 32-bit ARM assembly, which uses registers such as `r0`, `r1`, `lr`, and `pc`. In AArch64, use `x0`–`x30`.  

# Registers
```armasm
x0–x30
// Each x register is 64 bits.

// The lower 32 bits can be accessed using the corresponding w register:
x0  = 64-bit register
w0  = lower 32 bits of x0

mov x0, #0xffffffffffffffff
mov w0, #1

// After writing to w0, the entire x0 becomes:
x0 = 0x0000000000000001
```

## Common register roles
```armasm
x0–x7	    Function arguments and return values
x8	        Indirect result location or temporary
x9–x15	    Caller-saved temporary registers
x16–x17	    Intra-procedure-call temporary registers
x18	        Platform-specific register
x19–x28     Callee-saved registers
x29	        Frame pointer, fp
x30	        Link register, lr
sp	        Stack pointer
xzr	        64-bit zero register
wzr	        32-bit zero register
```
stack pointer should normally remain **16-byte aligned** whenever calling a function.  

Immediate values use `#`
```armasm
mov x0, #10
add x1, x0, #4
```
Memory operands use brackets `[]`:
```armasm
ldr x0, [x1]
```

# Moving values
## `mov`
```armasm
mov x0, x1
mov x0, #10
```
may fail or become an assembler-generated alias depending on the constant:    
`mov x0, #0x123456789abcdef0`  
For large constants, use `movz`, `movk`, and sometimes `movn`.
## `movz`
Move a 16-bit value and zero everything else:
```armasm
movz x0, #0x1234
// x0 = 0x0000000000001234

movz x0, #0x1234, lsl #16
// x0 = 0x0000000012340000
```

## `movk`
Move a 16-bit value into part of a register while keeping the other bits:  
```armasm
movz x0, #0xdef0
movk x0, #0x9abc, lsl #16
movk x0, #0x5678, lsl #32
movk x0, #0x1234, lsl #48
// x0 = 0x123456789abcdef0
```

## `movn`
```armasm
movn x0, #0
// x0 = 0xffffffffffffffff
```

# Arithmetic instructions
## `Addition`, `Subtraction`, `Negation`, `Multiplication`, `Division`, `Multiply-add`

```armasm
add x0, x1, x2
add x0, x1, #16


sub x0, x1, x2
sub sp, sp, #32


neg x0, x1          // sub x0, xzr, x1


mul x0, x1, x2      // x0 = x1 * x2;
// For the full 128-bit result of a 64-bit multiplication:
// `x0` gets the low 64 bits  
// `x3` gets the high 64 bits for unsigned multiplication
mul   x0, x1, x2
umulh x3, x1, x2
// Signed high half:
smulh x3, x1, x2


// Unsigned:    x0 = (uint64_t)x1 / (uint64_t)x2;
udiv x0, x1, x2
// Signed:      x0 = (int64_t)x1 / (int64_t)x2;
sdiv x0, x1, x2

// no basic remainder instruction. Compute it using multiply-subtract:
udiv x3, x1, x2
msub x0, x3, x2, x1
// msub means:
// x0 = x1 - x3 * x2;
// quotient  = x1 / x2;
// remainder = x1 - quotient * x2;


// x0 = x1 * x2 + x3;
madd x0, x1, x2, x3
// Multiply-subtract: x0 = x3 - x1 * x2;
msub x0, x1, x2, x3
```

# Flag-setting arithmetic
## `adds`, `subs`
Flag-setting variants have an `s` suffix:
```armasm
adds x0, x1, x2
subs x0, x1, x2
// x0 = x1 - x2;
// update_condition_flags(x0);
```
These update the `NZCV` flags:
```shell
N	Negative
Z	Zero
C	Carry or no borrow
V	Signed overflow
```

## `cmp`
```armasm
cmp x0, x1
// temporary = x0 - x1;
// update_flags(temporary);
cmp x0, #10
```

## `cmn`

Compare negative, effectively addition for flags:
```armasm
cmn x0, x1
temporary = x0 + x1;
update_flags(temporary);
```
use cmp much more often than cmn.


# Branch instructions
## Unconditional branch `b`
```armasm
b label
// goto label;

loop:
    b loop
// is an infinite loop.
```


## Function call: `bl`
```armasm
bl uart_init
// bl means branch with link.
// 1. stores the return address in x30
// 2. branches to the target

x30 = address_after_bl;
goto uart_init;
// x30 is also called lr, the link register.
```


## Return: `ret`
```armasm
ret
// branch to the address in x30

// Equivalent to:
ret x30

// A function call looks like:
bl function
// and the function ends with:
ret
```


## Indirect branch: `br`
```armasm
br x0
// Branch to the address stored in x0.
// goto *(address *)x0;
```
Used for:
- jump tables
- function pointers
- switching execution locations
- bootloader relocation



## Indirect function call: `blr`
```armasm
blr x0
// 1. stores the return address in x30
// 2. branches to the address in x0

// Equivalent to calling a function pointer:
function_pointer();
```

## Conditional branches

After `cmp`, `subs`, or another flag-setting instruction:
```armasm
cmp x0, x1
b.eq equal
b.ne not_equal
```

```armasm
//  Condition   Meaning
    eq	        Equal
    ne	        Not equal
    lt	        Signed less than
    le	        Signed less than or equal
    gt	        Signed greater than
    ge	        Signed greater than or equal
    lo,cc	    Unsigned lower
    ls	        Unsigned lower or same
    hi	        Unsigned higher
    hs,cs	    Unsigned higher or same
    mi	        Negative
    pl	        Positive or zero
    vs	        Signed overflow
    vc	        No signed overflow
```
### Signed versus unsigned comparison

This is crucial.  
```armasm
// Signed:
cmp x0, x1
b.lt smaller
// if ((int64_t)x0 < (int64_t)x1)

// Unsigned:
cmp x0, x1
b.lo smaller
// if ((uint64_t)x0 < (uint64_t)x1)
```
The `cmp` instruction is the same. The branch condition decides how the flags are interpreted.

### Compare and branch
```armasm
cbz x0, is_zero
// if (x0 == 0)
//     goto is_zero;
cbnz x0, not_zero
// if (x0 != 0)
//     goto not_zero;

// These do not modify condition flags.
```


### Test bit and branch
```armasm
tbz x0, #3, bit_is_zero
// if ((x0 & (1ULL << 3)) == 0)
//     goto bit_is_zero;
tbnz x0, #3, bit_is_one
// if ((x0 & (1ULL << 3)) != 0)
//     goto bit_is_one;

// Very useful when polling hardware status bits.
```




# Conditional selection

ARM can perform some conditions without branching.
```armasm
// csel
cmp x0, x1
csel x2, x3, x4, eq
// x2 = (x0 == x1) ? x3 : x4;

// cset
cmp x0, x1
cset x2, eq
// x2 = (x0 == x1) ? 1 : 0;

// cinc
cinc x0, x1, eq
// x0 = condition_is_true ? x1 + 1 : x1;

// Remember:  
`csel`
`cset`
```

# Logical and bitwise instructions
## `AND`, `OR`, `Exclusive OR`, `bic-Bit clear`, `Bitwise NOT`, `Flag-setting AND`, `tst`
```armasm
// AND
and x0, x1, x2
x0 = x1 & x2;
// Immediate:
and x0, x1, #0xff
// x0 = x1 & 0xff;


// OR
orr x0, x1, x2
// x0 = x1 | x2;
// mov between registers is commonly an alias of orr using xzr.


// Exclusive OR
eor x0, x1, x2
x0 = x1 ^ x2;
// ARM calls XOR EOR: exclusive OR.


// Bit clear
bic x0, x1, x2
// x0 = x1 & ~x2;
// This is useful for clearing selected bits.
// For example:
mov x1, #0b1100
bic x0, x0, x1
// clears bits 2 and 3 in x0.


// Bitwise NOT
mvn x0, x1
// x0 = ~x1;


// Flag-setting AND
ands x0, x1, x2
// Computes AND and updates flags.


// tst
tst x0, #0x4
b.ne bit_set
// if ((x0 & 0x4) != 0)
//    goto bit_set;
// tst is essentially ands with the result discarded.
// For hardware registers, this pattern is extremely common:
poll:
    ldr w0, [x1]
    tst w0, #STATUS_READY
    b.eq poll
```



# Shifts
## `lsl`, `lsr`, `asr`, `ror`, `Shifted operands`
```armasm
// Logical left shift
lsl x0, x1, #3
// x0 = x1 << 3;


// Logical right shift
lsr x0, x1, #3
// Zeros are inserted from the left:
// x0 = (uint64_t)x1 >> 3;


// Arithmetic right shift
asr x0, x1, #3
// The sign bit is copied:
// x0 = (int64_t)x1 >> 3;


// Rotate right
ror x0, x1, #8
// Bits shifted out from the right reappear on the left.
// x0 = rotate_right(x1, 8);


// Shifted operands
// Many AArch64 arithmetic instructions can shift an operand as part of the instruction:
add x0, x1, x2, lsl #3
// x0 = x1 + (x2 << 3);
// This is very common for array indexing:
// If x1 is a base pointer and each element is 8 bytes:
// x0 = &array[x2];
```
`RISC-V` would often require a separate shift instruction.   
`ARM` frequently combines it into the arithmetic or addressing operation.


# Load and store
This is one of the most important sections for kernel programming.
```armasm
// 64-bit load/store
ldr x0, [x1]
// x0 = *(uint64_t *)x1;
str x0, [x1]
// *(uint64_t *)x1 = x0;


// 32-bit load/store
ldr w0, [x1]
str w0, [x1]
// Equivalent to accessing uint32_t.
// MMIO registers are often 32 bits, so will commonly use w0, rather than x0.
ldr w0, [x1]
str w0, [x1]


// Byte load/store
ldrb w0, [x1]
strb w0, [x1]
// w0 = *(uint8_t *)x1;
// *(uint8_t *)x1 = w0;
// A byte load writes into a w register and zero-extends the value.


// Halfword load/store
ldrh w0, [x1]
strh w0, [x1]
// Equivalent to 16-bit access:
// w0 = *(uint16_t *)x1;
// *(uint16_t *)x1 = w0;


// Signed loads
// Normal small loads zero-extend:
ldrb w0, [x1]
// To sign-extend:
ldrsb x0, [x1]
// Equivalent to:
// x0 = (int64_t)*(int8_t *)x1;


// Other signed forms:
ldrsh x0, [x1]   // signed 16-bit to 64-bit
ldrsw x0, [x1]   // signed 32-bit to 64-bit


// This distinction matters:
ldr w0, [x1]
// loads 32 bits and zeroes the upper half of x0.
ldrsw x0, [x1]
// loads a signed 32-bit value and sign-extends it into 64 bits.
```


# Addressing modes
```armasm
// Base register only
ldr x0, [x1]


// Base plus immediate offset
ldr x0, [x1, #16]
// x0 = *(uint64_t *)(x1 + 16);


// Base plus register offset
ldr x0, [x1, x2]
// x0 = *(uint64_t *)(x1 + x2);


// Scaled register offset
ldr x0, [x1, x2, lsl #3]
// x0 = ((uint64_t *)x1)[x2];
// Because each 64-bit element is eight bytes:
// address = x1 + x2 × 8


// Pre-index addressing
ldr x0, [x1, #8]!
// x1 = x1 + 8;
// x0 = *(uint64_t *)x1;
// The ! means the base register is updated before access.


// Post-index addressing
ldr x0, [x1], #8
// x0 = *(uint64_t *)x1;
// x1 = x1 + 8;
// common in loops that walk through memory:
loop:
    ldr x0, [x1], #8
```

# Loading and storing register pairs
```armasm
// stp
stp x29, x30, [sp, #-16]!
// This:
// decreases sp by 16
// stores x29 at [sp]
// stores x30 at [sp + 8]
// sp -= 16;
// *(uint64_t *)(sp + 0) = x29;
// *(uint64_t *)(sp + 8) = x30;


// ldp
ldp x29, x30, [sp], #16
// This:
// loads x29 from [sp]
// loads x30 from [sp + 8]
// increases sp by 16
// x29 = *(uint64_t *)(sp + 0);
// x30 = *(uint64_t *)(sp + 8);
// sp += 16;


// A common function structure is:
function:
    stp x29, x30, [sp, #-16]!
    mov x29, sp

    // function body
    ldp x29, x30, [sp], #16
    ret
```

# Why save x30?
```armasm
foo:
    bl bar
    ret
// When someone calls foo, x30 contains the return address for foo.
// But then:
bl bar
// overwrites x30 with the address after bl bar.


// Therefore, foo loses its original return address.
// A non-leaf function generally needs to save x30:
foo:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    bl bar
    ldp x29, x30, [sp], #16
    ret


// A leaf function calls no other functions. It may return directly without saving x30:
add_two:
    add x0, x0, x1
    ret
```


# Calling convention
```armasm
// For a C function:
uint64_t add_three(uint64_t a, uint64_t b, uint64_t c);
// Arguments:
// a → x0
// b → x1
// c → x2
// Return value:
// x0
// Assembly implementation:
add_three:
    add x0, x0, x1
    add x0, x0, x2
    ret


// Caller-saved registers
// A called function may freely destroy
// Registers:
x0–x18
// There are platform-specific details for x18, so portable code should normally avoid using it casually.
// If the caller needs a value in x9 after a function call, the caller must save it.
// Example:
mov x9, #100
bl function
// cannot assume x9 is still 100 afterward.


// Callee-saved registers
// Registers:
x19–x28
// must be restored by a function if it changes them.
// Example:
function:
    stp x19, x20, [sp, #-16]!
    mov x19, x0
    mov x20, x1
    // use x19 and x20
    ldp x19, x20, [sp], #16
    ret
// The caller may expect x19 and x20 to retain their original values.


// Stack alignment
// Before calling another function, sp should be aligned to 16 bytes.
// Good:
sub sp, sp, #16
// Bad:
sub sp, sp, #8
bl function
// Even if the hardware does not immediately fail, this violates the calling convention.
```

# Address construction: `adr`, `adrp`, and `ldr =symbol`
```armasm
// This confuses many beginners.
// adr
adr x0, label
// Calculates the address of a nearby label relative to the current instruction.
// x0 = &label;
// It does not load the contents stored at label.
// Example:
adr x0, message
// Now x0 points to message.



// adrp
adrp x0, symbol
// Loads the page address containing symbol.
// A page here is normally treated as a 4 KiB-aligned address unit for instruction relocation calculations.
// Usually used with:
adrp x0, symbol
add  x0, x0, :lo12:symbol
// x0 = &symbol;
// But split into:
// page part + lower 12-bit offset
// This form can reach farther than adr.



// ldr x0, =symbol
// This is an assembler pseudo-instruction:
ldr x0, =symbol
// It asks the assembler/linker to put the address or constant into x0.
// Depending on the situation, the assembler may implement it using:
//      a literal pool
//      movz/movk
//      another suitable sequence
// x0 = (uintptr_t)&symbol;
// But it is not a single fixed AArch64 instruction.
// ex:
.data
value:
    .quad 123
.text
ldr x0, =value
// x0 contains the address of value, not the number 123



// Literal loads
ldr x0, symbol
// Usually a PC-relative literal load from the memory location represented by symbol:
// x0 = *(uint64_t *)&symbol;
// ex:
ldr x0, constant
constant:
    .quad 0x123456789abcdef0



// Address versus value
// Given:
value:
    .quad 123

// This:
adr x0, value
// means:
// x0 = &value;

// This:
ldr x0, [x0]
// then means:
// x0 = value;

// Together:
adr x0, value
ldr x0, [x0]
// load the value 123.
```


# Sign extension and zero extension

```armasm
// Zero extension
uxtb w0, w1
// w0 = (uint8_t)w1;
uxth w0, w1
// w0 = (uint16_t)w1;
// Because writing a w register automatically zeroes the upper 32 bits, simple 32-to-64 unsigned extension often needs no instruction:
mov w0, w1
// makes x0 a zero-extended 32-bit value.

// Sign extension
sxtb x0, w1
// x0 = (int64_t)(int8_t)w1;
sxth x0, w1
// x0 = (int64_t)(int16_t)w1;
sxtw x0, w1
// x0 = (int64_t)(int32_t)w1;
```


# Bit-field instructions
Powerful bit extraction and insertion instructions.
```armasm
// Unsigned bit-field extract
ubfx x0, x1, #8, #4
// Extract four bits beginning at bit 8:
// x0 = (x1 >> 8) & 0xf;
// Useful for hardware register fields.


// Signed bit-field extract
sbfx x0, x1, #8, #4
// Extracts the field and sign-extends it.

// Bit-field insert
bfi x0, x1, #8, #4
// Copies the low four bits of x1 into bits 8–11 of x0.
// x0 = (x0 & ~(0xfULL << 8)) | ((x1 & 0xf) << 8);
```


# Endianness and byte reversal
Most modern AArch64 systems run little-endian.
```armasm
// For:
x0 = 0x1122334455667788
// stored at increasing addresses:
// address + 0: 88
// address + 1: 77
// address + 2: 66
// ...
// address + 7: 11


// Byte-reversal instructions:
// Reverse all bytes in a 64-bit register.
rev x0, x1
// Reverse bytes separately within each 32-bit word.
rev32 x0, x1


// useful for:
// network protocols
// big-endian file formats
// hardware interfaces
```


# System registers

Kernel and bare-metal code use special system registers.  

accessed with:
```armasm
mrs destination, system_register
msr system_register, source


// mrs, Read system register:
mrs x0, CurrentEL
// x0 = CurrentEL;


// msr, Write system register:
msr VBAR_EL1, x0
// VBAR_EL1 = x0;


// Common system registers:
// Register	    Purpose
CurrentEL	    Current exception level
VBAR_EL1	    Exception vector table address
SPSR_EL1	    Saved processor state
ELR_EL1	        Exception return address
ESR_EL1	        Exception syndrome
FAR_EL1	        Fault address
SCTLR_EL1	    System control
TTBR0_EL1	    Translation-table base
TCR_EL1	        Translation configuration
MAIR_EL1	    Memory attribute configuration
SP_EL0	        Stack pointer for EL0
DAIF	        Interrupt mask state

// Do not try to memorize every system register. Learn them when implementing:
//      exceptions
//      interrupts
//      MMU
//      timers
//      privilege transitions
```


# Exception
```armasm
// AArch64 defines exception levels:
EL0: user applications
EL1: operating system kernel
EL2: hypervisor
EL3: secure monitor
// Higher numbers generally have more privilege.

// A Raspberry Pi bare-metal program may begin at different exception levels depending on:
//      firmware
//      board model
//      boot configuration
//      execution environment

// Read CurrentEL:
mrs x0, CurrentEL
// CurrentEL stores the level in bits [3:2].

// So:
mrs x0, CurrentEL
lsr x0, x0, #2
and x0, x0, #3
// x0 = (CurrentEL >> 2) & 3;



// Exception return
eret
// Returns from an exception.

// generally uses:
ELR_ELx as the return address
SPSR_ELx as the restored processor state

// For example, returning from an exception handled at EL1 uses:
ELR_EL1
SPSR_EL1

// eret is not the same as ret.
// ret:    normal function return using a general-purpose register, normally x30
// eret:   architectural exception return using exception state registers
```


# Interrupt masking, Barriers, Cache and TLB maintenance
AArch64 has interrupt-mask bits represented by `DAIF`:
```armasm
// Letter	Meaning
D	    Debug exceptions
A	    SError aborts
I	    IRQ interrupts
F	    FIQ interrupts

// Mask interrupts:
msr daifset, #0xf

// Unmask selected classes:
msr daifclr, #0xf

// For only IRQ masking, the immediate bit corresponding to I is used.
// These are privileged operations.
```


`Barriers`  
Barriers are critical in kernels, drivers, multicore code, and MMIO.  
```armasm
// dmb
// Data memory barrier:
dmb sy
// Ensures ordering of memory accesses around the barrier, within the specified domain.
// It does not necessarily mean that all operations have completely finished. It primarily enforces ordering.


// dsb
// Data synchronization barrier:
dsb sy
// Stronger than dmb. It waits for relevant previous operations to complete before later instructions proceed.


// isb
// Instruction synchronization barrier:
isb
// Flushes the effects of certain instruction-pipeline state and ensures subsequent instructions observe updated system state.
// Common after changing important system control registers:
msr SCTLR_EL1, x0
isb


// A rough mental model:
//      dmb = order memory operations
//      dsb = complete memory/system operations
//      isb = refresh instruction execution context
// The exact semantics depend on the barrier domain and operation type.
```


`Cache and TLB maintenance`
```armasm
// Kernel code may use instructions such as:
ic ivau, x0
dc cvau, x0
tlbi vmalle1

// Examples:
//      ic: instruction-cache maintenance
//      dc: data-cache maintenance
//      tlbi: TLB invalidation
// These instructions usually require barriers around them.


// A common conceptual sequence after modifying page tables is:
dsb ishst
tlbi vmalle1
dsb ish
isb

Do not memorize cache-maintenance variants at the beginning. First understand:

cache visibility
stale instructions
stale address translations
barriers
```


# Waiting and low-power instructions, Atomic operations
```armasm
// wfe
// Wait for event:
wfe
// The processor may stop actively executing until an event occurs.
// Common idle loop:
idle:
    wfe
    b idle


// sev
// Send event:
sev
// Signals an event that can wake processors waiting with wfe.



// wfi
// Wait for interrupt:
wfi
// Common when waiting for an interrupt.
// The exact behavior of wfe and wfi depends on interrupt masks, implementation, and system state.


// nop
nop
// Do nothing architecturally.
// Used for:
//      alignment
//      patching space
//      temporary debugging
//      timing experiments, though not reliable as a precise delay
```

`Atomic operations`
```armasm
// For multicore kernels, normal load-modify-store is unsafe:
ldr x0, [x1]
add x0, x0, #1
str x0, [x1]
// Another CPU may update the memory between the load and store.

// Exclusive load/store
retry:
    ldxr x0, [x1]
    add  x0, x0, #1
    stxr w2, x0, [x1]
    cbnz w2, retry
// do {
//     old = exclusive_load(address);
//     new = old + 1;
//     failed = exclusive_store(address, new);
// } while (failed);
// stxr writes a status result:
//      w2 = 0: store succeeded
//      w2 != 0: store failed


// Acquire/release versions:
ldaxr x0, [x1]
stlxr w2, x0, [x1]

// Modern ARM processors may also support Large System Extension atomic instructions such as:
ldadd
cas
swp
// But exclusive loops are still essential to understand.
```



# Acquire and release memory ordering
In concurrent code, atomicity alone is not enough. Memory ordering matters.  
```armasm
// Acquire load
ldar x0, [x1]
// Loads a value with acquire ordering.
// Memory accesses after this load cannot be observed as occurring before it in the relevant ordering model.


// Release store
stlr x0, [x1]
// Stores with release ordering.
// Earlier memory accesses are ordered before this store.


// A lock may conceptually use:
ldaxr
stxr
// or:
ldaxr
stlxr
// depending on the intended ordering.


// This becomes important when implementing:
//      spinlocks
//      reference counters
//      producer/consumer queues
//      scheduler structures
```

# Assembler directives, Labels
```armasm
// Lines beginning with . are usually assembler directives, not CPU instructions.

// Sections
.section ".text.boot"
// Selects a section named .text.boot.

// Common sections:
// Section	    Purpose
.text	        Executable instructions
.rodata	        Read-only constants
.data	        Initialized writable data
.bss	        Zero-initialized data
.text.boot	    Custom early boot code section


// Symbol visibility
.global _start
// Makes _start visible to the linker.
// Without .global, the symbol is normally local to the assembly file.


// Alignment
.align 4
// Be careful: the exact interpretation of .align can depend on the assembler and target.
// In GNU AArch64 assembly, .align 4 commonly aligns to a 2^4 = 16 byte boundary.
// To be explicit, may also see:
.balign 16
// which requests 16-byte alignment.


// Data definitions
.byte  0x12
.hword 0x1234
.word  0x12345678
.quad  0x123456789abcdef0
// Reserve zero-filled bytes:
.space 1024
// String:
.asciz "hello"
// .asciz adds a terminating zero byte.


// Constant symbols
.equ UART_BASE, 0x3f215040
// Then:
ldr x0, =UART_BASE


// Symbol type
.type function_name, %function
// Marks a symbol as a function for object-file metadata.


// Symbol size
.size function_name, . - function_name
// The expression . means the current location.
// Therefore:
current address - function start address
// gives the function size.
```


`Labels`
```armasm
// A label names an address:
loop:
    b loop
// loop represents the address of the b loop instruction.


// Local numeric labels are useful:
1:
    subs x0, x0, #1
    b.ne 1b
// 1b means: the nearest label 1 backward


// Forward reference:
b 1f
...
1:
// 1f means: the nearest label 1 forward
```


## Comments

GNU-style AArch64 assembly commonly uses:  
`// comment`  
or:  
`/* comment */`  


# example
```c
uint64_t sum_array(uint64_t *array, uint64_t count)
{
    uint64_t sum = 0;
    while (count != 0) {
        sum += *array;
        array++;
        count--;
    }
    return sum;
}
```
```armasm
sum_array:
    mov x2, #0

loop:
    cbz x1, done
    ldr x3, [x0], #8
    add x2, x2, x3
    sub x1, x1, #1
    b loop

done:
    mov x0, x2
    ret
```



# Multicore identification, MMIO
```armasm
// On AArch64, code often reads:
mrs x0, mpidr_el1
// MPIDR_EL1 identifies the processor affinity.

// For simple Raspberry Pi boot code, may extract a low affinity field:
and x0, x0, #3
cbz x0, primary_core

// However, do not assume that masking with 3 is universally correct for every ARM system. It depends on the platform’s CPU topology and how core identifiers are encoded.
```


`MMIO patterns`  
Hardware registers are accessed as memory.

```c
volatile uint32_t *status = (volatile uint32_t *)STATUS_ADDR;
while ((*status & READY_BIT) == 0) {
}
```
```armasm
ldr x0, =STATUS_ADDR
poll:
    ldr w1, [x0]
    tst w1, #READY_BIT
    b.eq poll
```


Writing a register:
```armasm
ldr x0, =UART_IO
mov w1, #'A'
str w1, [x0]
```

Important considerations:
- use the correct access width
- preserve reserved bits
- use barriers where required
- read the hardware manual
- do not assume normal RAM behavior
- do not let the compiler optimize polling away in C  

Assembly itself has no `volatile` keyword. Explicit assembly memory instructions are emitted as written, though CPU-level reordering and device-memory configuration still matter.  




`Instruction aliases`  
AArch64 has many aliases. Different-looking instructions may encode the same underlying operation.
```armasm
// Examples:
mov x0, x1
// may be encoded as a form of:
orr x0, xzr, x1


cmp x0, x1
// is an alias of:
subs xzr, x0, x1


tst x0, x1
// is an alias of:
ands xzr, x0, x1


neg x0, x1
// is an alias related to:
sub x0, xzr, x1

// This matters when objdump displays an instruction differently from how you wrote it. Both representations may be correct.
```


# Instructions should memorize first
```armasm
// Essential data movement
mov
movz
movk
adr
adrp
ldr
str
ldrb
strb
ldp
stp
// Essential arithmetic
add
sub
mul
udiv
sdiv
neg
// Essential comparisons and branches
cmp
tst
b
bl
br
blr
ret
b.eq
b.ne
b.lt
b.le
b.gt
b.ge
b.lo
b.hs
cbz
cbnz
// Essential bit operations
and
orr
eor
bic
lsl
lsr
asr
// Essential kernel instructions
mrs
msr
eret
dmb
dsb
isb
wfe
wfi
sev
// Essential atomic instructions
ldxr
stxr
ldaxr
stlxr
```

Instructions can learn later  
- every SIMD/NEON instruction
- floating-point instructions
- cryptographic instructions
- pointer-authentication instructions
- every cache-maintenance variant
- every atomic LSE instruction
- every bit-field alias
- every condition-select variant
- every exception-system register
- Scalable Vector Extension instructions


# Common mistakes
```armasm
// Mistake 1: confusing address and contents
adr x0, variable
// means: x0 = &variable;
// It does not mean: x0 = variable;
// To load the value:
adr x0, variable
ldr x0, [x0]



// Mistake 2: forgetting w writes clear upper bits
mov x0, #-1
mov w0, #1
// Final value: x0 = 1
// not:
0xffffffff00000001



// Mistake 3: using the wrong signed condition
cmp x0, x1
b.lt label
// is signed comparison.
// For addresses and sizes, unsigned conditions are usually more suitable:
b.lo
b.ls
b.hi
b.hs


// Mistake 4: destroying x30
// A function that calls another function must usually save its return address.


// Mistake 5: breaking stack alignment
// Keep sp 16-byte aligned across function calls.


// Mistake 6: assuming ldr =constant is one real instruction
// It is an assembler pseudo-instruction and may generate different code.


// Mistake 7: using the wrong memory-access width
// A 32-bit MMIO register should usually be accessed using w registers:
ldr w0, [x1]
str w0, [x1]
// Using a 64-bit access may read or write adjacent hardware registers.



// Mistake 8: forgetting that add does not set flags
// This does not update NZCV:
add x0, x0, #1
// This does:
adds x0, x0, #1


// Mistake 9: treating CPU ordering as source-code ordering
// For MMIO, page tables, caches, and multicore synchronization, barriers and memory attributes matter.
```
