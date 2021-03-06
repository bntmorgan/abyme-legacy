.global loader

.set FLAGS,    0
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# reserve initial kernel stack space
stack_bottom:
.skip 16384                             # reserve 16 KiB stack
stack_top:

.comm mbd, 4                           # we will use this in kmain
.comm magic, 4                         # we will use this in kmain

loader:
    movl  $stack_top, %esp              # set up the stack, stacks grow downwards
    movl  %eax, magic                   # Multiboot magic number
    movl  %ebx, mbd                     # Multiboot data structure
    cli

    /* Copy the bootstrap. */
    cld
    mov $bootstrap_start, %esi
    mov $0x600, %edi
    mov $(bootstrap_end - bootstrap_start), %ecx
    rep movsb

    /* Copy the kernel. */
    cld
    mov $kernel_start, %esi
    mov $0x700, %edi
    /* Note: The linker isn't able to compute $(kernel_end - kernel_start) */
    mov $(kernel_end), %ecx
    sub $(kernel_start), %ecx
    rep movsb

    /* unprotect the BIOS memory */
    // XXX Unprotecting the bios memory
    // Copying the BIOS flash in ram
    // See Intel 3rd generation core PAM0-PAM6
    movb $0x30, 0xf8000080
    movb $0x30, 0xf8000081
    movb $0x33, 0xf8000082
    movb $0x00, 0xf80f80d8

    lgdt gdtr
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmpl $0x18, $0x600

.code16
bootstrap_start:
    mov %cr0, %eax
    and $0xfffffffe, %eax
    mov %eax, %cr0
    xor %eax, %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    // Set our stack
    mov $0x2000, %sp
    jmpl $0x0, $0x700
bootstrap_end:

.code16
// Here is the 16bits real mode code
kernel_start:
    mov $0xb800, %ax
    mov %ax, %ds
    // Printing some chars LULZ
    movb $'n', 0x00
    movb $'o', 0x02
    movb $'k', 0x04
    // Own the handler
    mov $0xf000, %ax
    mov %ax, %ds
    movw $0x9090, 0x80bf
    // Print som ok chars
    mov $0xb800, %ax
    mov %ax, %ds
    // Printing some chars LULZ
    movb $'s', 0xa0
    movb $'u', 0xa2
    movb $'i', 0xa4
    movb $'c', 0xa6
    movb $'i', 0xa8
    movb $'d', 0xaa
    movb $'e', 0xac
    // Save %ds
    push %ds
    mov $0x00, %ax
    mov %ax, %ds
    call int13
    // Get the last %ds
    pop %ds
    movb $'o', 0x140
    movb $'k', 0x142
lulz:
    jmp lulz
int13:
  // Stack frame
  push %bp
  mov %sp, %bp
  // Sector
  sub $512, %sp
  mov %sp, %ax
  // Dap
  sub $0x10, %sp
  mov %sp, %bx
  // dap.size
  movb $0x10, 0x0(%bx)
  // dap.unused
  movb $0x00, 0x1(%bx)
  // dap.sectors_to_read
  movw $0x0001, 0x2(%bx)
  // dap.buffer_low : offset
  movw %ax, 0x4(%bx)
  // dap.buffer_high : segment
  movw $0x00, 0x8(%bx)
  // dap.offset (64 bits)
  movl $0x00000000, 0x0c(%bx)
  // dap.offset (64 bits)
  movl $0x00000000, 0x10(%bx)
  // The int13 DAP parameters
  mov %bx, %si
  mov $0x4200, %ax
  mov $0x80, %dx
  // int 13 dude
  xchg %bx, %bx
  int $0x13
  xchg %bx, %bx
  // Destroy the stack frame
  leave
  ret
kernel_end:

/*
 * Descriptors.
 */

.macro GDT_ENTRY_16 type, base, limit
  .word (((\limit) >> 12) & 0xffff)
  .word (((\base)  >>  0) & 0xffff)
  .byte (0x00 + (((\base)  >> 16) & 0xff))
  .byte (0x90 + (((\type)  >>  0) & 0x6f))
  .byte (0x80 + (((\limit) >> 28) & 0x0f))
  .byte (0x00 + (((\base)  >> 24) & 0xff))
.endm

.macro GDT_ENTRY_32 type, base, limit
  .word (((\limit) >> 12) & 0xffff)
  .word (((\base)  >>  0) & 0xffff)
  .byte (0x00 + (((\base)  >> 16) & 0xff))
  .byte (0x90 + (((\type)  >>  0) & 0x6f))
  .byte (0xC0 + (((\limit) >> 28) & 0x0f))
  .byte (0x00 + (((\base)  >> 24) & 0xff))
.endm

gdt:
  GDT_ENTRY_32 0x0,                               0x0, 0x00000000
  GDT_ENTRY_32 0x8 /* SEG X */ + 0x2 /* SEG R */, 0x0, 0xffffffff
  GDT_ENTRY_32 0x2 /* SEG W */,                   0x0, 0xffffffff
  GDT_ENTRY_16 0x8 /* SEG X */ + 0x2 /* SEG R */, 0x0, 0xffffffff
  GDT_ENTRY_16 0x2 /* SEG W */,                   0x0, 0xffffffff
gdt_end:

gdtr:
  .word gdt_end - gdt - 1
  .long gdt

