#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "section.h"
#include "strtab.h"
#include "i18n.h"

Elf64_Shdr *eux64_section_lookup_by_name(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *name)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }
    Elf64_Shdr *shname_strtab = shdr + ehdr->e_shstrndx;

    for (unsigned i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr+i;
        const char *sh_name = eux64_strtab_get_str(ehdr, shname_strtab, entry->sh_name);
        if ( !strcmp(sh_name, name) ) {
            return entry;
        }
    }
    return NULL;
}

void eux64_section_print_header_entry(
    Elf64_Ehdr *ehdr, Elf64_Shdr *entry, Elf64_Shdr *shname_strtab)
{
    const char *section_name = eux64_strtab_get_str(ehdr, shname_strtab, entry->sh_name);

    printf("%s \t\t(", section_name);
    switch ( entry->sh_type ) {
    case SHT_NULL:     printf("NULL");                   break;
    case SHT_PROGBITS: printf(_("Prog bits"));           break;
    case SHT_SYMTAB:   printf(_("Symbol table"));        break;
    case SHT_STRTAB:   printf(_("String table"));        break;
    case SHT_RELA:     printf(_("Relocation entries"));  break;
    case SHT_HASH:     printf(_("Symbol hash table"));   break;
    case SHT_DYNAMIC:  printf(_("Dynamic section"));     break;
    case SHT_NOTE:     printf(_("Notes"));               break;
    case SHT_NOBITS:   printf(_("Nobits"));              break;
    case SHT_REL:      printf(_("Relocations offsets")); break;
    case SHT_SHLIB:    printf(_("Reserved"));            break;
    case SHT_DYNSYM:   printf(_("Dynamic symbols"));     break;
    default:           printf(_("Other"));               break;
    }
    printf(")\n");

    if ( entry->sh_type == SHT_PROGBITS
         && !strcmp(".interp", section_name)) {
        char *interp = (((char*)ehdr)+entry->sh_offset);
        printf(_("interpreter is '%s'\n"), interp);
    }
}

void eux64_section_print_header(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr,
                                uint16_t shnum, uint16_t shentsize)
{
    Elf64_Shdr *entry;
    assert( shentsize == sizeof(*entry) );
    Elf64_Shdr *shname_strtab = shdr+ehdr->e_shstrndx;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    printf("\n");
    for (uint16_t i = 0; i < shnum; ++i) {
        entry = &shdr[i];
        printf("#%hu: ", i);
        eux64_section_print_header_entry(ehdr, entry, shname_strtab);
    }
    printf("\n");
}

const char *eux64_section_get_name(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *section)
{
    Elf64_Shdr *shname_strtab = shdr+ehdr->e_shstrndx;
    return eux64_strtab_get_str(ehdr, shname_strtab, section->sh_name);
}
