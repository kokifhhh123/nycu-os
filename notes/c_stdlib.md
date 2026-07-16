`<stdint.h>` is part of the C standard library. The difference is not “**standard library**” versus “**not standard library.**”  
Distinctions:
1. Headers that mostly provide types, constants, and macros
2. Headers that declare functions requiring a libc implementation, runtime, operating system, heap, files, or system calls

# Standard headers usually easy to use in bare-metal code
Usually do not require an operating system.  

## `<stdint.h>`
```c
#include <stdint.h>

uint8_t byte;
uint16_t halfword;
uint32_t register_value;
uint64_t large_value;

uintptr_t address;
intptr_t signed_address;
```

Important types:
```c
uint8_t
uint16_t
uint32_t
uint64_t

int8_t
int16_t
int32_t
int64_t

uintptr_t
intptr_t

uint_least32_t
uint_fast32_t
```
Very useful for:
- hardware registers
- addresses
- binary formats
- page tables
- protocols
- firmware structures



## `<stddef.h>`
Fundamental types and macros:
```c
#include <stddef.h>
size_t size;
ptrdiff_t difference;
void *null_pointer = NULL;
```
Important definitions:
```c
size_t
ptrdiff_t
NULL
offsetof
max_align_t
```
```c
// Example:
struct task {
    int pid;
    void *stack;
};
size_t stack_offset = offsetof(struct task, stack);
```
This is one of the most important headers for kernel programming.  



## `<stdbool.h>`

Boolean type and constants:
```c
#include <stdbool.h>
bool ready = true;
if (!ready) {
    ...
}
```
Provides:
```c
bool
true
false
```
In C23, bool, true, and false are built into the language, but `<stdbool.h>` is still common in older C standards.


## `<limits.h>`

Limits of fundamental integer types:
```c
#include <limits.h>
unsigned int maximum = UINT_MAX;
int minimum = INT_MIN;
```
Important macros:
```c
CHAR_BIT
CHAR_MIN
CHAR_MAX
INT_MIN
INT_MAX
UINT_MAX
LONG_MAX
ULONG_MAX
```
Useful when checking overflow or making architecture assumptions.

### `<stdint.h>` limit macros
`<stdint.h>` also provides limits for fixed-width types:  
```c
UINT8_MAX
UINT16_MAX
UINT32_MAX
UINT64_MAX

INT32_MIN
INT32_MAX

UINTPTR_MAX
INTPTR_MAX

// Example:
if (size > UINTPTR_MAX - address) {
    return NULL;
}
```


## `<stdalign.h>`
Alignment utilities in C11:
```c
#include <stdalign.h>
alignas(16) unsigned char stack[4096];
size_t alignment = alignof(uint64_t);
```
Provides convenient names for:
```c
_Alignas
_Alignof
```
Useful for:
- stacks
- page tables
- DMA buffers
- allocator blocks
- SIMD data

Modern projects may use `_Alignas` and `_Alignof` directly.



## `<stdarg.h>`
Variable argument handling:
```c
#include <stdarg.h>
void log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ...
    va_end(args);
}
```
Provides:
```c
va_list
va_start
va_arg
va_copy
va_end
```
This header itself is usually usable in bare-metal code because the compiler supports the calling convention.  
However, `<stdarg.h>` does not provide `printf`. It only helps me implement functions like my own:
```c
kernel_printf()
uart_printf()
panic()
log()
```


## `<stdatomic.h>`

Atomic types and operations:

```c
#include <stdatomic.h>

atomic_uint counter;

atomic_fetch_add(&counter, 1);
```

Important features:
```c
atomic_int
atomic_uint
atomic_flag

atomic_load
atomic_store
atomic_exchange
atomic_compare_exchange_strong
atomic_fetch_add
```

Whether it works easily depends on the compiler and architecture.  

The compiler may:
- emit native atomic instructions
- emit calls to helper functions such as `__atomic_*`

If helper functions are emitted, you may need compiler runtime support such as `libgcc` or `compiler-rt`  
For a small kernel, architecture-specific atomic assembly is sometimes used instead.



## `<stdnoreturn.h>`
For functions that never return:
```c
#include <stdnoreturn.h>
noreturn void panic(void)
{
    while (1) {
    }
}
```
Equivalent modern form:
```c
_Noreturn void panic(void);
```
Useful for:
- panic handlers
- kernel entry code
- reset functions
- fatal exception handlers



## `<iso646.h>`
Alternative operator spellings:
```c
and
or
not
bitand
bitor
xor
```
For example:
```c
if (a and b) {
}
```
This header is standard, but rarely used in kernel code. Normal operators are preferred:
```c
&& || ! & | ^
```


## `<float.h>`
Floating-point implementation limits:
```c
#include <float.h>
FLT_MAX
DBL_MAX
FLT_EPSILON
```
The header itself is easy to include.  
However, kernels often avoid floating-point operations because:  
- floating-point registers may need special context switching
- interrupt handlers should not casually use floating point
- early boot may not enable the floating-point unit
- compiler-generated floating-point code may require runtime support


## `<assert.h>`

Provides:
```c
assert(condition);
```
But this one is only partially easy.  
The macro may call an implementation-specific function when the assertion fails. In a hosted libc, that function usually prints a message and aborts.  
In a kernel, you commonly define your own assertion macro:  
```c
#define KASSERT(condition)            \
    do {                              \
        if (!(condition)) {           \
            kernel_panic(#condition); \
        }                             \
    } while (0)
```
So `<assert.h>` may compile, but its failure behavior may not be useful without libc support.


# Standard headers that declare useful functions which usually must implemented

