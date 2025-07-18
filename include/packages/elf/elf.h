// (c) 2008, 2025 Josh Wyant
// Structures and #defines taken from Tool Interface Standard (TIS) Executable and Linking Format (ELF) Specification Version 1.2
// Extended to https://uclibc.org/docs/elf-64-gen.pdf

#pragma once

#include <stdint.h>

/* 64-Bit Data Types */
#define Elf64_Addr void*
#define Elf64_Half uint16_t
#define Elf64_Off uint64_t
#define Elf64_Sword int32_t
#define Elf64_Xword uint64_t
#define Elf64_Sxword int64_t
#define Elf64_Word uint32_t

/* Book I: Executable and Linking Format (ELF) */

/* 32-Bit Data Types */
#define Elf32_Addr void*
#define Elf32_Half uint16_t
#define Elf32_Off uint32_t
#define Elf32_Sword int32_t
#define Elf32_Word uint32_t

/* Object Files */

/* ELF Header */

// e_type
#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOOS 0xFE00
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

// e_machine
#define EM_NONE 0
#define EM_M32 1
#define EM_SPARC 2
#define EM_386 3
#define EM_68K 4
#define EM_88K 5
#define EM_680 7
#define EM_MIPS 8
#define EM_MIPS_RS4_BE 10

// e_version
#define EV_NONE 0
#define EV_CURRENT 1

// ELF Identification

// e_ident[] Identification Indexes
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

// 32-bit
#define EI32_MAG0 0
#define EI32_MAG1 1
#define EI32_MAG2 2
#define EI32_MAG3 3
#define EI32_CLASS 4
#define EI32_DATA 5
#define EI32_VERSION 6
#define EI32_PAD 7
#define EI32_NIDENT 16

// e_ident[EI_MAG0] - e_ident[EI_MAG3]
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFMAG undefined  // symbol for a real magic
#define SELFMAG 4

// e_ident[EI_CLASS]
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

// e_ident[EI_DATA]
// #define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// e_ident[EI_OSABI]
#define ELFOSABI_SYSV 0
#define ELFOSABI_HPUX 1
#define ELFOSABI_STANDALONE 255

// ELF Header
typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

// 32-bit Header
typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

/* Sections */

// Special Section Indexes
#define SHN_UNDEF 0
#define SHN_LORESERVE 0xFF00
#define SHN_LOPROC 0xFF00
#define SHN_HIPROC 0xFF1F
#define SHN_LOOS 0xFF20
#define SHN_HIOS 0xFF3F
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF2
#define SHN_HIRESERVE 0xFFFF

// Section Header
typedef struct {
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;

// 32-bit ection Header
typedef struct {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

// Section Types, sh_type
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOOS 0x60000000
#define SHT_HIOS 0x6fffffff
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
// #define SHT_LOUSER 0x80000000
// #define SHT_HIUSER 0xffffffff

// Section Attribute Flags, sh_flags
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKOS 0x0f000000
#define SHF_MASKPROC 0xf0000000

/* Symbol Table */

#define STN_UNDEF 0

// Symbol Table Entry
typedef struct {
  Elf64_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf64_Half st_shndx;
  Elf64_Addr st_value;
  Elf64_Xword st_size;
} Elf64_Sym;

// 32-bit symbol Table Entry
typedef struct {
  Elf32_Word st_name;
  Elf32_Addr st_value;
  Elf32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half st_shndx;
} Elf32_Sym;

// st_info
#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) & 0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) & 0xf))

// Symbol Binding, ELF32_ST_BIND
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOOS 10
#define STB_HIOS 12
#define STB_LOPROC 13
#define STB_HIPROC 15

// Symbol Types, ELF32_ST_TYPE
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOOS 10
#define STT_HIOS 12
#define STT_LOPROC 13
#define STT_HIPROC 15

/* Relocation */

// Relocation Entries
typedef struct {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
} Elf64_Rel;

typedef struct {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
  Elf64_Sxword r_addend;
} Elf64_Rela;

// 32-bit elocation Entries
typedef struct {
  Elf32_Addr r_offset;
  Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
  Elf32_Addr r_offset;
  Elf32_Word r_info;
  Elf32_Sword r_addend;
} Elf32_Rela;

// r_info
#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))
#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)(t))

/* Program Loading and Dynamic Linking */

/* Program Header */

// Program Header
typedef struct {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

// 32-bit program Header
typedef struct {
  Elf32_Word p_type;
  Elf32_Off p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
} Elf32_Phdr;

// Segment Types, p_type
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6FFFFFFF
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

/* Book II: Processer Specific (Intel Architecture) */
// Relocation Types
#define R_386_NONE 0
#define R_386_32 1
#define R_386_PC32 2

/* Book III: Operating System Specific (UNIX System V Release 4) */

// Segment Flag Bits, p_flags
#define PF_X 1
#define PF_W 2
#define PF_R 4
#define PF_MASKOS 0x00FF0000
#define PF_MASKPROC 0xf0000000

// Dynamic Structure
typedef struct {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;
extern Elf64_Dyn _DYNAMIC[];

// 32-bit dynamic structure
typedef struct {
  Elf32_Sword d_tag;
  union {
    Elf32_Word d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;
// extern Elf32_Dyn _DYNAMIC[];

// Dynamic Array Tags, d_tag
#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PLTRELSZ 2
#define DT_PLTGOT 3
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_INIT 12
#define DT_FINI 13
#define DT_SONAME 14
#define DT_RPATH 15
#define DT_SYMBOLIC 16
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_JMP_REL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_LOOS 0x60000000
#define DT_HIOS 0x6FFFFFFF
#define DT_LOPROC 0x70000000
#define DT_HIPROC 0x7FFFFFFF

/* Appendix A: Intel Architecture and System V Release 4 Dependencies */

// Relocation Types
#define R_386_GOT32 3
#define R_386_PLT32 4
#define R_386_COPY 5
#define R_386_GLOB_DAT 6
#define R_386_JMP_SLOT 7
#define R_386_RELATIVE 8
#define R_386_GOTOFF 9
#define R_386_GOTPC 10

// Global Offset Table
extern Elf32_Addr _GLOBAL_OFFSET_TABLE_[];

/* Relocation types discovered with "readelf" */
#define R_386_32PLT 11
#define R_386_TLS_TPOFF 14
#define R_386_TLS_IE 15
#define R_386_TLS_GOTIE 16
#define R_386_TLS_LE 17
#define R_386_TLS_GD 18
#define R_386_TLS_LDM 19
#define R_386_16 20    // I know what this is
#define R_386_PC16 21  // and this
#define R_386_8 22     //
#define R_386_PC8 23   // These are useful
#define R_386_TLS_GD_32 24
#define R_386_TLS_GD_PUSH 25
#define R_386_TLS_GD_CALL 26
#define R_386_TLS_GD_POP 27
#define R_386_TLS_LDM_32 28
#define R_386_TLS_LDM_PUSH 29
#define R_386_TLS_LDM_CALL 30
#define R_386_TLS_LDM_POP 31
#define R_386_TLS_LDO_32 32
#define R_386_TLS_IE_32 33
#define R_386_TLS_LE_32 34
#define R_386_TLS_DTPMOD32 35
#define R_386_TLS_DTPOFF32 36
#define R_386_TLS_TPOFF32 37
#define R_386_TLS_GOTDESC 39
#define R_386_TLS_DESC_CA 40
#define R_386_TLS_DESC 41

/* Our OS-specific constants */
#define K_ELF_MACHINE EM_386
#define K_ELF_CLASS ELFCLASS32
#define K_ELF_DATA

static unsigned long elf64_hash(const unsigned char* name) {
  unsigned long h = 0, g;
  while (*name) {
    h = (h << 4) + *name++;
    if ((g = h & 0xf0000000))
      h ^= g >> 24;
    h &= 0x0fffffff;
  }
  return h;
}

// #ifdef __cplusplus

// namespace kernel
// {
//     class ElfError : public Error
//     {
//     public:
//         ElfError(const char *msg) : Error(msg) {}
//     }; // class ElfError

//     class SymbolManager
//     {
//     public:
//         SymbolManager(Elf64_Dyn dyn[], void *base)
//             : dynamic(dyn), base(base)
//         {
//             // Get the hash, string, and symbol tables
//             for (int i = 0; dyn[i].d_tag != DT_NULL; i++)
//             {
//                 void *ptr = (void *)((char *)base + dyn[i].d_un.d_val);
//                 switch (dyn[i].d_tag)
//                 {
//                 case DT_SYMTAB:
//                     symtab = (Elf64_Sym *)ptr;
//                     break;
//                 case DT_STRTAB:
//                     strtab = (char *)ptr;
//                     break;
//                 case DT_HASH:
//                     hashtable = (unsigned *)ptr;
//                     break;
//                 }
//             }
//             nbucket = hashtable[0];
//             nchain = hashtable[1];
//             bucket = &hashtable[2];
//             chain = &bucket[nbucket];
//         }
//         Elf64_Sym *find_symbol(const char *name)
//         {
//             // Use the ELF hash table to find the symbol.
//             auto i = bucket[elf_hash((const unsigned char *)name) % nbucket];
//             while (i != SHN_UNDEF)
//             {
//                 if (kstrcmp(strtab + symtab[i].st_name, name) == 0)
//                     return symtab + i;
//                 i = chain[i];
//             }
//             return nullptr;
//         }
//         const char *symbol_name(Elf64_Sym *sym)
//         {
//             return strtab + sym->st_name;
//         }
//         // Calls the function with the given name. Pretty useless, but neat.
//         void invoke(const char *function)
//         {
//             auto s = find_symbol(function);
//             if (s && (Elf64_ST_TYPE(s->st_info) == STT_FUNC))
//                 asm volatile("call *%0" ::"g"(s->st_value));
//             else
//                 // kprintf("Function '%s' not found\n", function);
//                 throw NotFoundError();
//         }

//     private:
//         static unsigned long elf_hash(const unsigned char *name)
//         {
//             unsigned long h = 0, g;
//             while (*name)
//             {
//                 h = (h << 4) + *name++;
//                 if ((g = h & 0xf0000000))
//                     h ^= g >> 24;
//                 h &= ~g;
//             }
//             return h;
//         }
//         void *base;
//         unsigned *hashtable;
//         unsigned nbucket;
//         unsigned nchain;
//         unsigned *bucket;
//         unsigned *chain;
//         Elf64_Sym *symtab;
//         char *strtab;
//         Elf64_Dyn *dynamic;
//     }; // SymbolManager
// } // namespace kernel

// extern int load_driver(
//     std::shared_ptr<kernel::FileSystem> fs,
//     std::shared_ptr<kernel::SymbolManager> symbols,
//     const char *filename);
// #endif // __cplusplus