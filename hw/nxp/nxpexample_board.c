#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "nxpexp.h"
#include "boot.h"
#include "qom/object.h"
#include "hw/boards.h"

struct NXPExampleBoardMachineState {
    /*< private >*/
    MachineState parent_obj;
    /*< public >*/
    NXPEXPMcuState mcu;

};
typedef struct NXPExampleBoardMachineState NXPExampleBoardMachineState;

struct NXPExampleBoardMachineClass {
    /*< private >*/
    MachineClass parent_class;
    /*< public >*/
};
typedef struct NXPExampleBoardMachineClass NXPExampleBoardMachineClass;

#define TYPE_NXPEXAMPLE_BOARD_BASE_MACHINE MACHINE_TYPE_NAME("nxpexample-board-base")

#define TYPE_NXPEXAMPLE_BOARD_MACHINE MACHINE_TYPE_NAME("nxpexample-board")
DECLARE_OBJ_CHECKERS(NXPExampleBoardMachineState, NXPExampleBoardMachineClass,
        NXPEXAMPLE_BOARD_MACHINE, TYPE_NXPEXAMPLE_BOARD_MACHINE)


static void nxpexample_board_init(MachineState *machine)
{
    NXPExampleBoardMachineState* m_state = NXPEXAMPLE_BOARD_MACHINE(machine);

    printf("Setting up board...\n");

    object_initialize_child(OBJECT(machine), "mcu", &m_state->mcu, TYPE_NXPEXPS_MCU);
    sysbus_realize(SYS_BUS_DEVICE(&m_state->mcu), &error_abort);


    printf("Board setup complete\n");
    printf("Loading firmware '%s'...\n", machine->firmware);

    if (machine->firmware) {
        if (!nxp_load_firmware(&m_state->mcu.cpu, machine,
                                 &m_state->mcu.flash, machine->firmware)) {
            exit(1);
        }
    }
}

static void nxpexample_board_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "NXP Example Board";
    mc->alias = "nxpexample-board";
    mc->init = nxpexample_board_init;
    mc->default_cpus = 1;
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    mc->no_parallel = 1;
}

static const TypeInfo nxpexample_board_machine_types[] = {
        {
                .name           = TYPE_NXPEXAMPLE_BOARD_MACHINE,
                .parent         = TYPE_MACHINE,
                .instance_size  = sizeof(NXPExampleBoardMachineState),
                .class_size     = sizeof(NXPExampleBoardMachineClass),
                .class_init     = nxpexample_board_class_init,
        }
};

DEFINE_TYPES(nxpexample_board_machine_types)
