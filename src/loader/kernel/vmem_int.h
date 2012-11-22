#ifndef __VMEM_INT_H__
#define __VMEM_INT_H__

#include <stdint.h>

#include "include/vmem.h"

void vmem_setup(vmem_info_t *vmem_info, uint32_t physical_mod_dest);

void vmem_print_info(void);

uint64_t vmem_addr_linear_to_logical_ds(uint64_t addr);

#endif
