#ifndef HW_NXP_BOOT_H
#define HW_NXP_BOOT_H

#include "hw/boards.h"
#include "cpu.h"
#include "elf.h"

/**
 * NXP_load_firmware:   load an image into a memory region
 *
 * @cpu:        Handle a AVR CPU object
 * @ms:         A MachineState
 * @mr:         Memory Region to load into
 * @firmware:   Path to the firmware file (raw binary or ELF format)
 *
 * Load a firmware supplied by the machine or by the user  with the
 * '-bios' command line option, and put it in target memory.
 *
 * Returns: true on success, false on error.
 */
bool nxp_load_firmware(NXPACPU *cpu, MachineState *ms,
                         MemoryRegion *mr, const char *firmware);
bool nxp_load_elf_file(NXPACPU *cpu, const char *filename, MemoryRegion *program_mr);
void nxp_copy_sections(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, MemoryRegion *program_mr);
void nxp_copy_text_section(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, FILE* output);
void nxp_copy_data_section(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, FILE* output);

#endif // HW_NXP_BOOT_H
