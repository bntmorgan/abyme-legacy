#include "types.h"
#include "screen.h"
#include "stdio.h"

#include "hardware/msr.h"
#include "pmem.h"
#include "vmem.h"
#include "vmm.h"
#include "vmm_info.h"
#include "vmm_setup.h"
#include "smp.h"
#include "hardware/msr.h"
#include "debugger.h"

#include <efi.h>
#include <efilib.h>

/*
 * Used to identify the size used of vmm (see the linker script).
 */
extern uint8_t kernel_end;
extern uint8_t kernel_start;
extern uint8_t pepin_end;
extern uint8_t pepin_start;

/* Vector of addresses: segment (first word) : offset (second word) */
uint32_t bios_ivt[256];

vmm_info_t *vmm_info;

void kernel_print_info(void) {
  INFO("kernel_start: %08X\n", (uint64_t) &kernel_start);
  INFO("kernel_end:   %08X\n", ((uint64_t) &kernel_end) & 0xffffffff);
}

void dump_bios_ivt() {
  for (int i = 0; i < 256; i++) {
    bios_ivt[i] = *((uint32_t*) (uint64_t) (4 * i));
  }

  /*for (int i = 0; i < 256; i++) {
    INFO("vector for int 0x%02x: segment = %04x, offset = %04x\n", i, bios_ivt[i] >> 16, bios_ivt[i] & 0xFFFF);
  }*/
}

void write_bioshang(uint64_t addr_goal) {
  uint64_t i;
  extern uint64_t bioshang_start;
  extern uint64_t bioshang_end;
  uint64_t length = (uint64_t)(((uint8_t *)&bioshang_end) - ((uint8_t *)&bioshang_start));
  uint8_t *start = (uint8_t *)&bioshang_start;
  for (i = 0; i < length + 1; i++) {
    printk(" %x %x %x\n", addr_goal + i, start + i, *(start + i));
    *((uint8_t*)(addr_goal + i)) = *(start + i);
  }
}

void kernel_main(vmm_info_t *_vmm_info) {

  EFI_SYSTEM_TABLE **systab = (EFI_SYSTEM_TABLE **)0x80000000;
  
  while(1);

  if (*systab == (EFI_SYSTEM_TABLE *)0xcf5b6f18 ) {

    (*systab)->ConOut->OutputString((*systab)->ConOut, (CHAR16*)L"HZEUIF HUIZEF HUIZEF HUI FHZEUIHEZFUI HUEZI HFUIZE HFUIZE HFUIZEHFUI ZHEFUI HZEFUI HUEFZI FHUIZE FHUIZE HFU IZE HFUIZEHFUIEZHFUI HFUEZI FHUZIE FHUEZHFUIZE HFUI ZEHFUI HZEFUI HUFEZ HFUIZE FHUIEZ HFUIZEFHUI Hello World\n\r");
 
  }

  while(1);


  uint8_t *dst;
  uint32_t i;
  vmm_info = _vmm_info;
  screen_clear();
  dump_bios_ivt();

  /* Install new INT 0x15 handler at the end of the BIOS IVT (unused) */
  /* TODO: move it to another place? */
  *((uint8_t*) (255 * 4 + 4)) = 0x0f; /* VMCALL (3 bytes) */
  *((uint8_t*) (255 * 4 + 5)) = 0x01;
  *((uint8_t*) (255 * 4 + 6)) = 0xc1;
  *((uint8_t*) (255 * 4 + 7)) = 0xcf; /* IRET (1 byte) */
  /* Change the INT 0x15 handler address in the BIOS IVT */
  //*((uint16_t*) (4 * 0x15 + 2)) = 0;
  //*((uint16_t*) (4 * 0x15 + 0)) = 255 * 4;

//  *((uint8_t*) (254 * 4 + 0)) = 0xb2; /* MOV dl, 0x80 */
//  *((uint8_t*) (254 * 4 + 1)) = 0x80;
//  *((uint8_t*) (254 * 4 + 2)) = 0xcd; /* INT 0x19 (2 bytes) */
//  *((uint8_t*) (254 * 4 + 3)) = 0x19;

  /* TODO: create a variable, set to 7c00 by default and update this value according to
     the parameter provided by syslinux. Change also vmm_vmcs:RIP !!
   */
  dst = (uint8_t *) 0x7C00;
  dst = (uint8_t *) 0x700;
  INFO("from %x to %x\n", vmm_info->rm_kernel_start, (uint32_t) (uint64_t) dst);
  for (i = 0; i < vmm_info->rm_kernel_size; i++) {
    dst[i] = ((uint8_t *) (uint64_t) vmm_info->rm_kernel_start)[i];
  }
  for (i = 0; i < 16 ; i++) {
    printk("%02x ", dst[i]);
  }
  printk("\n");
  for (i = 0; i < 16 ; i++) {
    printk("%02x ", ((uint8_t *) (uint64_t) vmm_info->rm_kernel_start)[i]);
  }
  printk("\n");
  /*dst = (uint8_t *) 0x7C00;
  for (i = 0; i < &pepin_end - &pepin_start + 1; i++) {
    dst[i] = ((uint8_t *) (&pepin_start))[i];
  }
  */

  kernel_print_info();
  //vmem_print_info();
  //pmem_print_info(&vmm_info->pmem_mmap);
  pmem_fix_info(&vmm_info->pmem_mmap, VMEM_ADDR_PHYSICAL_TO_VIRTUAL(vmm_info->vmm_physical_start));
  //pmem_print_info(&vmm_info->pmem_mmap);

  /*
   * Enables core/cpu.
   */
  //smp_setup();

  vmm_read_cmos();
  vmm_setup();
  vmm_ept_setup(&vmm_info->ept_info, vmm_info->vmm_physical_start, vmm_info->vmm_physical_end - vmm_info->vmm_physical_start + 1);
  /* Enables the debugger */
  INFO("INIT THE DEBUGGER AT 0x700\n");
  debug_install();

  /**
   * XXX Overriding the int 13 handler
   */
  //write_bioshang(0xf80c6);
  
  vmm_vm_setup_and_launch();
}

