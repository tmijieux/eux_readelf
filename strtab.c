#include <assert.h>
#include <string.h>
#include "strtab.h"

const char *eux64_strtab_get_str(Elf64_Ehdr *ehdr, Elf64_Shdr *strtab, uint32_t idx)
{
    assert( strtab->sh_type == SHT_STRTAB );
    return ((const char*)ehdr) + strtab->sh_offset + idx;
}
