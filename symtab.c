#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "symtab.h"
#include "strtab.h"


/**
 * \brief get the symbol table header
 * \param ehdr pointer to the elf header
 * \param ehdr pointer to the section header table
 */
Elf64_Shdr *eux64_get_symtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (unsigned i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr+i;
        if (entry->sh_type == SHT_SYMTAB) {
            return entry;
        }
    }
    return NULL;
}

/**
 * \brief get the dynamic symbol table header
 * \param ehdr pointer to the elf header
 * \param ehdr pointer to the section header table
 */
Elf64_Shdr *eux64_get_dynsym_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (unsigned i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr+i;
        if (entry->sh_type == SHT_DYNSYM) {
            return entry;
        }
    }
    return NULL;
}


/**
 * \brief get the header of the string table associated with the (static) symbol table
 *
 * \param ehdr pointer to elf header
 * \param shdr pointer to section header table
  */
Elf64_Shdr *eux64_get_strtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    return eux64_lookup_strtab_hdr(ehdr, shdr, ".strtab");
}

/**
* \brief get the header of the string table associated with the (dynamic) symbol table
*
* \param ehdr pointer to elf header
* \param shdr pointer to section header table
*/
Elf64_Shdr *eux64_get_dynstr_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    return eux64_lookup_strtab_hdr(ehdr, shdr, ".dynstr");
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
void eux64_symtab_print(Elf64_Ehdr *ehdr, Elf64_Shdr *symtab_hdr, Elf64_Shdr *sym_strtab_hdr)
{
    Elf64_Sym *symtab = (Elf64_Sym*) (((uint8_t*)ehdr)+symtab_hdr->sh_offset);
    uint16_t symnum = symtab_hdr->sh_size / symtab_hdr->sh_entsize;
    printf("symnum = %hu\n", symnum);

    for (uint16_t i = 0; i < symnum; ++i) {
        Elf64_Sym *sym = symtab + i;
        if (sym->st_name == 0) // no name
            continue;
        printf("NAME = %s\n", eux64_strtab_get_str(ehdr, sym_strtab_hdr, sym->st_name));
    }
}
