#include <stdio.h>
#include <assert.h>

#include "program.h"
#include "i18n.h"

#define EUX_PHDR_TYPE_LIST(TYPE)                \
    TYPE(NULL)                                  \
    TYPE(LOAD)                                  \
    TYPE(DYNAMIC)                               \
    TYPE(INTERP)                                \
    TYPE(NOTE)                                  \
    TYPE(SHLIB)                                 \
    TYPE(PHDR)                                  \
    TYPE(GNU_EH_FRAME)                          \
    TYPE(GNU_STACK)                             \
    TYPE(GNU_RELRO)                             \

#define EUX_PHDR_CASE_TO_STR(TYPE_)             \
    case PT_##TYPE_: return #TYPE_;             \

static const char *program_hdr_type_name(Elf64_Phdr *entry)
{
    switch ( entry->p_type ) {
        EUX_PHDR_TYPE_LIST(EUX_PHDR_CASE_TO_STR)
    default: return "UNKNOWN";
    }
}

void eux64_program_print_header_entry(Elf64_Ehdr *ehdr, Elf64_Phdr *entry)
{
    printf("  %-14s %016lx %016lx %016lx\n"
           "%-16s %016lx %016lx %c%c%c   0x%lx",
           program_hdr_type_name(entry),
           entry->p_offset, entry->p_vaddr, entry->p_paddr,
           "", entry->p_filesz, entry->p_memsz,
           ((entry->p_flags & PF_R) ? 'R' : ' '),
           ((entry->p_flags & PF_W) ? 'W' : ' '),
           ((entry->p_flags & PF_X) ? 'X' : ' '),
           entry->p_align);
    if (entry->p_type == PT_INTERP) {
        char *interp = (((char*)ehdr) + entry->p_offset);
        printf(_("\n\t [Required program interpreter %s]"), interp);
    }
    printf("\n\n");
}

void eux64_program_print_header(Elf64_Ehdr *ehdr, Elf64_Phdr *phdr,
                                uint16_t phnum, uint16_t phentsize)
{
    Elf64_Phdr *entry;
    assert( phentsize == sizeof(*entry) );
    printf(_("Program header table:\n"));
    printf("  %-14s %-16s %-16s %-16s\n"
           "%-16s %-16s %-16s %s %s\n",
           "Type", "File offset", "Virt Addr", "Phys Addr",
           "", "FileSize", "MemSize", "Flags", "Align");
    for (uint16_t i = 0; i < phnum; ++i) {
        entry = &phdr[i];
        eux64_program_print_header_entry(ehdr, entry);
    }
    printf("\n");
}
