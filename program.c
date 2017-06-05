#include <stdio.h>
#include <assert.h>

#include "program.h"
#include "i18n.h"

void eux64_program_print_header_entry(Elf64_Ehdr *ehdr, Elf64_Phdr *entry)
{
    switch ( entry->p_type ) {
    case PT_NULL: printf("NULL\n"); break;
    case PT_LOAD: printf(_("Loadable Segment\n")); break;
    case PT_DYNAMIC: printf(_("Dynamic linking information\n")); break;
    case PT_INTERP: printf(_("Interpreter\n")); break;
    case PT_NOTE: printf(_("Notes\n")); break;
    case PT_SHLIB: printf(_("Reserved (shlib)\n")); break;
    case PT_PHDR: printf(_("Program header\n")); break;
    case PT_GNU_STACK: printf(_("GNU Stack\n")); break;
    default: printf("Other\n"); break;
    }

    if (entry->p_type == PT_INTERP) {
        char *interp = (((char*)ehdr) + entry->p_offset);
        printf(_("Interpreter is '%s'\n"), interp);
    }
}

void eux64_program_print_header(Elf64_Ehdr *ehdr, Elf64_Phdr *phdr,
                                uint16_t phnum, uint16_t phentsize)
{
    Elf64_Phdr *entry;
    assert( phentsize == sizeof(*entry) );

    printf("\n");
    for (uint16_t i = 0; i < phnum; ++i) {
        entry = &phdr[i];
        printf("#%hu: ", i);
        eux64_program_print_header_entry(ehdr, entry);
    }
    printf("\n");
}
