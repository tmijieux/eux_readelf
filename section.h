#ifndef EUX_SECTION_H
#define EUX_SECTION_H

#include <elf.h>

void eux64_section_print_header(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, uint16_t shnum, uint16_t shentsize);
void eux64_section_print_header_entry(Elf64_Ehdr *ehdr, Elf64_Shdr *entry, Elf64_Shdr *shname_strtab);
Elf64_Shdr *eux64_section_lookup_by_name(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *name);
const char *eux64_section_get_name(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *section);

#endif // EUX_SECTION_H
