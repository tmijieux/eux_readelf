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

#include <assert.h>
#include <stdarg.h>

#include "color.h"
#include "error.h"
#include "eux_readelf_cmdline.h"
#include "symtab.h"
#include "strtab.h"
#include "section.h"
#include "program.h"
#include "i18n.h"
#include "elf.h"
#include "reloc.h"
#include "dyn.h"

static void eux_init_locale_settings()
{
    setlocale(LC_MESSAGES, "");
    setlocale(LC_TIME, "");
    textdomain(DOMAIN);
    bind_textdomain_codeset(DOMAIN, "UTF-8");
}

static void eux64_print_memory(struct gengetopt_args_info *info, Elf64_Ehdr *ehdr)
{
    if (info->file_header_given) {
        eux64_elf_print_header(ehdr);
    }
    Elf64_Phdr *phdr = (Elf64_Phdr*) (((uint8_t*)ehdr) + ehdr->e_phoff);
    Elf64_Shdr *shdr = (Elf64_Shdr*) (((uint8_t*)ehdr) + ehdr->e_shoff);

    if (info->program_headers_given) {
        if ( ehdr->e_phoff != 0 ) {
            eux64_program_print_header(ehdr, phdr,
                                       ehdr->e_phnum, ehdr->e_phentsize);
        } else {
            fprintf(stderr, _("No program header in this file.\n"));
        }
    }

    if (info->section_headers_given) {
        if (ehdr->e_shoff != 0) {
            eux64_section_print_header(ehdr, shdr,
                                       ehdr->e_shnum, ehdr->e_shentsize);
        } else {
            fprintf(stderr, _("No section header in this file.\n"));
        }
    }

    if (info->symbols_given) {
        Elf64_Shdr *symtab_hdr = eux64_sym_get_symtab_hdr(ehdr, shdr);
        Elf64_Shdr *sym_strtab_hdr = eux64_sym_get_strtab_hdr(ehdr, shdr);
        if ( symtab_hdr != NULL ) {
            eux64_sym_print_symtab(ehdr, shdr, symtab_hdr, sym_strtab_hdr);
        } else {
            fprintf(stderr, _("No symbol table in this file.\n"));
        }
    }

    if (info->dyn_syms_given) {
        Elf64_Shdr *dynsym_hdr = eux64_sym_get_dynsym_hdr(ehdr, shdr);
        Elf64_Shdr *dynstr_hdr = eux64_sym_get_dynstr_hdr(ehdr, shdr);
        if ( dynsym_hdr != NULL ) {
            eux64_sym_print_symtab(ehdr, shdr, dynsym_hdr, dynstr_hdr);
        } else {
            fprintf(stderr, _("No dynamic symbol table in this file.\n"));
        }
    }

    if (info->relocs_given) {
        eux64_reloc_print_all_relocations(ehdr, shdr);
    }

    if (info->dynamic_given) {
        eux64_dyn_print_all_tags(ehdr, shdr);
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
        // printf("Elf 64bit!\n");
        eux64_print_memory(info, addr);
    } else {
        eux_error(_("Elf NOCLASS not handled!\n"));
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
