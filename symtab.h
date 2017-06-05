#ifndef EUX_SYMTAB_H
#define EUX_SYMTAB_H

#include <elf.h>

Elf64_Shdr *eux64_get_symtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);
Elf64_Shdr *eux64_get_dynsym_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);

Elf64_Shdr *eux64_get_strtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);
Elf64_Shdr *eux64_get_dynstr_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr);

void eux64_symtab_print(Elf64_Ehdr *ehdr, Elf64_Shdr *symtab_hdr, Elf64_Shdr *sym_strtab_hdr);

#endif // EUX_SYMTAB_H
