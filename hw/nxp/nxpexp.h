
#ifndef HW_NXP_NXPEXPC_H
#define HW_NXP_NXPEXPC_H

#include "target/nxp/cpu.h"
#include "qom/object.h"
#include "hw/sysbus.h"

#define TYPE_NXPEXP_MCU "NXPEXP"
#define TYPE_NXPEXPS_MCU "NXPEXPS"

typedef struct NXPEXPMcuState NXPEXPMcuState;
DECLARE_INSTANCE_CHECKER(NXPEXPMcuState, NXPEXP_MCU, TYPE_NXPEXP_MCU)

struct NXPEXPMcuState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    NXPACPU cpu;
    MemoryRegion flash;
};

#endif // HW_NXP_NXPEXPC_H