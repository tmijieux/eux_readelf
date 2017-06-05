#include <stdio.h>
#include <assert.h>

#include "dyn.h"
#include "section.h"
#include "strtab.h"

#define EUX_DT_TAG_LIST(TAG )                   \
    TAG(NULL        )                           \
    TAG(NEEDED      )                           \
    TAG(PLTRELSZ    )                           \
    TAG(PLTGOT      )                           \
    TAG(HASH        )                           \
    TAG(STRTAB      )                           \
    TAG(SYMTAB      )                           \
    TAG(RELA        )                           \
    TAG(RELASZ      )                           \
    TAG(RELAENT     )                           \
    TAG(STRSZ       )                           \
    TAG(SYMENT      )                           \
    TAG(INIT        )                           \
    TAG(FINI        )                           \
    TAG(SONAME      )                           \
    TAG(RPATH       )                           \
    TAG(SYMBOLIC    )                           \
    TAG(REL         )                           \
    TAG(RELSZ       )                           \
    TAG(RELENT      )                           \
    TAG(PLTREL      )                           \
    TAG(DEBUG       )                           \
    TAG(TEXTREL     )                           \
    TAG(JMPREL      )                           \
    TAG(BIND_NOW    )                           \
    TAG(RUNPATH     )                           \
    TAG(INIT_ARRAY  )                           \
    TAG(FINI_ARRAY  )                           \
    TAG(INIT_ARRAYSZ)                           \
    TAG(FINI_ARRAYSZ)                           \
    TAG(FLAGS       )                           \
    TAG(FLAGS_1     )                           \
    TAG(GNU_HASH    )                           \
    TAG(VERNEED     )                           \
    TAG(VERNEEDNUM  )                           \
    TAG(VERSYM      )                           \
    TAG(RELACOUNT   )                           \

#define EUX_TAG_TO_CASE_STR(TAG_)               \
    case DT_##TAG_: return "(" #TAG_ ")";       \

static const char *tag_type_name(Elf64_Sxword tag)
{
    switch (tag) {
        EUX_DT_TAG_LIST(EUX_TAG_TO_CASE_STR)
    default: return "Unknown tag ";
    }
}

void eux64_dyn_print_tag(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, const char *strtab, Elf64_Dyn *dyn)
{
    (void) ehdr;
    (void) shdr;

    if (    dyn->d_tag == DT_NEEDED
            || dyn->d_tag == DT_SONAME
            || dyn->d_tag == DT_RUNPATH
            || dyn->d_tag == DT_RPATH ) {
        printf("0x%016lx %-14s [%s]\n",
               dyn->d_tag,
               tag_type_name(dyn->d_tag),
               strtab+dyn->d_un.d_val);
    } else {
        printf("0x%016lx %-14s 0x%lx\n",
               dyn->d_tag,
               tag_type_name(dyn->d_tag),
               dyn->d_un.d_val);
    }
}

void eux64_dyn_print_tags(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *dyn_hdr)
{
    Elf64_Dyn *tag = (Elf64_Dyn*) (((uint8_t*)ehdr)+dyn_hdr->sh_offset);
    Elf64_Dyn *tmp = tag;

    printf("Section '%s':\n", eux64_section_get_name(ehdr, shdr, dyn_hdr));
    printf("%-18s %-14s %s\n", "Tag", "Type", "Name/Value");

    const char *strtab = NULL;
    while (tmp->d_tag != DT_NULL) {
        if ( tmp->d_tag == DT_STRTAB )  {
            strtab = (const char*) (((uint8_t*)ehdr)+tmp->d_un.d_ptr);
        }
        ++tmp;
    }
    assert( strtab != NULL );

    tmp = tag;
    while (tmp->d_tag != DT_NULL) {
        eux64_dyn_print_tag(ehdr, shdr, strtab, tag);
        tmp = tag;
        ++tag;
    }
    printf("\n");
}

void eux64_dyn_print_all_tags(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (uint16_t i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr + i;
        if ( entry->sh_type != SHT_DYNAMIC ) {
            continue;
        }
        eux64_dyn_print_tags(ehdr, shdr, entry);
    }
}
