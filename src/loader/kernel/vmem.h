#ifndef __VMEM_H__
#define __VMEM_H__

#include "types.h"

#include "vmm_info.h"

void vmem_setup(vmem_info_t *vmem_info);

void vmem_print_info(void);

uint64_t vmem_addr_linear_to_logical_ds(uint64_t addr);

#endif
