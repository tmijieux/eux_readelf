#ifndef EUX_PROGRAM_H
#define EUX_PROGRAM_H

#include <libelf.h>
#include <stdint.h>

void eux64_program_print_header_entry(Elf64_Ehdr *ehdr, Elf64_Phdr *entry);
void eux64_program_print_header(Elf64_Ehdr *ehdr, Elf64_Phdr *phdr,
                                uint16_t phnum, uint16_t phentsize);

#endif // EUX_PROGRAM_H
