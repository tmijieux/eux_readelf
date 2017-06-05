#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "symtab.h"
#include "strtab.h"
#include "i18n.h"
#include "section.h"

/**
 * \brief get the symbol table header
 * \param ehdr pointer to the elf header
 * \param ehdr pointer to the section header table
 */
Elf64_Shdr *eux64_sym_get_symtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    Elf64_Shdr *symtab = eux64_section_lookup_by_name(ehdr, shdr, ".symtab");
    assert( symtab->sh_type == SHT_SYMTAB );
    return symtab;
}

/**
 * \brief get the dynamic symbol table header
 * \param ehdr pointer to the elf header
 * \param ehdr pointer to the section header table
 */
Elf64_Shdr *eux64_sym_get_dynsym_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    Elf64_Shdr *dynsym = eux64_section_lookup_by_name(ehdr, shdr, ".dynsym");
    assert( dynsym->sh_type == SHT_DYNSYM );
    return dynsym;
}

/**
 * \brief get the header of the string table associated with the (static) symbol table
 *
 * \param ehdr pointer to elf header
 * \param shdr pointer to section header table
 */
Elf64_Shdr *eux64_sym_get_strtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    Elf64_Shdr *strtab = eux64_section_lookup_by_name(ehdr, shdr, ".strtab");
    assert( strtab->sh_type == SHT_STRTAB );
    return strtab;
}

/**
 * \brief get the header of the string table associated with the (dynamic) symbol table
 *
 * \param ehdr pointer to elf header
 * \param shdr pointer to section header table
 */
Elf64_Shdr *eux64_sym_get_dynstr_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    Elf64_Shdr *dynstr = eux64_section_lookup_by_name(ehdr, shdr, ".dynstr");
    assert( dynstr->sh_type == SHT_STRTAB );
    return dynstr;
}


static void eux64_sym_print_symbol(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr,
                                   Elf64_Shdr *sym_strtab_hdr, Elf64_Sym *sym)
{
    (void) shdr;

    const char *name = eux64_strtab_get_str(ehdr, sym_strtab_hdr, sym->st_name);
    printf("%016lx %016lx %08x %08x %s\n",
           sym->st_value, sym->st_size,
           ELF64_ST_BIND(sym->st_info),
           ELF64_ST_TYPE(sym->st_info),
           name  );
}

/**
 * \brief print symbol table
 *
 *  print symbol table pointed to by section table header entry symtab_hdr
 *  associated with symbol string table pointed to by section table header entry sym_strtab_hdr
 *
 * \param ehdr pointer to elf header
 * \param symtab_hdr pointer to section header table entry describing the symbol table
 * \param sym_strtab_hdr pointer to section header table entry describing the string table
 * associated with the symbol table
 */
void eux64_sym_print_symtab(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr,
                            Elf64_Shdr *symtab_hdr, Elf64_Shdr *sym_strtab_hdr)
{
    Elf64_Sym *symtab = (Elf64_Sym*) (((uint8_t*)ehdr)+symtab_hdr->sh_offset);
    uint16_t symnum = symtab_hdr->sh_size / symtab_hdr->sh_entsize;
    printf(_("Section '%s': %hu Entries\n"),
           eux64_section_get_name(ehdr, shdr, symtab_hdr), symnum);

    printf("%-16s %-16s %-8s %-8s %s\n",
           "Value", "Size", "Binding", "Type", "Name");
    for (uint16_t i = 0; i < symnum; ++i) {
        Elf64_Sym *sym = symtab + i;
        eux64_sym_print_symbol(ehdr, shdr, sym_strtab_hdr, sym);
    }
}
