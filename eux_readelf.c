#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <elf.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <assert.h>
#include <stdarg.h>

#include "color.h"
#include "error.h"
#include "eux_readelf_cmdline.h"
#include "symtab.h"
#include "strtab.h"

#define DOMAIN "eux"
#define _(X) dgettext(DOMAIN, (X))

static void eux_init_locale_settings()
{
    setlocale(LC_MESSAGES, "");
    setlocale(LC_TIME, "");
    textdomain(DOMAIN);
    bind_textdomain_codeset(DOMAIN, "UTF-8");
}

static void eux64_print_header_ident(Elf64_Ehdr *header)
{
    if ( header->e_ident[EI_DATA] == ELFDATA2LSB ) {
        printf(_("Little Endian\n"));
    } else if ( header->e_ident[EI_DATA] == ELFDATA2MSB ) {
        printf(_("Big Endian\n"));
    } else if ( header->e_ident[EI_DATA] == ELFDATA2MSB ) {
        eux_fatal_error(_("Unknown data format\n"));
    }

    if ( header->e_ident[EI_VERSION] == EV_NONE ) {
        eux_fatal_error(_("Invalid version\n"));
    } else if ( header->e_ident[EI_DATA] == EV_CURRENT ) {
        printf(_("Current version\n"));
    }

    printf("ABI: ");
    switch ( header->e_ident[EI_OSABI] ) {
    case ELFOSABI_SYSV: printf("SYSV"); break;
    case ELFOSABI_HPUX: printf("HPUX"); break;
    case ELFOSABI_NETBSD: printf("NETBSD"); break;
    case ELFOSABI_LINUX: printf("LINUX"); break;
    case ELFOSABI_SOLARIS: printf("SOLARIS"); break;
    case ELFOSABI_IRIX: printf("IRIX"); break;
    case ELFOSABI_FREEBSD: printf("FREEBSD"); break;
    case ELFOSABI_TRU64: printf("TRU64"); break;
    case ELFOSABI_ARM: printf("ARM"); break;
    case ELFOSABI_STANDALONE: printf("STANDALONE"); break;
    }
    printf("\n");
    printf(_("ABI VERSION: %hhu\n"), header->e_ident[EI_ABIVERSION]);
}

static void eux64_print_elf_header(Elf64_Ehdr *header)
{
    eux64_print_header_ident(header);

    printf(_("Object type: "));
    switch( header->e_type ) {
    default: /* fall-thru */
    case ET_NONE: printf(_("Unknown"));     break;
    case ET_REL:  printf(_("Relocatable")); break;
    case ET_EXEC: printf(_("Executable"));  break;
    case ET_DYN:  printf(_("Shared"));      break;
    case ET_CORE: printf(_("Core file"));   break;
    }
    printf("\n");

    printf(_("Machine type: "));
    switch ( header->e_machine ) {
    default: /* fall-thru */
    case EM_NONE       :  printf(_("An unknown machine")); break;
    case EM_M32        :  printf("AT&T WE 32100"); break;
    case EM_SPARC      :  printf("Sun Microsystems SPARC"); break;
    case EM_386        :  printf("Intel 80386"); break;
    case EM_68K        :  printf("Motorola 68000"); break;
    case EM_88K        :  printf("Motorola 88000"); break;
    case EM_860        :  printf("Intel 80860"); break;
    case EM_MIPS       :  printf(_("MIPS RS3000 (big-endian only")); break;
    case EM_PARISC     :  printf("HP/PA"); break;
    case EM_SPARC32PLUS:  printf(_("SPARC with enhanced instruction set")); break;
    case EM_PPC        :  printf("PowerPC"); break;
    case EM_PPC64      :  printf("PowerPC 64-bit"); break;
    case EM_S390       :  printf("IBM S/390"); break;
    case EM_ARM        :  printf(_("Advanced RISC Machines")); break;
    case EM_SH         :  printf("Renesas SuperH"); break;
    case EM_SPARCV9    :  printf("SPARC v9 64-bit"); break;
    case EM_IA_64      :  printf("Intel Itanium"); break;
    case EM_X86_64     :  printf("AMD x86-64"); break;
    case EM_VAX        :  printf("DEC Vax"); break;
    }
    printf("\n");

    printf(_("Entry point: %p\n"), (void*)header->e_entry);
    printf(_("Program header offset: %lu\n"), header->e_phoff);
    printf(_("Elf header size: %hu\n"), header->e_ehsize);
    printf(_("Section header offset: %lu\n"), header->e_shoff);
    printf(_("Program header entry size: %hu\n"), header->e_phentsize);
    printf(_("Program header entry count: %hu\n"), header->e_phnum);
    printf(_("Section header entry size: %hu\n"), header->e_shentsize);
    printf(_("Section header entry count: %hu\n"), header->e_shnum);
    printf(_("Section header name string table index: %hu\n"), header->e_shstrndx);

}

static void eux64_print_program_header_entry(Elf64_Ehdr *ehdr, Elf64_Phdr *entry)
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

static void eux64_print_section_header_entry(
    Elf64_Ehdr *ehdr, Elf64_Shdr *entry, Elf64_Shdr *shname_strtab)
{
    char *section_name = eux64_strtab_get_str(ehdr, shname_strtab, entry->sh_name);

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

static void eux64_print_program_header(Elf64_Ehdr *ehdr, Elf64_Phdr *phdr,
                                       uint16_t phnum, uint16_t phentsize)
{
    Elf64_Phdr *entry;
    assert( phentsize == sizeof(*entry) );

    printf("\n");
    for (uint16_t i = 0; i < phnum; ++i) {
        entry = &phdr[i];
        printf("#%hu: ", i);
        eux64_print_program_header_entry(ehdr, entry);
    }
    printf("\n");
}

static void eux64_print_section_header(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr,
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
        eux64_print_section_header_entry(ehdr, entry, shname_strtab);
    }
    printf("\n");
}

static void eux64_print_memory(struct gengetopt_args_info *info, Elf64_Ehdr *ehdr)
{
    if (info->file_header_given) {
        eux64_print_elf_header(ehdr);
    }

    if (info->program_headers_given) {
        if ( ehdr->e_phoff != 0 ) {
            Elf64_Phdr *program_hdr = (Elf64_Phdr*) (((uint8_t*)ehdr) + ehdr->e_phoff);
            eux64_print_program_header(ehdr, program_hdr,
                                       ehdr->e_phnum, ehdr->e_phentsize);
        } else {
            fprintf(stderr, _("No program header in this file.\n"));
        }
    }

    if (info->section_headers_given) {
        if (ehdr->e_shoff != 0) {
            Elf64_Shdr *section_hdr = (Elf64_Shdr*) (((uint8_t*)ehdr) + ehdr->e_shoff);
            eux64_print_section_header(ehdr, section_hdr,
                                       ehdr->e_shnum, ehdr->e_shentsize);
        } else {
            fprintf(stderr, _("No section header in this file.\n"));
        }
    }

    if (info->symbols_given) {
        Elf64_Shdr *shdr = (Elf64_Shdr*) (((uint8_t*)ehdr) + ehdr->e_shoff);
        Elf64_Shdr *symtab_hdr = eux64_get_symtab_hdr(ehdr, shdr);
        Elf64_Shdr *sym_strtab_hdr = eux64_get_strtab_hdr(ehdr, shdr);
        if ( symtab_hdr != NULL ) {
            eux64_symtab_print(ehdr, symtab_hdr, sym_strtab_hdr);
        } else {
            fprintf(stderr, _("No symbol table in this file.\n"));
        }
    }

    if (info->dyn_syms_given) {
        Elf64_Shdr *shdr = (Elf64_Shdr*) (((uint8_t*)ehdr) + ehdr->e_shoff);
        Elf64_Shdr *dynsym_hdr = eux64_get_dynsym_hdr(ehdr, shdr);
        Elf64_Shdr *dynstr_hdr = eux64_get_dynstr_hdr(ehdr, shdr);
        if ( dynsym_hdr != NULL ) {
            eux64_symtab_print(ehdr, dynsym_hdr, dynstr_hdr);
        } else {
            fprintf(stderr, _("No dynamic symbol table in this file.\n"));
        }
    }

}

static void eux_print_filename(struct gengetopt_args_info *info, const char *filename)
{
    int err;
    struct stat st;
    err = stat(filename, &st);
    if (err != 0) {
        eux_error("'%s': %s\n", filename, strerror(errno));
        return;
    }
    if (st.st_size < (long)sizeof(Elf32_Ehdr)) {
        eux_fatal_error(_("Invalid file format: '%s' have not the ELF file format\n"), filename);
    }

    int fd = open(filename, 0);
    if (fd < 0) {
        eux_fatal_error(_("Cannot open '%s': %s\n"), filename, strerror(errno));
    }

    void *addr;
    addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        eux_fatal_error(_("Cannot mmap '%s': %s\n"), filename, strerror(errno));
    }
    close(fd);

    uint8_t *ptr = addr;
    if (  ptr[EI_MAG0] != ELFMAG0
          || ptr[EI_MAG1] != ELFMAG1
          || ptr[EI_MAG2] != ELFMAG2
          || ptr[EI_MAG3] != ELFMAG3 ) {
        eux_fatal_error(_("Invalid file format: '%s' (Invalid ELF magic bytes)\n"), filename);
    }

    if ( ptr[EI_CLASS] == ELFCLASS32 ) {
        eux_error(_("Elf 32bit not handled!\n"));
        return;
    } else if ( ptr[EI_CLASS] == ELFCLASS64 ) {
        printf("Elf 64bit!\n");
        eux64_print_memory(info, addr);
    } else {
        eux_error("Elf NOCLASS not handled!\n");
        return;
    }

    err = munmap(addr, st.st_size);
    if (err != 0) {
        eux_fatal_error(_("Cannot munmap '%s': %s\n"), filename, strerror(errno));
    }
}

static void update_args_info(struct gengetopt_args_info *info)
{
    if (info->all_given) {
        info->headers_given = 1;
        info->symbols_given = 1;
        info->relocs_given = 1;
        info->dynamic_given = 1;
    }

    if (info->headers_given) {
        info->file_header_given = 1;
        info->program_headers_given = 1;
        info->section_headers_given = 1;
    }
}

int main(int argc, char *argv[])
{
    eux_init_locale_settings();

    struct gengetopt_args_info info;
    cmdline_parser(argc, argv, &info);
    update_args_info(&info);

    if (info.help_given || info.inputs_num == 0) {
        cmdline_parser_print_help();
        exit(EXIT_FAILURE);
    } else if (info.version_given) {
        cmdline_parser_print_version();
        exit(EXIT_SUCCESS);
    }
    for (unsigned i = 0; i < info.inputs_num; ++i) {
        eux_print_filename(&info, info.inputs[i]);
    }
    return 0;
}
