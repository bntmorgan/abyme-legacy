# Generated with make config.py script : DO NOT MODIFY
ui menu.c32
menu title Boot Menu

${for k in pm_kernels :}$
  label protected_$[k[0]]$_$[k[1]]$
  menu label $[k[0]]$ $[k[1]]$ without tinyvisor
  kernel mboot.c32
  append $[boot_directory]$/pm_kernels/$[k[0]]$/$[k[1]]$
${#end}$

${for k in rm_kernels :}$
  label tinyvisor_$[k[0]]$_$[k[1]]$
  menu label $[k[0]]$ $[k[1]]$ with tinyvisor
  kernel mboot.c32
  append $[boot_directory]$/loader/loader.elf32 --- $[boot_directory]$/vmm/vmm.elf32 --- $[boot_directory]$/rm_kernels/$[k[0]]$/$[k[1]]$
${#end}$

label write
menu label Write into mem
COM32 $[boot_directory]$/writemem.c32
append f8000080 30 f80f80d8 00 f106c 00 -- hd1

