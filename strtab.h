#ifndef EUX_STRTAB_H
#define EUX_STRTAB_H

#include <libelf.h>

const char *eux64_strtab_get_str(Elf64_Ehdr *eheader, Elf64_Shdr *strtab, uint32_t idx);
void eux64_str_print_table(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *table_hdr);
void eux64_str_print_all_tables(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);

#endif // EUX_STRTAB_H
