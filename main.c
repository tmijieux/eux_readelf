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


static void eux64_print_header(Elf64_Ehdr *header)
{
    eux64_print_header_ident(header);

    printf(_("Object type: "));
    switch( header->e_type ) {
    default: /* fall-thru */
    case ET_NONE: printf("Unknown");     break;
    case ET_REL:  printf("Relocatable"); break;
    case ET_EXEC: printf("Executable");  break;
    case ET_DYN:  printf("Shared");      break;
    case ET_CORE: printf("Core file");   break;
    }
    printf("\n");

    printf(_("Machine type: "));
    switch ( header->e_machine ) {
    default: /* fall-thru */
    case EM_NONE       :  printf("An unknown machine"); break;
    case EM_M32        :  printf("AT&T WE 32100"); break;
    case EM_SPARC      :  printf("Sun Microsystems SPARC"); break;
    case EM_386        :  printf("Intel 80386"); break;
    case EM_68K        :  printf("Motorola 68000"); break;
    case EM_88K        :  printf("Motorola 88000"); break;
    case EM_860        :  printf("Intel 80860"); break;
    case EM_MIPS       :  printf("MIPS RS3000 (big-endian only"); break;
    case EM_PARISC     :  printf("HP/PA"); break;
    case EM_SPARC32PLUS:  printf("SPARC with enhanced instruction set"); break;
    case EM_PPC        :  printf("PowerPC"); break;
    case EM_PPC64      :  printf("PowerPC 64-bit"); break;
    case EM_S390       :  printf("IBM S/390"); break;
    case EM_ARM        :  printf("Advanced RISC Machines"); break;
    case EM_SH         :  printf("Renesas SuperH"); break;
    case EM_SPARCV9    :  printf("SPARC v9 64-bit"); break;
    case EM_IA_64      :  printf("Intel Itanium"); break;
    case EM_X86_64     :  printf("AMD x86-64"); break;
    case EM_VAX        :  printf("DEC Vax"); break;
    }
    printf("\n");

    printf("Entry point: %p\n", (void*)header->e_entry);
    printf("Program header offset: %lu\n", header->e_phoff);
    printf("elf header size: %hu\n", header->e_ehsize);
    printf("Section header offset: %lu\n", header->e_shoff);
    printf("Program header entry size: %hu\n", header->e_phentsize);
    printf("Program header entry count: %hu\n", header->e_phnum);
    printf("Section header entry size: %hu\n", header->e_shentsize);
    printf("Section header entry count: %hu\n", header->e_shnum);

    printf("Section header name string table index: %hu\n", header->e_shstrndx);

}

static void eux64_print_program_header_entry(Elf64_Ehdr *eheader, Elf64_Phdr *entry)
{
    switch ( entry->p_type ) {
    case PT_NULL: printf("NULL\n"); break;
    case PT_LOAD: printf("Loadable Segment\n"); break;
    case PT_DYNAMIC: printf("Dynamic linking information\n"); break;
    case PT_INTERP: printf("Interpreter\n"); break;
    case PT_NOTE: printf("Notes\n"); break;
    case PT_SHLIB: printf("Reserved (shlib)\n"); break;
    case PT_PHDR: printf("Program header\n"); break;
    case PT_GNU_STACK: printf("GNU Stack\n"); break;
    default: printf("Other\n"); break;
    }

    if (entry->p_type == PT_INTERP) {
        char *interp = (((char*)eheader) + entry->p_offset);
        printf("Interpreter is '%s'\n", interp);
        printf("Interpreter is '%lu'\n", entry->p_offset);
    }
}

static char *eux64_strtab_get_str(Elf64_Ehdr *eheader, Elf64_Shdr *strtab, uint32_t idx)
{
    assert( strtab->sh_type == SHT_STRTAB );
    return ((char*)eheader) + strtab->sh_offset + idx;
}

static void eux64_print_section_header_entry(
    Elf64_Ehdr *eheader, Elf64_Shdr *entry, Elf64_Shdr *shname_strtab)
{
    char *section_name = eux64_strtab_get_str(eheader, shname_strtab, entry->sh_name);

    printf("%s \t\t(", section_name);
    switch ( entry->sh_type ) {
    case SHT_NULL: printf("NULL"); break;
    case SHT_PROGBITS: printf("Prog bits"); break;
    case SHT_SYMTAB: printf("Symbol table"); break;
    case SHT_STRTAB: printf("String table"); break;
    case SHT_RELA: printf("Relocation entries"); break;
    case SHT_HASH: printf("Symbol hash table"); break;
    case SHT_DYNAMIC: printf("Dynamic section"); break;
    case SHT_NOTE: printf("Notes"); break;
    case SHT_NOBITS: printf("Nobits"); break;
    case SHT_REL: printf("Relocations offsets"); break;
    case SHT_SHLIB: printf("Reserved"); break;
    case SHT_DYNSYM: printf("Dynamic symbols"); break;
    default: printf("Other"); break;
    }
    printf(")\n");

    if ( entry->sh_type == SHT_PROGBITS
         && !strcmp(".interp", section_name)) {
        char *interp = (((char*)eheader)+entry->sh_offset);
        printf("interp='%s'\n", interp);
    }
}

static void eux64_print_program_header(Elf64_Ehdr *eheader, Elf64_Phdr *pheader,
                                       uint16_t phnum, uint16_t phentsize)
{
    Elf64_Phdr *entry;
    assert( phentsize == sizeof(*entry) );

    printf("\n");
    for (uint16_t i = 0; i < phnum; ++i) {
        entry = &pheader[i];
        printf("#%hu: ", i);
        eux64_print_program_header_entry(eheader, entry);
    }
    printf("\n");
}

static void eux64_print_section_header(Elf64_Ehdr *eheader, Elf64_Shdr *sheader,
                                       uint16_t shnum, uint16_t shentsize)
{
    Elf64_Shdr *entry;
    assert( shentsize == sizeof(*entry) );
    Elf64_Shdr *shname_strtab = sheader+eheader->e_shstrndx;
    if ( shnum == 0 ) {
        shnum = sheader->sh_size;
        assert( sheader->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    printf("\n");
    for (uint16_t i = 0; i < shnum; ++i) {
        entry = &sheader[i];
        printf("#%hu: ", i);
        eux64_print_section_header_entry(eheader, entry, shname_strtab);
    }
    printf("\n");
}

static void eux_load_64(Elf64_Ehdr *header)
{
    eux64_print_header(header);
    if (header->e_phoff != 0) {
        eux64_print_program_header(header, (Elf64_Phdr*) (((uint8_t*)header) + header->e_phoff),
                                   header->e_phnum, header->e_phentsize);
    }

    if (header->e_shoff != 0) {
        eux64_print_section_header(header, (Elf64_Shdr*) (((uint8_t*)header) + header->e_shoff),
                                   header->e_shnum, header->e_shentsize);
    }
}

int main(int argc, char *argv[])
{
    eux_set_output_colored(false);
    eux_init_locale_settings();

    if (argc < 2) {
        eux_fatal_error(_("usage: %s elf_filename\n"), argv[0]);
    }

    int err;
    struct stat st;
    err = stat(argv[1], &st);
    if (err != 0) {
        eux_fatal_error("'%s': %s\n", argv[1], strerror(errno));
    }
    if (st.st_size < (long)sizeof(Elf32_Ehdr)) {
        eux_fatal_error(_("Invalid file format: '%s' have not the elf file format\n"), argv[1]);
    }

    int fd = open(argv[1], 0);
    if (fd < 0) {
        eux_fatal_error(_("Cannot open '%s': %s\n"), argv[1], strerror(errno));
    }

    void *addr;
    addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        eux_fatal_error(_("Cannot mmap '%s': %s\n"), argv[1], strerror(errno));
    }

    uint8_t *ptr = addr;
    if (  ptr[EI_MAG0] != ELFMAG0
          || ptr[EI_MAG1] != ELFMAG1
          || ptr[EI_MAG2] != ELFMAG2
          || ptr[EI_MAG3] != ELFMAG3 ) {
        eux_fatal_error(_("Invalid file format: '%s' have not the elf file format\n"), argv[1]);
    }

    if ( ptr[EI_CLASS] == ELFCLASS32 ) {
        eux_fatal_error("Elf 32bit not handled!\n");
    } else if ( ptr[EI_CLASS] == ELFCLASS64 ) {
        printf("Elf 64bit!\n");
        eux_load_64(addr);
    } else {
        eux_fatal_error("Elf NOCLASS not handled!\n");
    }

    err = munmap(addr, st.st_size);
    close(fd);
    if (err != 0) {
        eux_fatal_error(_("Cannot munmap '%s': %s\n"), argv[1], strerror(errno));
    }
    return 0;
}
