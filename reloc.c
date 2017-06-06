#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "reloc.h"
#include "strtab.h"
#include "i18n.h"
#include "symtab.h"
#include "section.h"

#define EUX_AMD_X86_64_RELOC_TYPE_LIST(RELOC)                           \
    RELOC(NONE           ,0   , "No reloc" )                            \
    RELOC(64             ,1   , "Direct 64 bit " )                      \
    RELOC(PC32           ,2   , "PC relative 32 bit signed" )           \
    RELOC(GOT32          ,3   , "32 bit GOT entry" )                    \
    RELOC(PLT32          ,4   , "32 bit PLT address" )                  \
    RELOC(COPY           ,5   , "Copy symbol at runtime" )              \
    RELOC(GLOB_DAT       ,6   , "Create GOT entry" )                    \
    RELOC(JUMP_SLOT      ,7   , "Create PLT entry" )                    \
    RELOC(RELATIVE       ,8   , "Adjust by program base" )              \
    RELOC(GOTPCREL       ,9   , "32 bit signed PC relative offset to GOT" ) \
    RELOC(32             ,10  , "Direct 32 bit zero extended" )         \
    RELOC(32S            ,11  , "Direct 32 bit sign extended" )         \
    RELOC(16             ,12  , "Direct 16 bit zero extended" )         \
    RELOC(PC16           ,13  , "16 bit sign extended pc relative" )    \
    RELOC(8              ,14  , "Direct 8 bit sign extended " )         \
    RELOC(PC8            ,15  , "8 bit sign extended pc relative" )     \
    RELOC(DTPMOD64       ,16  , "ID of module containing symbol" )      \
    RELOC(DTPOFF64       ,17  , "Offset in module's TLS block" )        \
    RELOC(TPOFF64        ,18  , "Offset in initial TLS block" )         \
    RELOC(TLSGD          ,19  , "32 bit signed PC relative offset to two GOT entries for GD symbol" ) \
    RELOC(TLSLD          ,20  , "32 bit signed PC relative offset to two GOT entries for LD symbol" ) \
    RELOC(DTPOFF32       ,21  , "Offset in TLS block" )                 \
    RELOC(GOTTPOFF       ,22  , "32 bit signed PC relative offset to GOT entry for IE symbol" ) \
    RELOC(TPOFF32        ,23  , "Offset in initial TLS block" )         \
    RELOC(PC64           ,24  , "PC relative 64 bit" )                  \
    RELOC(GOTOFF64       ,25  , "64 bit offset to GOT" )                \
    RELOC(GOTPC32        ,26  , "32 bit signed pc relative offset to GOT" ) \
    RELOC(GOT64          ,27  , "64-bit GOT entry offset" )             \
    RELOC(GOTPCREL64     ,28  , "64-bit PC relative offset to GOT entry" ) \
    RELOC(GOTPC64        ,29  , "64-bit PC relative offset to GOT" )    \
    RELOC(GOTPLT64       ,30  , "like GOT64, says PLT entry needed" )   \
    RELOC(PLTOFF64       ,31  , "64-bit GOT relative offset to PLT entry" ) \
    RELOC(SIZE32         ,32  , "Size of symbol plus 32-bit addend" )   \
    RELOC(SIZE64         ,33  , "Size of symbol plus 64-bit addend" )   \
    RELOC(GOTPC32_TLSDESC,34  , "GOT offset for TLS descriptor. " )     \
    RELOC(TLSDESC_CALL   ,35  , "Marker for call through TLS descriptor. " ) \
    RELOC(TLSDESC        ,36  , "TLS descriptor. " )                    \
    RELOC(IRELATIVE      ,37  , "Adjust indirectly by program base" )   \
    RELOC(RELATIVE64     ,38  , "64-bit adjust by program base." )      \
    /* 39 Reserved was PC32_BND. */                                     \
    /* 40 Reserved was PLT32_BND */                                     \
    RELOC(GOTPCRELX      ,41  , "Load from 32 bit signed pc relative offset to GOT" \
          " entry without REX prefix, relaxable. " )                    \
    RELOC(REX_GOTPCRELX  ,42  , "Load from 32 bit signed pc relative offset to GOT" \
          " entry with REX prefix, relaxable. " )                       \


#define EUX_RELOC_TYPE_CASE_TO_STR(TYPE_, VALUE_, DESC_)        \
    case R_X86_64_##TYPE_: return #TYPE_;                       \

static const char *reloc_type_name(uint64_t type)
{
    switch ( type ) {
        EUX_AMD_X86_64_RELOC_TYPE_LIST(EUX_RELOC_TYPE_CASE_TO_STR)
    default: return "UNKNOWN";
    }
}

void eux64_reloc_print_rela_entry(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Rela *entry)
{
    Elf64_Shdr *symtab = eux64_sym_get_symtab_hdr(ehdr, shdr);
    Elf64_Shdr *strtab = eux64_sym_get_strtab_hdr(ehdr, shdr);

    Elf64_Sym *symbol = ((Elf64_Sym*)(((uint8_t*)ehdr)+symtab->sh_offset)
                         + ELF64_R_SYM(entry->r_info));
    const char *sym_name = eux64_strtab_get_str(ehdr, strtab, symbol->st_name);
    printf("%012lx %012lx %-15s %016lx '%s' %s %lx\n",
           entry->r_offset, entry->r_info,
           reloc_type_name(ELF64_R_TYPE(entry->r_info)),
           symbol->st_value, sym_name,
           (entry->r_addend >= 0
            ? "+"
            : "-" ),
           (entry->r_addend >= 0
            ? entry->r_addend
            : -entry->r_addend));
}

void eux64_reloc_print_rel_entry(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Rel *entry)
{
    Elf64_Shdr *symtab = eux64_sym_get_symtab_hdr(ehdr, shdr);
    Elf64_Shdr *strtab = eux64_sym_get_strtab_hdr(ehdr, shdr);

    Elf64_Sym *symbol = ((Elf64_Sym*)(((uint8_t*)ehdr)+symtab->sh_offset)
                         + ELF64_R_SYM(entry->r_info));
    const char *sym_name = eux64_strtab_get_str(ehdr, strtab, symbol->st_name);
    printf("%012lx %012lx %-15lu %016lx '%s'\n",
           entry->r_offset, entry->r_info,
           ELF64_R_TYPE(entry->r_info),
           symbol->st_value, sym_name);
}

void eux64_reloc_print_relocations(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr *rela_hdr)
{
    const char *relhdr_name = eux64_section_get_name(ehdr, shdr, rela_hdr);

    uint64_t rela_num = rela_hdr->sh_size / rela_hdr->sh_entsize;
    printf(_("Section '%s' (%lu entries):\n"), relhdr_name, rela_num);

    if ( rela_hdr->sh_type == SHT_RELA ) {
        Elf64_Rela *rela = (Elf64_Rela*)  (((uint8_t*)ehdr)+rela_hdr->sh_offset);
        printf("%-12s %-12s %-15s %-16s %s\n",
               "Offset", "Info", "Type", "Symbol value", "Symbol name + addend");
        for (uint64_t i = 0; i < rela_num; ++i) {
            Elf64_Rela *entry = rela + i;
            eux64_reloc_print_rela_entry(ehdr, shdr, entry);
        }
        printf("\n");
    } else if ( rela_hdr->sh_type == SHT_REL ) {
        Elf64_Rel *rel = (Elf64_Rel*)  (((uint8_t*)ehdr)+rela_hdr->sh_offset);
        printf("%-12s %-12s %-15s %-16s %s\n",
               "Offset", "Info", "Type", "Symbol value", "Symbol name");
        for (uint64_t i = 0; i < rela_num; ++i) {
            Elf64_Rel *entry = rel + i;
            eux64_reloc_print_rel_entry(ehdr, shdr, entry);
        }
        printf("\n");
    }
}

void eux64_reloc_print_all_relocations(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
    uint16_t shnum = ehdr->e_shnum;
    if ( shnum == 0 ) {
        shnum = shdr->sh_size;
        assert( shdr->sh_type == SHT_NULL );
        assert( shnum >= SHN_LORESERVE );
    }

    for (uint16_t i = 0; i < shnum; ++i) {
        Elf64_Shdr *entry = shdr + i;
        if ( entry->sh_type != SHT_REL && entry->sh_type != SHT_RELA ) {
            continue;
        }
        eux64_reloc_print_relocations(ehdr, shdr, entry);
    }
}
