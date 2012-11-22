#ifndef __MULTIBOOT_INT_H__
#define __MULTIBOOT_INT_H__

#include <stdint.h>

/*
 * Multiboot header, macros.
 */
#define MB_ALIGN    (1 << 0)
#define MB_MEMINFO  (1 << 1)
#define MB_FLAGS    (MB_ALIGN | MB_MEMINFO)
#define MB_MAGIC    0x1BADB002
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

/*
 * The following value must be in %eax.
 */
#define MB_BOOTLOADER_MAGIC 0x2BADB002

#define MB_CHECK_FLAG(flags, flag) ((flags) & (flag))

#define MB_MEMORY_AVAILABLE 1
#define MB_MEMORY_RESERVED  2

typedef struct {
  uint32_t size;
  uint64_t addr;
  uint64_t len;
  uint32_t type;
} __attribute__((packed)) mb_memory_map_t;

typedef struct {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t cmdline;
  uint32_t pad;
} mb_module_t;

typedef struct {
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
} mb_aout_symbol_table_t;

typedef struct {
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} mb_elf_section_header_table_t;

typedef struct {
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  union {
    mb_aout_symbol_table_t aout_sym;
    mb_elf_section_header_table_t elf_sec;
  } u;
  uint32_t mmap_length;
  uint32_t mmap_addr;
  uint32_t drives_length;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;
} mb_info_t;

void mb_check(uint32_t magic, uint32_t *address);

mb_info_t *mb_get_info(void);

#endif
