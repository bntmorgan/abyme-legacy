#!/bin/sh
objdump -D -b binary --adjust-vma=0xffc00000 -mi386 -Maddr32,data32 $1