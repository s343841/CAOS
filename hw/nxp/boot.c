#include "qemu/osdep.h"
#include "qemu/datadir.h"
#include "hw/loader.h"
#include "boot.h"
#include "qemu/error-report.h"
#include "elf.h"
#include <string.h>
#include "hw/core/tcg-cpu-ops.h"
#include "exec/exec-all.h"
#include "target/nxp/helper_elf.h"

void nxp_copy_text_section(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, FILE* output){
    int text_section_idx = -1;
    for(int i= 0; i< e_shnum; i++){
        if(strcmp(&sh_strtable[sh_table[i]->sh_name], ".text") == 0){
            text_section_idx = i;
            break;
        }
    }
    if(text_section_idx >= 0){
        printf("[NXP-BOOT] Text section is at index %i\n", text_section_idx);
    }
    else{
        error_report("[NXP-BOOT] Unable to find .text section!\n");
        exit(1);
    }

    char *buffer = malloc(sh_table[text_section_idx]->sh_size);
    fseek(file, sh_table[text_section_idx]->sh_offset, SEEK_SET);
    int res = fread(buffer, 1, sh_table[text_section_idx]->sh_size, file);
    printf("[NXP-BOOT] Loaded 0x%x bytes from .text section\n", res);
    fwrite(buffer, 1, res, output);
    free(buffer);
}

void nxp_copy_data_section(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, FILE* output){
    int data_section_idx = -1;
    for(int i= 0; i< e_shnum; i++){
        if(strcmp(&sh_strtable[sh_table[i]->sh_name], ".data") == 0){
            data_section_idx = i;
            break;
        }
    }
    if(data_section_idx >= 0){
        printf("[NXP-BOOT] Data section is at index %i\n", data_section_idx);
    }
    else{
        error_report("[NXP-BOOT] Unable to find .data section.\n");
        return;
    }
    if(sh_table[data_section_idx]->sh_size <= 0){
        printf("[NXP-BOOT] Data section has size 0, skipping.\n");
        return;
    }
    int padding_size = sh_table[data_section_idx]->sh_offset - (sh_table[data_section_idx - 1]->sh_offset + sh_table[data_section_idx - 1]->sh_size) - sh_table[data_section_idx]->sh_addr;
    printf("[NXP-BOOT] Data section padding size: 0x%x\n", padding_size);
    char *buffer = malloc(padding_size);
    memset(buffer, 0, padding_size);
    fwrite(buffer, 1, padding_size, output);
    free(buffer);

    buffer = malloc(sh_table[data_section_idx]->sh_size);
    fseek(file, sh_table[data_section_idx]->sh_offset, SEEK_SET);
    int res = fread(buffer, 1, sh_table[data_section_idx]->sh_size, file);
    printf("[NXP-BOOT] Loaded 0x%x bytes from .data section\n", res);
    fwrite(buffer, 1, res, output);
    free(buffer);
}

void nxp_copy_sections(int e_shnum, FILE* file, Elf32_Shdr** sh_table, char *sh_strtable, MemoryRegion *program_mr){
    FILE *output = fopen("/tmp/qemu_nxp_tmp_text_sec", "wb");
    nxp_copy_text_section(e_shnum, file, sh_table, sh_strtable, output);
    nxp_copy_data_section(e_shnum, file, sh_table, sh_strtable, output);
    fclose(output);


    int bytes_loaded = load_image_mr("/tmp/qemu_nxp_tmp_text_sec", program_mr);
    if (bytes_loaded < 0) {
        error_report("[NXP-BOOT] Unable to load firmware image %s as raw binary",
                     "/tmp/qemu_nxp_tmp_text_sec");
        exit(1);
    }
    printf("[NXP-BOOT] Binary data successfully loaded\n");
    remove("/tmp/qemu_nxp_tmp_text_sec");
    printf("[NXP-BOOT] Removed temp firmware file\n");
}

bool nxp_load_elf_file(NXPACPU *cpu, const char *filename, MemoryRegion *program_mr){
    printf("[NXP-BOOT] Loading firmware images as ELF file\n");

    Elf32_Ehdr header;
    FILE* file = fopen(filename, "rb");
    char *sh_strtable = 0;
    Elf32_Shdr** sh_table = 0;
    if(file) {
        // read the header
        int res = fread(&header, 1, sizeof(header), file);

        if(res <= 0){
            error_report("[NXP-BOOT] Cannot read firmware image header\n");
            exit(1);
        }
        // check if it's really an elf file
        if (memcmp(header.e_ident, ELFMAG, SELFMAG) == 0) {
            nxp_convert_elf_header(&header);
            if(header.e_machine == EM_ARM){
                sh_table = malloc(header.e_shnum * sizeof (Elf32_Shdr*));

                nxp_elf_read_section_headers(&header, file, sh_table);
                sh_strtable = (char *)malloc(sh_table[header.e_shstrndx]->sh_size * sizeof(char));

                nxp_elf_read_sh_string_table(&header, file, sh_table, sh_strtable);
                nxp_copy_sections(header.e_shnum, file, sh_table, sh_strtable, program_mr);

            }
            else{
                error_report("[NXP-BOOT] Firmware file is not an NXP file!\n");
            }
        }
        else{
            error_report("[NXP-BOOT] ELF file is not valid!\n");
            exit(1);
        }
    }

    fclose(file);
    free(sh_strtable);
    for(int i= 0; i< header.e_shnum; i++){
        free(sh_table[i]);
    }
    free(sh_table);
    return true;
}

bool nxp_load_firmware(NXPACPU *cpu, MachineState *ms,
                         MemoryRegion *program_mr, const char *firmware)
{
    NXP_FIRMWARE_FILE = qemu_find_file(QEMU_FILE_TYPE_BIOS, firmware);
    if (NXP_FIRMWARE_FILE == NULL) {
        error_report("[NXP-BOOT] Cannot find firmware image '%s'", firmware);
        return false;
    }

    if(nxp_is_elf_file(NXP_FIRMWARE_FILE)){
        //TODO: For some reason QEMU internal ELF-loaders fail to load an NXP Elf file. For now we use a custom elf-loader to extract the .text section from an elf-file.
        nxp_load_elf_file(cpu, NXP_FIRMWARE_FILE, program_mr);
    }
    else{
        printf("[NXP-BOOT]: Loading firmware images as raw binary\n");
        int bytes_loaded = load_image_mr(NXP_FIRMWARE_FILE, program_mr);
        if (bytes_loaded < 0) {
            error_report("[NXP-BOOT] Unable to load firmware image %s as raw binary",
                         firmware);
            return false;
        }
    }
    printf("[NXP-BOOT] Loaded boot image successfully\n");

    return true;
}
