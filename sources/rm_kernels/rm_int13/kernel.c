#include "types.h"
#include "own_bios.h"
#include "stdio.h"
#include "screen.h"
#include "stdlib.h"
#include "debug.h"
#include "seg.h"

__asm__("jmpl $0x0, $main	;\n");

#define __NOINLINE __attribute__((noinline))
#define __REGPARM  __attribute__((regparm(3)))
#define __NORETURN __attribute__((noreturn))

typedef struct _dap {
  uint8_t size; /* Size of DAP = 16, 0x10 bytes */
  uint8_t unused; /* Should be zero */
  uint16_t nb_sectors_to_read; /* some BIOSes limit it to 127 */
  uint16_t buffer_low; /* segment:offset pointer to the memory buffer to which sectors 
                      will be transferred (note that x86 is little-endian: if declaring 
                      the segment and offset separately, the offset must be 
                      declared before the segment) */
  uint16_t buffer_high;
  uint64_t offset; /* The segment offset where to start the reading */
} __attribute__((packed)) dap;

int __NOINLINE __REGPARM read_first_sector(uint8_t *sector) {
  // Dap structure
  dap d;
  d.size = 0x10;
  d.unused = 0x0;
  d.nb_sectors_to_read = 0x1;
  d.buffer_high = 0x0; // Segment 0x0
  d.buffer_low = (uint16_t) (uint32_t)sector; // Offset address of sector in the stack
  d.offset = 0;
  uint8_t c = 0;
  uint16_t ret = 0; 
  __asm__ __volatile__ ("mov %0, %%esi" : : "a"((uint16_t*)&d));
  __asm__ __volatile__ ("int $0x13" : "=cc" (c), "=a" (ret) : "a" (0x4200), "d" (0x80));
    
  if (c) {
    return ret;
  } else {
    return 0;
  }
}

//#define DEBUG

int __NORETURN main(void) {
  screen_clear();
  printk("Time to own the bios...\n");
  run_protected(&own_bios, 0xf831f);
  printk("Bios owned\n:))\n");
  // Sector read
  // printk("bonjour\n");
  // dump_core_state();
  // while(1);
  uint8_t sector[512]; // The sector data
  if (read_first_sector(sector)) {
    printk("FAILED\n");
  } else {
    printk("SUCCESS\n");
    uint32_t i;
    char buf[11];
    for (i = 0; i < 0x10; i++) {
    // Index
#ifdef DEBUG
      printk("sector[");
      itoa(buf, 16, i);
      buf[8] = ' ';
      buf[9] = '\0';
      printk(buf);
      printk("] = ");
#endif
      // 4 Octets du secteur
      itoa(buf, 16, ((uint32_t *)sector)[i]);
      buf[8] = '\r';
      buf[9] = '\n';
      buf[10] = '\0';
      printk(buf);
#ifdef DEBUG
      printk("\r\n");
#endif
#ifndef DEBUG
      printk(", ");
#endif
    }
  }
  printk("end\n:))");
  while (1);
}

void hook_bios(struct core_gpr *gpr) {
  screen_clear();
  printk("hook bios\n");
  __asm__ __volatile__(
    "movl 0x2640, %eax;"
    "movl %eax, %cr4;"
  );
  dump_core_state(gpr);
}
