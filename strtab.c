#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "strtab.h"
#include "section.h"

const char *eux64_strtab_get_str(Elf64_Ehdr *ehdr, Elf64_Shdr *strtab, uint32_t idx)
{
    assert( strtab->sh_type == SHT_STRTAB );
    return ((const char*)ehdr) + strtab->sh_offset + idx;
}

void eux64_str_print_table(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *table_hdr)
{
    const char *table = (const char*) (((uint8_t*)ehdr)+table_hdr->sh_offset);

    printf("Section '%s':\n", eux64_section_get_name(ehdr, shdr, table_hdr));
    ssize_t size = table_hdr->sh_size;
    const char *tmp = table+1;
    const char *tmp_old = tmp;
    while ( tmp - table < size ) {
        if ( *tmp == '\0' ) {
            printf("%s\n", tmp_old);
            tmp_old = tmp+1;
        }
        ++tmp;
    }
    printf("\n");
}

void eux64_str_print_all_tables(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (uint16_t i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr + i;
        if ( entry->sh_type != SHT_STRTAB ) {
            continue;
        }
        eux64_str_print_table(ehdr, shdr, entry);
    }
}

