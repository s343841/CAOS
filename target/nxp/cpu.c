#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/exec-all.h"
#include "cpu.h"
#include "disas/dis-asm.h"
#include "hw/core/tcg-cpu-ops.h"
#include "exec/address-spaces.h"
#include "exec/helper-proto.h"

static NXPACPU * cpu_self;
static bool first_reset = true;

static void nxp_cpu_disas_set_info(CPUState *cpu, disassemble_info *info)
{
    printf("[NXP-DISAS] nxp_cpu_disas_set_info\n");
    info->mach = bfd_arch_nxp;
}

static void nxpa_cpu_init(Object* obj)
{
    CPUState *cs = CPU(obj);
    NXPACPU *cpu = NXPA_CPU(obj);
    CPUNXPAState *env = &cpu->env;

    cpu_set_cpustate_pointers(cpu);
    cs->env_ptr = env;
}


static void nxpb_cpu_init(Object* obj)
{
    // TODO
}

static void nxp_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState* cs = CPU(dev);
    cpu_self = NXPA_CPU(cs);
    NXPACPUClass* acc = NXPA_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    // TODO: Custom CPU setup stuff per CPU core arch

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    acc->parent_realize(dev, errp);
}

static void nxp_cpu_reset(DeviceState *dev)
{
    if(!first_reset) {
        printf("[NXP-CPU] CPU RESET\n");
    }
    CPUState *cs = CPU(dev);
    NXPACPU *cpu = NXPA_CPU(cs);
    NXPACPUClass* acc = NXPA_CPU_GET_CLASS(dev);
    CPUNXPAState* env = &cpu->env;

    env->isInInterrupt = 0;
    env->intlevel = 0;
    env->intsrc = -1;
    acc->parent_reset(dev);

    if(first_reset) {
        memset(env->r, 0, sizeof(env->r));
        memset(env->sflags, 1, sizeof(env->sflags));
        first_reset = false;
    }

    env->sr = 0;

    for(int i= 0; i< 32; i++){
        env->sflags[i] = 0;
    }
    env->sflags[16] = 1;
    env->sflags[21] = 1;
    env->sflags[22] = 1;


    for(int i= 0; i< NXPA_SYS_REG; i++){
        env->sysr[i] = 0;
    }

    for(int i= 0; i< NXPA_REG_PAGE_SIZE; i++){
        env->r[i] = 0;
    }

    printf("RESET 2\n");

    env->r[NXPA_PC_REG] = 0xd0000000;
    env->r[NXPA_LR_REG] = 0;
    env->r[NXPA_SP_REG] = 0;
}

static ObjectClass* nxp_cpu_class_by_name(const char *cpu_model)
{
    ObjectClass *oc;
    printf("[NXP-TODO] nxp_cpu_class_by_name: %s\n", cpu_model);
    oc = object_class_by_name(NXPA_CPU_TYPE_NAME("NXPEXPC"));
    return oc;
}
static bool nxp_cpu_has_work(CPUState *cs)
{
    NXPACPU *cpu = NXPA_CPU(cs);
    CPUNXPAState *env = &cpu->env;

    return (cs->interrupt_request & CPU_INTERRUPT_HARD) &&
           cpu_interrupts_enabled(env);
}

static void nxp_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    NXPACPU *cpu = NXPA_CPU(cs);
    CPUNXPAState *env = &cpu->env;

    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->r[NXPA_PC_REG]);
    qemu_fprintf(f, "SP:    " TARGET_FMT_lx "\n", env->r[NXPA_SP_REG]);
    qemu_fprintf(f, "LR:    " TARGET_FMT_lx "\n", env->r[NXPA_LR_REG]);

    int i;
    for(i = 0;i < NXPA_REG_PAGE_SIZE-3; ++i) {
        qemu_fprintf(f, "r%d:    " TARGET_FMT_lx "\n", i, env->r[i]);
    }

    for(i= 0; i< 32; i++){
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n", nxp_cpu_sr_flag_names[i], env->sflags[i]);
    }

    qemu_fprintf(f, "\n");
}

static void nxp_cpu_set_pc(CPUState *cs, vaddr value)
{
    NXPACPU *cpu = NXPA_CPU(cs);

    printf("[NXP-CPU] nxp_cpu_set_pc, pc: %04lx\n", value);
    cpu->env.r[NXPA_PC_REG] = value;
}

static bool nxp_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    //TODO: Later
    return false;
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps nxp_sysemu_ops = {
        .get_phys_page_debug = nxp_cpu_get_phys_page_debug,
};

static const struct TCGCPUOps nxp_tcg_ops = {
        .initialize = nxp_tcg_init,
        .synchronize_from_tb = nxp_cpu_synchronize_from_tb,
        .cpu_exec_interrupt = nxp_cpu_exec_interrupt,
        .tlb_fill = nxp_cpu_tlb_fill,
        .do_interrupt = nxp_cpu_do_interrupt,
};

void nxp_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb){
    NXPACPU *cpu = NXPA_CPU(cs);
    cpu->env.r[NXPA_PC_REG] = tb->pc;
}

static void nxpa_cpu_class_init(ObjectClass *oc, void *data)
{
    printf("CPU-INIT!\n");
    NXPACPUClass *acc = NXPA_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_set_parent_realize(dc, nxp_cpu_realizefn, &acc->parent_realize);
    device_class_set_parent_reset(dc, nxp_cpu_reset, &acc->parent_reset);

    cc->class_by_name = nxp_cpu_class_by_name;

    cc->has_work = nxp_cpu_has_work;
    cc->dump_state = nxp_cpu_dump_state;
    cc->set_pc = nxp_cpu_set_pc;
    cc->memory_rw_debug = nxp_cpu_memory_rw_debug;
    cc->sysemu_ops = &nxp_sysemu_ops;
    cc->disas_set_info = nxp_cpu_disas_set_info;
    cc->tcg_ops = &nxp_tcg_ops;
    cc->gdb_read_register = nxp_cpu_gdb_read_register;
    cc->gdb_write_register = nxp_cpu_gdb_write_register;
    cc->gdb_adjust_breakpoint = nxp_cpu_gdb_adjust_breakpoint;
    cc->gdb_num_core_regs = 16;
    cc->gdb_core_xml_file = "nxpa-cpu.xml";
}


static void nxpb_cpu_class_init(ObjectClass *oc, void *data)
{
    // TODO
}

static const NXPACPUDef nxp_cpu_defs[] = {
        {
                .name = "NXPEXPC",
                .parent_microarch = TYPE_NXPA_CPU,
                .core_type = NXP_EXP,
                .series_type = NXP_EXP_S,
                .clock_speed = 66 * 1000 * 1000, /* 66 MHz */
                .audio = false,
                .aes = false
        }
};

static void nxp_cpu_cpudef_class_init(ObjectClass *oc, void *data)
{
    NXPACPUClass* acc = NXPA_CPU_CLASS(oc);
    acc->cpu_def = data;
}

static char* nxp_cpu_type_name(const char* model_name)
{
    return g_strdup_printf(NXPA_CPU_TYPE_NAME("%s"), model_name);
}

static void nxp_register_cpudef_type(const struct NXPACPUDef* def)
{
    char* cpu_model_name = nxp_cpu_type_name(def->name);
    TypeInfo ti = {
            .name = cpu_model_name,
            .parent = def->parent_microarch,
            .class_init = nxp_cpu_cpudef_class_init,
            .class_data = (void *)def,
    };

    type_register(&ti);
    g_free(cpu_model_name);
}

static const TypeInfo nxp_cpu_arch_types[] = {
        {
                .name = TYPE_NXPA_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(NXPACPU),
                .instance_init = nxpa_cpu_init,
                .abstract = true,
                .class_size = sizeof(NXPACPUClass),
                .class_init = nxpa_cpu_class_init,
        },
        {
                .name = TYPE_NXPB_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(NXPACPU),
                .instance_init = nxpb_cpu_init,
                .abstract = true,
                .class_size = sizeof(NXPACPUClass),
                .class_init = nxpb_cpu_class_init,
        }
};

static void nxp_cpu_register_types(void)
{
    int i;

    type_register_static_array(nxp_cpu_arch_types, 2);
    for (i = 0; i < ARRAY_SIZE(nxp_cpu_defs); i++) {
        nxp_register_cpudef_type(&nxp_cpu_defs[i]);
    }
}

type_init(nxp_cpu_register_types)
