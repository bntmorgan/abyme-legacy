#include "common/string_int.h"

#include "pmem_int.h"

pmem_mmap_t pmem_mmap;

void pmem_setup(mb_info_t *mb_info) {
  if (mb_info->mmap_length > sizeof(pmem_mmap)) {
    ERROR("too many information in the memory mapping\n");
  }
  /*
   * Copy memory mapping information.
   */
  uint8_t *mb_mmap_end = ((uint8_t *) mb_info->mmap_addr) + mb_info->mmap_length;
  mb_memory_map_t *mb_mmap_ptr = (mb_memory_map_t *) mb_info->mmap_addr;
  pmem_mmap.nb_area = 0;
  while ((uint8_t *) mb_mmap_ptr < mb_mmap_end) {
    pmem_mmap.area[pmem_mmap.nb_area].size = mb_mmap_ptr->size;
    pmem_mmap.area[pmem_mmap.nb_area].addr = mb_mmap_ptr->addr;
    pmem_mmap.area[pmem_mmap.nb_area].len = mb_mmap_ptr->len;
    pmem_mmap.area[pmem_mmap.nb_area].type = mb_mmap_ptr->type;
    pmem_mmap.nb_area = pmem_mmap.nb_area + 1;
    mb_mmap_ptr = (mb_memory_map_t *) (((uint8_t *) mb_mmap_ptr)
        + mb_mmap_ptr->size + sizeof(mb_mmap_ptr->size));
  }
}

uint64_t pmem_get_stealth_area(uint32_t size, uint32_t alignment) {
  /*
   * Start from the end of the memory map and look for
   * a free area large enough for our code.
   */
  for (uint8_t i = 0; i < pmem_mmap.nb_area; i++) {
    uint8_t j = pmem_mmap.nb_area - i - 1;
    if ((pmem_mmap.area[j].type == 1) && (pmem_mmap.area[j].len > (size * 2))) {
      /*
       * Allocated memory must be aligned.
       */
      uint64_t value = ~((((uint64_t) 1) << alignment) - 1);
      value = ((uint64_t) (pmem_mmap.area[j].addr + pmem_mmap.area[j].len - size)) & value;
      if (value > pmem_mmap.area[j].addr) {
        return value;
      }
    }
  }
  return 0;
}

void pmem_copy_info(pmem_mmap_t *dest) {
  uint8_t *from8 = (uint8_t *) &pmem_mmap;
  uint8_t *dest8 = (uint8_t *) dest;
  INFO("copy from %08x to %08x\n", from8, dest8);
  for (uint32_t i = 0; i < sizeof(pmem_mmap); i++) {
    dest8[i] = from8[i];
  }
}

void pmem_print_info(mb_info_t *mb_info) {
  INFO("mmap: at %08x with a size of %08x bytes and %d sections\n",
      (unsigned int) mb_info->mmap_addr, (unsigned int) mb_info->mmap_length,
      pmem_mmap.nb_area);
  for (uint8_t i = 0; i < pmem_mmap.nb_area; i++) {
    INFO("size=%2x addr=%08x%08x len=%08x%08x type=%01x\n",
        (uint32_t) (pmem_mmap.area[i].size),
        (uint32_t) (pmem_mmap.area[i].addr >> 32),
        (uint32_t) (pmem_mmap.area[i].addr & 0xffffffff),
        (uint32_t) (pmem_mmap.area[i].len >> 32),
        (uint32_t) (pmem_mmap.area[i].len & 0xffffffff),
        (uint32_t) (pmem_mmap.area[i].type));
  }
}
