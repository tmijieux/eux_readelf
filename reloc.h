#ifndef EUX_RELOC_H
#define EUX_RELOC_H

#include <elf.h>

void eux64_reloc_print_all_relocations(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);

#endif // EUX_RELOC_H
