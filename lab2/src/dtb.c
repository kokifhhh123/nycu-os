#include "dtb.h"
#include <stdint.h>
// #include <string.h>

extern unsigned long _dtb_ptr;

unsigned int be32_to_cpu(unsigned int val) {
    return (val>>24 & 0x000000FF) | \
            (val>>8 & 0x0000FF00) | \
            (val<<8 & 0x00FF0000) | \
            (val<<24 & 0xff000000);
}

void test() {
    unsigned char *dtb = (unsigned char *)_dtb_ptr;
    struct fdt_header *header = (struct fdt_header *)dtb;
    uart_send_hex(be32_to_cpu(header->magic));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->totalsize));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->off_dt_struct));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->off_dt_strings));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->off_mem_rsvmap));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->version));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->last_comp_version));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->boot_cpuid_phys));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->size_dt_strings));
    uart_send_string("\n");
    uart_send_hex(be32_to_cpu(header->size_dt_struct));
    uart_send_string("\n");
    unsigned char *dt_struct = dtb + be32_to_cpu(header->off_dt_strings);
    int nn = be32_to_cpu(header->size_dt_strings);
    while (nn--) {
        char c = *dt_struct;
        if(c=='\0') uart_send('&');
        else {
            if(c=='\n') uart_send('\r');
            uart_send(c);
        }
        dt_struct++;
    }
    uart_send_string("\n");
}

void initramfs_callback() {
}

// uint32_t strlen(char *name) {
// }

uint32_t* align4_ptr(char *ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    addr = (addr + 3) & ~(uintptr_t)3;
    return (uint32_t *)addr;
}

unsigned long strlen(const char *s) {
    unsigned long len = 0;
    while(s[len] != '\0') {
        len++;
    }
    return len;
}

unsigned char* fdt_traverse() {
    void *dtb_ptr = (void*)_dtb_ptr;
    // unsigned long addr = _dtb_ptr;
    // uart_send_string("dtb address: ");
    // uart_send_hex(addr);
    // uart_send_string("\n");
    // char *ptr = dtb_ptr;
    unsigned char *dtb = (unsigned char *)_dtb_ptr;
    struct fdt_header *header = (struct fdt_header *)dtb;
    
    uint32_t *ptr = (uint32_t*)(dtb + be32_to_cpu(header->off_dt_struct));
    char *strings_block = (char*)(dtb + be32_to_cpu(header->off_dt_strings));
    int num = 0;
    int find_init = 0;
    unsigned int num_init = 0;
    unsigned int num_end = 0;
    char *startname, *endname;
    uint32_t start_addr;
    uint32_t end_addr;

    while (1) {
        uint32_t token = be32_to_cpu(*ptr);
        ptr++;
        char *now_name;
        if (token==FDT_BEGIN_NODE) {
            num++;
            char *name = (char*)ptr;
            // uart_send_string("BEGIN NODE: ");
            // uart_send_string(name);
            // uart_send_string("\n");
            ptr = align4_ptr(name + strlen(name) + 1);
            now_name = name;
        }
        else if (token == FDT_PROP) {
            uint32_t len = be32_to_cpu(*ptr++);
            uint32_t nameoff = be32_to_cpu(*ptr++);

            char *property_name = strings_block + nameoff;
            void *property_value = ptr;

            // uart_send_string("PROPERTY: ");
            // uart_send_string(property_name);
            // uart_send('\n');

            ptr = align4_ptr((char *)property_value + len);
            if (cmp_string(property_name, "linux,initrd-start")==0) {
                find_init = 1;
                num_init++;
                startname = now_name;
                // break;
                start_addr = be32_to_cpu(*(uint32_t*)property_value);
            }
            if (cmp_string(property_name, "linux,initrd-end")==0) {
                find_init = 1;
                num_end++;
                endname = now_name;
                // end_addr = *(unsigned int*)ptr;
                end_addr = be32_to_cpu(*(uint32_t*)property_value);
            }
            // if (cmp_string(property_name, ) && cmp)
        }
        else if (token == FDT_END_NODE) {
            // uart_send_string("END NODE\n");
            if (cmp_string(now_name, "chosen")==0) {
                // find_init = 1;
                // num_init++;
                // startname = now_name;
                break;
            }
        }
        else if (token == FDT_NOP) {
            continue;
        }
        else if (token == FDT_END) {
            break;
        }
    }
    // uart_send_hex(num);
    // uart_send_string("\n");

    // unsigned char *dt_struct = dtb + be32_to_cpu(header->off_dt_strings);
    // int nn = be32_to_cpu(header->size_dt_strings);
    // while (nn--) {
    //     char c = *dt_struct;
    //     if(c=='\0') uart_send('&');
    //     else {
    //         if(c=='\n') uart_send('\r');
    //         uart_send(c);
    //     }
    //     dt_struct++;
    // }
    // uart_send_string("\n");
    // if (find_init) {
    //     uart_send_string("find it\n");
    //     uart_send_hex(num_init);
    //     uart_send_string("\n");
    //     uart_send_hex(num_end);
    //     uart_send_string("\n");
    //     uart_send_string(startname);
    //     uart_send_string("\n");
    //     uart_send_string(endname);
    //     uart_send_string("\n");
    //     uart_send_hex(start_addr);
    //     uart_send_string("\n");
    //     uart_send_hex(end_addr);
    //     uart_send_string("\n");
    // } else {
    //     uart_send_string("not find\n");
    // }
    // uart_send_string("\n");
    return (unsigned char*)(uintptr_t)start_addr;
}