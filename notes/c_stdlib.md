`<stdint.h>` is part of the C standard library. The difference is not “**standard library**” versus “**not standard library.**”  
Distinctions:
1. Headers that mostly provide types, constants, and macros
2. Headers that declare functions requiring a libc implementation, runtime, operating system, heap, files, or system calls

# Easy to use
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


# Must implemented

## `<string.h>`

Memory and string functions:
```c
memcpy
memmove
memset
memcmp

strlen
strcmp
strncmp
strcpy
strncpy
strcat
strchr
strstr
```
In kernel code, the most important are:
```c
memcpy
memmove
memset
memcmp
strlen
strcmp
strncmp
```
Example:
```c
memset(bss_start, 0, bss_size);
memcpy(destination, source, size);
```
These functions do not inherently require an operating system. They are easy to implement yourself.

Important caveat: compilers may generate calls to `memcpy` or `memset` even when your C code does not explicitly call them.

For example:
```c
struct large_object a;
struct large_object b;

a = b;
```
The compiler may internally generate:
```c
memcpy(&a, &b, sizeof(a));
```
Therefore, freestanding kernels commonly provide at least:
```c
void *memcpy(void *dst, const void *src, size_t size);
void *memset(void *dst, int value, size_t size);
void *memmove(void *dst, const void *src, size_t size);
int memcmp(const void *a, const void *b, size_t size);
```



## `<stdlib.h>`
This header contains several different categories.  
### Dynamic memory functions
```c
malloc
calloc
realloc
free
aligned_alloc
```
These require a heap allocator.  

A bare-metal kernel cannot use them until you implement memory management, such as:  
- bump allocator
- free-list allocator
- buddy allocator
- slab allocator
- page allocator

You may provide your own versions:
```c
void *malloc(size_t size);
void free(void *pointer);
```
But kernel projects often use different names:  
```c
kmalloc
kfree
page_alloc
page_free
```
### Process-control functions
```c
abort
exit
atexit
quick_exit
```
These assume some runtime or process model.  

A kernel usually replaces them with:
```c
panic
halt
shutdown
reboot
```
### String conversion functions
```c
strtol
strtoul
atoi
atol
```
These can be implemented without an operating system and may be useful in a kernel shell.  

### Searching and sorting
```c
qsort
bsearch
```
These do not inherently require an OS, but still require a libc implementation.  




## `<stdio.h>`

Standard input/output:
```c
printf
fprintf
sprintf
snprintf

fopen
fclose
fread
fwrite

stdin
stdout
stderr

putchar
puts
getchar
```
This is difficult in early kernel programming because it assumes an I/O abstraction.  

`printf` itself requires at least:  
- formatting logic
- an output destination
- character output support
- variable arguments

A kernel can implement its own:
```c
uart_putc
uart_puts
kernel_printf
```
For example:
```c
void uart_putc(char c);
void uart_puts(const char *text);
void kernel_printf(const char *format, ...);
```
File functions require much more infrastructure:
```c
fopen
fread
fwrite
fseek
fclose
```
They normally require:  
- filesystem
- file descriptors or file objects
- storage drivers
- buffering
- error handling

Do not normally include `<stdio.h>` in an early bare-metal kernel.




## `<time.h>`
Time-related functions and types:
```c
time
clock
difftime
mktime
localtime
gmtime
strftime
```
These require some source of time and usually runtime support.  

A kernel must provide hardware-specific support using:  
- CPU timer
- system timer
- real-time clock
- platform firmware
- timer interrupts

The types may be easy to use:
```c
time_t
clock_t
struct tm
```
But the functions will not work unless implemented.



## `<errno.h>`
Provides error codes and errno:
```c
EINVAL
ENOMEM
EIO
EBUSY
ENOENT
```
The macros may be usable, depending on the toolchain.  
However:
```c
errno
```
is usually implemented as runtime or thread-local state.  
In kernel code, functions commonly return negative error values:  
```c
return -EINVAL;
return -ENOMEM;
return -EIO;
```
Many kernels define their own error header rather than depending on the hosted libc version.  




## `<ctype.h>`
Character classification and conversion:
```c
isdigit
isalpha
isalnum
isspace
tolower
toupper
```
These functions are easy to implement and do not require an operating system.  

However, a libc implementation may involve locale support. A small kernel usually implements simple ASCII-only versions:  
```c
static inline bool ascii_isdigit(char c)
{
    return c >= '0' && c <= '9';
}
```



## `<math.h>`
Floating-point mathematics:  
```c
sqrt
sin
cos
tan
pow
log
exp
floor
ceil

// These generally require a mathematics library, often linked with:
-lm

// In kernel code, using them is usually difficult because:
//     implementations may be large
//     they may use floating-point instructions
//     they may require runtime support
//     the kernel must preserve floating-point CPU state
//     floating-point is often prohibited in interrupt or kernel context
```
Most early kernels avoid `<math.h>` entirely.



## `<locale.h>`
Locale handling:
```c
setlocale
localeconv
```
This assumes locale databases and runtime support.  

Almost never useful in an early kernel or firmware project.  



## `<signal.h>`
Hosted C signal handling:
```c
signal
raise
SIGINT
SIGABRT
SIGSEGV
```
These are user-space process concepts.  
They are not the same as hardware interrupts or CPU exceptions.  

A kernel implements its own:
- exception vectors
- IRQ handlers
- process signals
- fault delivery
Do not use `<signal.h>` for hardware interrupt handling.  



## `<setjmp.h>`
Non-local control transfer:
```c
setjmp
longjmp
```
These save and restore part of the execution context.  

They may require assembly or compiler runtime support.  

A kernel can implement similar functionality, but it must carefully handle:  
- registers
- stack pointer
- callee-saved state
- architecture ABI
Not normally used in beginner kernels.  



# Depend strongly on hosted environments

These are generally unsuitable for a small bare-metal project.  

## `<threads.h>`
C11 threading:
```c
thrd_create
thrd_join
mtx_lock
cnd_wait
```
Requires:  
- scheduler
- thread creation
- blocking
- mutexes
- condition variables
- thread-local storage
A kernel implements these mechanisms itself.



## `<unistd.h>`
This is important: `<unistd.h>` is **not part of ISO C.**  
It is POSIX.  
It declares functions such as:
```c
read
write
close
fork
exec
sleep
getpid
```
These require an operating system and system-call interface.  
Do not use it in a bare-metal kernel unless your project intentionally provides compatible functions.  



## `<sys/types.h>`, `<sys/stat.h>`, `<fcntl.h>`
These are also generally POSIX or operating-system-specific, not ISO C.  
Examples:
```c
open
stat
mode_t
off_t
ssize_t
```
They require a filesystem and OS interface.  


## `<pthread.h>`
POSIX threads, not standard C:
```c
pthread_create
pthread_mutex_lock
pthread_cond_wait
```
Requires an operating system or threading runtime.  
Not suitable for early bare-metal code.  



# Compiler-provided headers
These are not necessarily ISO C library headers, but they are often available directly from the compiler.  
`<stdalign.h>`, `<stdatomic.h>`, `<stdarg.h>`  
Although they are standard headers, much of their functionality is implemented directly by the compiler.  


## `<stdckdint.h>`
C23 checked integer arithmetic:  
```c
#include <stdckdint.h>
size_t result;
if (ckd_add(&result, a, b)) {
    // overflow
}
```
Operations:
```c
ckd_add
ckd_sub
ckd_mul
```
Availability depends on compiler and C library support.  

For older compilers, you can use compiler built-ins:  
```c
__builtin_add_overflow
__builtin_sub_overflow
__builtin_mul_overflow
```
Example:

```c
size_t total;

if (__builtin_add_overflow(header_size, payload_size, &total)) {
    return ERROR;
}
```
These built-ins are very useful in allocator and parser code.


## `<immintrin.h>`, `<arm_neon.h>`
Architecture-specific intrinsic headers.  
Examples:  
- `<immintrin.h>` for x86 SIMD
- `<arm_neon.h>` for ARM NEON
- `<arm_acle.h>` for ARM architecture intrinsics

These are not portable ISO C, but may be usable without an OS because the compiler translates them into machine instructions.  


# can compile but still cause linker errors
A header only tells the compiler:
> This function exists, and this is its type.

```c
#include <string.h>
memset(buffer, 0, 100);

// The compiler accepts the declaration from <string.h>.

// But later, the linker may report:
//     undefined reference to `memset`
```

The same applies to:  
```c
printf
malloc
free
strlen
memcpy
sqrt
abort
```
Including a header does not provide the function implementation.  

## Compiler runtime helper functions
Even without libc, the compiler may generate calls to helper functions.  
Common examples include:  
```c
__aeabi_*
__divdi3
__udivdi3
__moddi3
__muldi3
__atomic_*
memcpy
memset
```
These may appear when the CPU cannot directly perform an operation or when the compiler chooses a helper.  
For example, on some architectures:  
```c
uint64_t result = a / b;
```
may generate a call to: `__udivdi3`

These helpers are commonly provided by:
- `libgcc`
- LLVM `compiler-rt`


# Practical classification for kernel
## safe
```c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>
#include <stdalign.h>
```
Potentially safe, but verify toolchain behavior:
```c
#include <stdatomic.h>
#include <stdnoreturn.h>
#include <assert.h>
```

## not safe
```c
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
```
Typical kernel replacements:  
```c
memcpy
memset
memmove
memcmp
strlen
strcmp

kmalloc
kfree

ascii_isdigit
ascii_isspace
```

# Header set for Pi kernel
```c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
```
When implementing formatted UART output: `#include <stdarg.h>`  
When handling alignment: `#include <stdalign.h>`  
should probably provide small kernel library containing:
```c
void *memcpy(void *dst, const void *src, size_t size);
void *memmove(void *dst, const void *src, size_t size);
void *memset(void *dst, int value, size_t size);
int memcmp(const void *a, const void *b, size_t size);

size_t strlen(const char *string);
int strcmp(const char *a, const char *b);

void uart_putc(char character);
void uart_puts(const char *string);
void kernel_printf(const char *format, ...);

void *kmalloc(size_t size);
void kfree(void *pointer);
```