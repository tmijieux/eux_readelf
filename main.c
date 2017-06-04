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

int main(int argc, char *argv[])
{
    eux_set_output_colored(true);
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
    if (  ptr[0] != ELFMAG0
          || ptr[1] != ELFMAG1
          || ptr[2] != ELFMAG2
          || ptr[3] != ELFMAG3 ) {
        eux_fatal_error(_("Invalid file format: '%s' have not the elf file format\n"), argv[1]);
    }

    err = munmap(addr, st.st_size);
    close(fd);
    if (err != 0) {
        eux_fatal_error(_("Cannot munmap '%s': %s\n"), argv[1], strerror(errno));
    }
    return 0;
}
