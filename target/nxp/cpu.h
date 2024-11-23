
#ifndef QEMU_NXPA_CPU_H
#define QEMU_NXPA_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"

#define NXP_EXP 0x100
#define NXP_EXP_S    NXP_EXP | 0x30

#define NXPA_REG_PAGE_SIZE 16 // r0 - r12 + LR + SP + PC
#define NXPA_PC_REG 15
#define NXPA_LR_REG 14
#define NXPA_SP_REG 13
#define NXPA_SYS_REG 256



struct NXPACPUDef {
    const char* name;
    const char* parent_microarch; 
    size_t core_type; 
    size_t series_type;
    size_t clock_speed;
    bool audio; // MCUs with 'AU' suffix
    bool aes;  // MCUs with 'S' suffix
};


#define NXPA_CPU_TYPE_SUFFIX "-" TYPE_NXPA_CPU
#define NXPA_CPU_TYPE_NAME(name) (name NXPA_CPU_TYPE_SUFFIX)
#define CPU_RESOLVING_TYPE TYPE_NXPA_CPU

// Global Interrupt Mask
#define NXP_GM_FLAG(sr)       (sr & 0x10000) >> 16

#define NXP_EXTENDED_INSTR_FORMAT_MASK 0b1110000000000000
#define NXP_EXTENDED_INSTR_FORMAT_MASK_LE 0b11100000

static const char nxp_cpu_r_names[NXPA_REG_PAGE_SIZE][8] = {
        "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
        "r8",  "r9",  "r10",  "r11",  "r12",  "SP",  "LR",  "PC",
};

static const char nxp_cpu_sr_flag_names[32][8] = {
        "sregC", "sregZ", "sregN", "sregV", "sregQ", "sregL",
        "sreg6", "sreg7", "sreg8", "sreg9", "sreg10", "sreg11","sreg12","sreg13",
        "sregT", "sregR",
        "sregGM","sregI0M","sregI1M", "sregI2M", "sregI3M","sregEM","sregM0",
        "sregM1",
        "sregM2", "sreg25","sregD","sregDM","sregJ","sregH", "sreg30", "sregSS"
};

typedef struct CPUArchState {
    // Status Register
    uint sr;
    uint pc_w;

    uint32_t sflags[32];

    // Register File Registers
    uint32_t r[NXPA_REG_PAGE_SIZE]; // 32 bits each

    //System registers
    uint32_t sysr[NXPA_SYS_REG];

    //interrupt source
    int intsrc;
    int intlevel;
    uint64_t autovector;
    int isInInterrupt;

} CPUNXPAState;

struct ArchCPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/

    CPUNegativeOffsetState neg;
    CPUNXPAState env;
};


int nxp_print_insn(bfd_vma addr, disassemble_info *info);


static inline int cpu_interrupts_enabled(CPUNXPAState* env)
{
    return NXP_GM_FLAG(env->sr);
}

static inline int cpu_mmu_index(CPUNXPAState *env, bool ifetch)
{
    // There is only one MMU, so that should be fine
    return 0;
}

static inline void cpu_get_tb_cpu_state(CPUNXPAState *env, target_ulong *pc,
                                        target_ulong *cs_base, uint32_t *pflags)
{
    *pc = env->r[NXPA_PC_REG];
    *cs_base = 0;
    *pflags = 0;
}

void nxp_tcg_init(void);
bool nxp_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                        MMUAccessType access_type, int mmu_idx,
                        bool probe, uintptr_t retaddr);
void nxp_cpu_do_interrupt(CPUState *cpu);
void nxp_cpu_set_int(void *opaque, int irq, int level);
hwaddr nxp_cpu_get_phys_page_debug(CPUState *cs, vaddr addr);
int nxp_cpu_memory_rw_debug(CPUState *cs, vaddr addr, uint8_t *buf, int len, bool is_write);

void nxp_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb);

int nxp_cpu_gdb_read_register(CPUState *cpu, GByteArray *buf, int reg);
int nxp_cpu_gdb_write_register(CPUState *cpu, uint8_t *buf, int reg);
vaddr nxp_cpu_gdb_adjust_breakpoint(CPUState *cpu, vaddr addr);

#include "exec/cpu-all.h"

#endif /* !defined (QEMU_NXPA_CPU_H) */
