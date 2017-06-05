#ifndef EUX_DYN_H
#define EUX_DYN_H

#include <elf.h>

void eux64_dyn_print_all_tags(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);
void eux64_dyn_print_tags(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *dyn_hdr);
void eux64_dyn_print_tag(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *strtab_hdr, Elf64_Dyn *dyn);

#endif // EUX_DYN_H
