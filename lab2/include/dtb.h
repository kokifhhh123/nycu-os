#include "mini_uart.h"
#include "utils.h"
#include <stdint.h>

#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP        0x00000003
#define FDT_NOP         0x00000004
#define FDT_END         0x00000009

struct fdt_header {	
    uint32_t magic;			     /* magic word FDT_MAGIC */	
    uint32_t totalsize;		     /* total size of DT block */	
    uint32_t off_dt_struct;		 /* offset to structure */	
    uint32_t off_dt_strings;		 /* offset to strings */	
    uint32_t off_mem_rsvmap;		 /* offset to memory reserve map */	
    uint32_t version;		      /* format version */	
    uint32_t last_comp_version;	 /* last compatible version */ 	/* version 2 fields below */	
    uint32_t boot_cpuid_phys;	 /* Which physical CPU id we're booting on */	/* version 3 fields below */	
    uint32_t size_dt_strings;	 /* size of the strings block */ 	/* version 17 fields below */	
    uint32_t size_dt_struct;		 /* size of the structure block */
};

void initramfs_callback();
unsigned char* fdt_traverse();