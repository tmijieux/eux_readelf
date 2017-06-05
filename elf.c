#include <stdio.h>
#include "error.h"
#include "./elf.h"
#include "i18n.h"

void eux64_elf_print_header_ident(Elf64_Ehdr *ehdr)
{
    printf("Format:                                 ");
    if ( ehdr->e_ident[EI_DATA] == ELFDATA2LSB ) {
        printf(_("Little Endian\n"));
    } else if ( ehdr->e_ident[EI_DATA] == ELFDATA2MSB ) {
        printf(_("Big Endian\n"));
    } else if ( ehdr->e_ident[EI_DATA] == ELFDATA2MSB ) {
        eux_fatal_error(_("Unknown data format\n"));
    }

    printf("VERSION:                                ");
    if ( ehdr->e_ident[EI_VERSION] == EV_NONE ) {
        eux_fatal_error(_("Invalid version\n"));
    } else if ( ehdr->e_ident[EI_DATA] == EV_CURRENT ) {
        printf(_("Current version\n"));
    }

    printf("ABI:                                    ");
    switch ( ehdr->e_ident[EI_OSABI] ) {
    case ELFOSABI_SYSV:       printf("SYSV");       break;
    case ELFOSABI_HPUX:       printf("HPUX");       break;
    case ELFOSABI_NETBSD:     printf("NETBSD");     break;
    case ELFOSABI_LINUX:      printf("LINUX");      break;
    case ELFOSABI_SOLARIS:    printf("SOLARIS");    break;
    case ELFOSABI_IRIX:       printf("IRIX");       break;
    case ELFOSABI_FREEBSD:    printf("FREEBSD");    break;
    case ELFOSABI_TRU64:      printf("TRU64");      break;
    case ELFOSABI_ARM:        printf("ARM");        break;
    case ELFOSABI_STANDALONE: printf("STANDALONE"); break;
    }
    printf("\n");
    printf(_("ABI VERSION:                            %hhu\n"), ehdr->e_ident[EI_ABIVERSION]);
}

void eux64_elf_print_header(Elf64_Ehdr *ehdr)
{
    eux64_elf_print_header_ident(ehdr);

    printf(_("Object type:                            "));
    switch( ehdr->e_type ) {
    default: /* fall-thru */
    case ET_NONE: printf(_("Unknown"));     break;
    case ET_REL:  printf(_("Relocatable")); break;
    case ET_EXEC: printf(_("Executable"));  break;
    case ET_DYN:  printf(_("Shared"));      break;
    case ET_CORE: printf(_("Core file"));   break;
    }
    printf("\n");

    printf(_("Machine type:                           "));
    switch ( ehdr->e_machine ) {
    default: /* fall-thru */
    case EM_NONE       :  printf(_("An unknown machine"));           break;
    case EM_M32        :  printf("AT&T WE 32100");                   break;
    case EM_SPARC      :  printf("Sun Microsystems SPARC");          break;
    case EM_386        :  printf("Intel 80386");                     break;
    case EM_68K        :  printf("Motorola 68000");                  break;
    case EM_88K        :  printf("Motorola 88000");                  break;
    case EM_860        :  printf("Intel 80860");                     break;
    case EM_MIPS       :  printf(_("MIPS RS3000 (big-endian only")); break;
    case EM_PARISC     :  printf("HP/PA");                           break;
    case EM_SPARC32PLUS:  printf(_("SPARC with enhanced instruction set")); break;
    case EM_PPC        :  printf("PowerPC");                         break;
    case EM_PPC64      :  printf("PowerPC 64-bit");                  break;
    case EM_S390       :  printf("IBM S/390");                       break;
    case EM_ARM        :  printf(_("Advanced RISC Machines"));       break;
    case EM_SH         :  printf("Renesas SuperH");                  break;
    case EM_SPARCV9    :  printf("SPARC v9 64-bit");                 break;
    case EM_IA_64      :  printf("Intel Itanium");                   break;
    case EM_X86_64     :  printf("AMD x86-64");                      break;
    case EM_VAX        :  printf("DEC Vax");                         break;
    }
    printf("\n");

    printf(_("Entry point:                            %p\n"), (void*)ehdr->e_entry);
    printf(_("Program header offset:                  %lu\n"), ehdr->e_phoff);
    printf(_("Elf header size:                        %hu\n"), ehdr->e_ehsize);
    printf(_("Section header offset:                  %lu\n"), ehdr->e_shoff);
    printf(_("Program header entry size:              %hu\n"), ehdr->e_phentsize);
    printf(_("Program header entry count:             %hu\n"), ehdr->e_phnum);
    printf(_("Section header entry size:              %hu\n"), ehdr->e_shentsize);
    printf(_("Section header entry count:             %hu\n"), ehdr->e_shnum);
    printf(_("Section header name string table index: %hu\n"), ehdr->e_shstrndx);
    printf("\n");
}
