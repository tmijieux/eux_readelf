#include <assert.h>
#include <string.h>
#include "strtab.h"

char *eux64_strtab_get_str(Elf64_Ehdr *eheader, Elf64_Shdr *strtab, uint32_t idx)
{
    assert( strtab->sh_type == SHT_STRTAB );
    return ((char*)eheader) + strtab->sh_offset + idx;
}

/**
 * \brief get the header of the string table named 'name'
 *
 * \param ehdr pointer to elf header
 * \param shdr pointer to section header table
 * \param name name of string table searched for
 */
Elf64_Shdr *eux64_lookup_strtab_hdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *name)
{
    uint16_t shnum = ehdr->e_shnum;
    Elf64_Shdr *sh_strtab_hdr = shdr+ehdr->e_shstrndx;

    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (uint16_t i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr+i;
        if (entry->sh_type == SHT_STRTAB) {
            char *sh_name = eux64_strtab_get_str(ehdr, sh_strtab_hdr, entry->sh_name);
            if ( !strcmp(sh_name, name) ) {
                return entry;
            }
        }
    }
    return NULL;
}
