#ifndef EUX_STRTAB_H
#define EUX_STRTAB_H

#include <libelf.h>

Elf64_Shdr *eux64_lookup_strtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *name);
char *eux64_strtab_get_str(Elf64_Ehdr *eheader, Elf64_Shdr *strtab, uint32_t idx);


#endif // EUX_STRTAB_H
