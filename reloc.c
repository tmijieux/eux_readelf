#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "reloc.h"
#include "strtab.h"
#include "i18n.h"
#include "symtab.h"
#include "section.h"

void eux64_reloc_print_rela_entry(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Rela *entry)
{
    Elf64_Shdr *symtab = eux64_sym_get_symtab_hdr(ehdr, shdr);
    Elf64_Shdr *strtab = eux64_sym_get_strtab_hdr(ehdr, shdr);

    Elf64_Sym *symbol = ((Elf64_Sym*)(((uint8_t*)ehdr)+symtab->sh_offset)
                         + ELF64_R_SYM(entry->r_info));
    const char *sym_name = eux64_strtab_get_str(ehdr, strtab, symbol->st_name);
    printf("%012lx %012lx %-12lu %016lx '%s' %s %lx\n",
           entry->r_offset, entry->r_info,
           ELF64_R_TYPE(entry->r_info),
           symbol->st_value, sym_name,
           (entry->r_addend >= 0
            ? "+"
            : "-" ),
           (entry->r_addend >= 0
            ? entry->r_addend
            : -entry->r_addend));
}

void eux64_reloc_print_rel_entry(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Rel *entry)
{
    Elf64_Shdr *symtab = eux64_sym_get_symtab_hdr(ehdr, shdr);
    Elf64_Shdr *strtab = eux64_sym_get_strtab_hdr(ehdr, shdr);

    Elf64_Sym *symbol = ((Elf64_Sym*)(((uint8_t*)ehdr)+symtab->sh_offset)
                         + ELF64_R_SYM(entry->r_info));
    const char *sym_name = eux64_strtab_get_str(ehdr, strtab, symbol->st_name);
    printf("%012lx %012lx %-12lu %016lx '%s'\n",
           entry->r_offset, entry->r_info,
           ELF64_R_TYPE(entry->r_info),
           symbol->st_value, sym_name);
}

void eux64_reloc_print_relocations(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *rela_hdr)
{
    const char *relhdr_name = eux64_section_get_name(ehdr, shdr, rela_hdr);

    uint64_t rela_num = rela_hdr->sh_size / rela_hdr->sh_entsize;
    printf(_("Section '%s' (%lu entries):\n"), relhdr_name, rela_num);

    if ( rela_hdr->sh_type == SHT_RELA ) {
        Elf64_Rela *rela = (Elf64_Rela*)  (((uint8_t*)ehdr)+rela_hdr->sh_offset);
        printf("%-12s %-12s %-12s %-16s %s\n",
               "Offset", "Info", "Type", "Symbol value", "Symbol name + addend");
        for (uint64_t i = 0; i < rela_num; ++i) {
            Elf64_Rela *entry = rela + i;
            eux64_reloc_print_rela_entry(ehdr, shdr, entry);
        }
        printf("\n");
    } else if ( rela_hdr->sh_type == SHT_REL ) {
        Elf64_Rel *rel = (Elf64_Rel*)  (((uint8_t*)ehdr)+rela_hdr->sh_offset);
        printf("%-12s %-12s %-12s %-16s %s\n",
               "Offset", "Info", "Type", "Symbol value", "Symbol name");
        for (uint64_t i = 0; i < rela_num; ++i) {
            Elf64_Rel *entry = rel + i;
            eux64_reloc_print_rel_entry(ehdr, shdr, entry);
        }
        printf("\n");
    }
}

void eux64_reloc_print_all_relocations(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (uint16_t i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr + i;
        if ( entry->sh_type != SHT_REL && entry->sh_type != SHT_RELA ) {
            continue;
        }
        eux64_reloc_print_relocations(ehdr, shdr, entry);
    }
}
