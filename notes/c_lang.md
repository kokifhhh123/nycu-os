# Integer types and machine-width assumptions

```c
#include <stdint.h>
#include <stddef.h>

uint8_t  byte;
uint32_t register_value;
uint64_t physical_address;
uintptr_t address_as_integer;
size_t length;
ptrdiff_t difference;
```
`uint8_t`, `uint32_t`, `uint64_t`: exact-width unsigned integers  
`uintptr_t`: integer large enough to store a pointer  
`size_t`: object size and array index type  
`ptrdiff_t`: result of subtracting two pointers  


# Bitwise operations
```c
#define UART_ENABLE (1u << 0)
#define UART_TX_INT (1u << 1)

reg |= UART_ENABLE;       // set bit
reg &= ~UART_ENABLE;      // clear bit
reg ^= UART_ENABLE;       // toggle bit

// Extracting a bit field:
uint32_t field = (reg >> 8) & 0xFu;

// Replacing a bit field
reg &= ~(0xFu << 8);
reg |= (value & 0xFu) << 8;

// Always use unsigned constants for shifts:
1u << 31
// instead of:
1 << 31
```

# Pointer arithmetic
```c
// Pointers represent typed addresses
// p contains the address 0x3F215040
// *p means “access a uint32_t stored at that address”
uint32_t *p = (uint32_t *)0x3F215040;
// This performs a 32-bit store.
*p = 0x12345678;
```

Pointer arithmetic is scaled by the pointed-to type.  
```c
uint32_t *p = (uint32_t *)0x1000;
p + 1;   // address 0x1004
p + 2;   // address 0x1008

// For a byte pointer:
uint8_t *p = (uint8_t *)0x1000;
p + 1;   // address 0x1001

// For a structure pointer:
struct task *p;
p + 1;   // advances by sizeof(struct task)


// Standard C does not define arithmetic on void *.
void *p;
p + 1;       // not standard C
// portable code should cast it:
void *p;
uint8_t *next = (uint8_t *)p + 1;
```

## Pointer subtraction and comparison
Subtracting pointers gives the number of elements between them:
```c
uint32_t array[10];

ptrdiff_t distance = &array[8] - &array[2];
// distance == 6, not 24
```
Pointer subtraction and ordering comparisons are normally only defined when both pointers refer to the same array object.  
```c
p2 - p1;
p1 < p2;
```
For explicit address comparison, kernel code often converts to uintptr_t:  
```c
if ((uintptr_t)p1 < (uintptr_t)p2) {
    ...
}
```

# Arrays versus pointers
```c
int a[10];
int *p = a;
```
`a` is an array object containing ten integers.  
`p` is a pointer variable containing an address.  
```c
sizeof(a);   // 10 * sizeof(int)
sizeof(p);   // size of one pointer
```
For most expressions, an array decays into a pointer to its first element:  
```c
a           // usually becomes &a[0]
a[i]        // *(a + i)
i[a]        // *(i + a)
```

## a, &a[0], and &a
Given:  
```c
int a[10];
```
These have related addresses but different types:  
```c
a       // int *
&a[0]   // int *
&a      // int (*)[10]

a + 1   // address + sizeof(int)
&a + 1  // moves forward by one entire array:
// address + sizeof(int[10])
// address + 10 * sizeof(int)
```
`int (*)[10]` means:  
a pointer to an array of 10 integers  
```c
int (*)[10]
// * → pointer
// [10] → array of 10 elements
// int → each element is an int
```
### Example:  
```c
int a[10];

int *p1 = a;
int (*p2)[10] = &a;
```
Then we have:  
```c
p2          // pointer to array
*p2         // the array itself
(*p2)[3]    // element 3 of that array
```
```c
(*p2)[0] = 10;
(*p2)[3] = 20;
```

```c
p2 + 1      // moves by one whole array, meaning 10 * sizeof(int) bytes.
(*p2) + 1   // moves by one int, because *p2 decays to int *.
```

() is important  
```c
int *p[10];      // array of 10 pointers to int
int (*p)[10];    // pointer to an array of 10 ints
```

### Multidimensional arrays
```c
// A two-dimensional array is an array of arrays:
int matrix[3][4];

// When passed to a function, the inner dimension must be known:
void process(int matrix[][4]);
// Equivalent:
void process(int (*matrix)[4]);

// It is not equivalent to:
void process(int **matrix);
// int[3][4] is one contiguous memory block. int ** normally represents a pointer to pointers.
```


## Arrays in function parameters
This function:  
```c
void process(int a[10]);
```
is adjusted by the compiler to:  
```c
void process(int *a);

// Inside the function:
sizeof(a)       // returns the size of a pointer, not the size of the original array.

// Pass the length separately:
void process(int *a, size_t count);

// can express a minimum required size:
void process(int a[static 10]);
```


## Address alignment
```c
// Common power-of-two alignment operation:
// This formula requires alignment to be a power of two.
#define ALIGN_UP(value, alignment) \
    (((value) + (alignment) - 1) & ~((alignment) - 1))

uintptr_t x = ALIGN_UP(0x1003, 8);
// result: 0x1008
```



# Structure padding and alignment
```c
struct example {
    uint8_t  a;
    uint32_t b;
};
// The layout will be:
//     offset 0: a
//     offset 1-3: padding
//     offset 4-7: b
sizeof(struct example)  // be 8, not 5.

// Inspect offsets with:
#include <stddef.h>
offsetof(struct example, b)
```


## Packed structures
A compiler extension can suppress normal padding:  
A **packed** structure is a structure where the compiler is instructed _not to insert its normal padding bytes_ between members.
```c
struct __attribute__((packed)) header {
    uint8_t type;
    uint32_t length;
};
// The layout becomes:
// offset 0: type       1 byte
// offset 1: length     4 bytes
// total: 5 bytes


// But packed members may become unaligned:
struct header h;
uint32_t *p = &h.length;   // potentially unsafe
// Packed structures are useful for external binary formats, but should not be used casually.


// Avoid directly taking the address of an unaligned packed member:
uint32_t *p = &header.length;  // potentially unsafe

// A safer approach is to copy the bytes into an aligned local variable:
uint32_t length;
memcpy(&length, buffer + 1, sizeof(length));
```

## Why packed structures are needed  
### Binary protocols

Suppose a protocol defines:
```
byte 0: message type
bytes 1–4: message length
```
A normal structure would insert three bytes after type, so its memory layout would not match the protocol.


### File formats
Executable formats, filesystem metadata, image formats, and other binary files may define exact byte offsets.
Examples include:
- ELF headers;
- BMP headers;
- filesystem superblocks;
- firmware image headers.

## Packed versus aligned
`packed` minimizes padding and allows members to be unaligned.  
```c
struct __attribute__((packed)) header;
```
`aligned(16)` requires the structure itself to begin at a 16-byte-aligned address.
```c
struct __attribute__((aligned(16))) header;
```
They can also be combined:  
```c
struct __attribute__((packed, aligned(4))) header {
    uint8_t type;
    uint32_t length;
};
// members remain tightly packed;
// the structure itself has 4-byte alignment;
// length is still at offset 1 and therefore still unaligned.
```

## Verify the actual layout
Use `sizeof` and `offsetof`:
```c
#include <stddef.h>

_Static_assert(sizeof(struct header) == 5,
               "unexpected header size");

_Static_assert(offsetof(struct header, length) == 1,
               "unexpected length offset");
```





# `volatile`
Tells the compiler that each access is observable and must not be optimized away.  
```c
// Typical MMIO example:
#define UART_STATUS \
    (*(volatile uint32_t *)0x3F215054)

while ((UART_STATUS & (1u << 5)) == 0) {
}
```
Without volatile, the compiler might read the register once and reuse the old value forever.  

# `const` with pointers

```c
// Pointer to read-only integer:
const int *p;
    *p = 1;   // forbidden
    p = q;    // allowed
```
```c
// Constant pointer to writable integer:
int *const p = address;
    *p = 1;   // allowed
    p = q;    // forbidden
```
```c
// Constant pointer to read-only integer.
const int *const p = address;
```
```c
// For MMIO, you may see:
volatile const uint32_t *status;
```
```c
int *const p;        // const pointer to int

const int *p;        // pointer to const int
int const *p;        // pointer to const int

const int *const p;  // const pointer to const int
```






# Function pointers
A function pointer stores the address of executable code.
```c
int add(int a, int b);

int (*operation)(int, int) = add;

int result = operation(2, 3);

// Use typedef to simplify it:
typedef int (*binary_operation_t)(int, int);
binary_operation_t operation = add;

// Kernel uses include:
//      interrupt handlers
//      device-driver operation tables
//      system-call tables
//      callbacks
//      scheduler policies
//      architecture abstraction layers

// ex
struct file_operations {
    int     (*open)(void *file);
    ssize_t (*read)(void *file, void *buf, size_t size);
    int     (*close)(void *file);
};
```


# `typedef` can hide pointer semantics
```c
typedef int *int_ptr;
const int_ptr p = address;

// This means:
int *const p = address;     // const pointer to int

// not
const int *p;               // pointer to const int
```



# Linkage: `static` and `extern`
```c
static int counter;         // This symbol has internal linkage. It is visible only inside that source file.
static void helper(void);   // makes helper private to the source file.

extern int counter;     // This declares a variable defined elsewhere.
```
## Storage duration and lifetime
```c
int global;
static int file_only;
void f(void)
{
    int x;                      // Automatic storage: x normally lives on the stack until the function returns.
    static int y;               // Static storage: exist for the entire program lifetime.
    void *p = malloc(size);     // Dynamic storage: Exists until explicitly freed.
}

// Never do this
int *wrong(void)
{
    int x = 10;
    return &x;        // dangling pointer
}
```


# Linker symbols and the address-of operator
A linker script may define:  
```
__bss_start = .;
__bss_end = .;
```
In C:
```c
extern char __bss_start[];
extern char __bss_end[];

// Then:
char *start = __bss_start;
char *end   = __bss_end;

// Another common declaration is:
extern char __kernel_end;
// Then its address is:
void *heap_start = &__kernel_end;
```
The linker symbol _usually represents an address_, not a normal C variable containing that address.




# Strict aliasing
The compiler assumes that pointers to unrelated types usually do not point to the same object.  
Consider:
```c
float f = 1.0f;
uint32_t bits = *(uint32_t *)&f;

// This code does three things:
&f                 // float *
(uint32_t *)&f     // pretend the address points to uint32_t
*(uint32_t *)&f    // read that memory as uint32_t

// Under strict-aliasing rules, the compiler may assume that:
float *
// and:
uint32_t *
// do not point to the same object.
```
## Why the compiler makes this assumption
Suppose you have:  
```c
void update(float *f, uint32_t *i)
{
    *f = 1.0f;
    *i = 0;
    use(*f);
}
// With strict aliasing, the compiler can assume f and i do not refer to the same object because they point to unrelated types.
// It may optimize the code approximately as:
*f = 1.0f;
*i = 0;
use(1.0f);
```
Safe method using `memcpy`
```c
float f = 1.0f;
uint32_t bits;

memcpy(&bits, &f, sizeof(bits));
// This copies the raw bytes from f into bits.
```
## Character-pointer exception
