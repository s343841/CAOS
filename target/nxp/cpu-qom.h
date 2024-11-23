#ifndef NXPA_CPU_QOM_H
#define NXPA_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_NXPA_CPU "nxpa-cpu"
#define TYPE_NXPB_CPU "nxpb-cpu"

OBJECT_DECLARE_CPU_TYPE(NXPACPU, NXPACPUClass, NXPA_CPU)

typedef struct NXPACPUDef NXPACPUDef;

/**
 *  NXPACPUClass:
 *  @parent_realize: The parent class' realize handler.
 *  @parent_reset: The parent class' reset handler.
 *
 *  A NXP CPU model.
 */
struct AVR32ACPUClass {
    /*< private >*/
    CPUClass parent_class;

    /*< public >*/
    DeviceRealize parent_realize;
    DeviceReset parent_reset;

    NXPACPUDef* cpu_def;
};

#endif // NXPA_CPU_QOM_H