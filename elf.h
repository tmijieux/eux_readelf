#ifndef EUX_ELF_H
#define EUX_ELF_H

#include <elf.h>

void eux64_elf_print_header_ident(Elf64_Ehdr *header);
void eux64_elf_print_header(Elf64_Ehdr *header);

#endif // EUX_ELF_H
