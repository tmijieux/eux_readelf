#ifndef EUX_STRTAB_H
#define EUX_STRTAB_H

#include <libelf.h>

const char *eux64_strtab_get_str(Elf64_Ehdr *eheader, Elf64_Shdr *strtab, uint32_t idx);

#endif // EUX_STRTAB_H
