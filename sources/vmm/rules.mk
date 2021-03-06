sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)

KERNEL			:= $(call SRC_2_BIN, $(d)/vmm.elf32)
KERNELS			+= $(KERNEL)
OBJS_32_$(d)		:= $(call SRC_2_OBJ, $(d)/kernel/kernel.o $(d)/common/screen.o      \
			   $(d)/kernel/pmem.o $(d)/hardware/msr.o $(d)/kernel/vmm.o         \
			   $(d)/kernel/vmm_setup.o $(d)/kernel/vmm_vmcs.o                   \
			   $(d)/common/stdio.o $(d)/common/stdlib.o $(d)/kernel/start.o     \
			   $(d)/kernel/vmm_vm_exit_handler.o $(d)/kernel/debugger.o 						\
			   $(d)/common/string.o $(d)/kernel/vmem.o  $(d)/hardware/cpu.o    \
				 $(d)/kernel/bioshang.o $(d)/kernel/stm.o $(d)/common/keyboard.o \
				 $(d)/common/debug.o)
OBJECTS			+= $(OBJS_32_$(d))
# XXX
$(OBJS_32_$(d)):	EFI_INCLUDE			:= /usr/include/efi
$(OBJS_32_$(d)):	ARCH 						:= $(shell uname -m | sed s,i[3456789]86,ia32,)
$(OBJS_32_$(d)):	EFI_INCLUDES 		:= -DEFI_FUNCTION_WRAPPER -I$(EFI_INCLUDE) -I$(EFI_INCLUDE)/$(ARCH) -I$(EFI_INCLUDE)/protocol
# XXX end
$(OBJS_32_$(d)):	CC_FLAGS_KERNEL	:= -I$(d) -I$(d)/include -Wall -W -pipe -nostdlib \
					   -nostdinc -ffreestanding -fms-extensions -m64  \
					   -mno-red-zone -mcmodel=small -fpie -std=c99    \
					   -D__X86_64__ $(EFI_INCLUDES)
$(KERNEL):		LD_FLAGS_KERNEL	:= --warn-common --no-check-sections -n -melf_x86_64 -pie
$(KERNEL):		LD_OBJECTS	:= $(OBJS_32_$(d))
$(KERNEL):		LD_SCRIPT	:= $(d)/linker.ld
$(KERNEL):		$(OBJS_32_$(d))

d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
