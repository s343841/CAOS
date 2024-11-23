#include "qemu/osdep.h"
#include "qemu/module.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/qdev-properties.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/misc/unimp.h"
#include "nxpexp.h"

struct NXPEXPMcuClass {
    /*< private >*/
    SysBusDeviceClass parent_class;

    /*< public >*/
    const char *cpu_type;

    size_t flash_size;
};

typedef struct NXPEXPMcuClass NXPEXPMcuClass;

DECLARE_CLASS_CHECKERS(NXPEXPMcuClass, NXPEXP_MCU,
        TYPE_NXPEXP_MCU)

// This functions sets up the device
static void nxpexp_realize(DeviceState *dev, Error **errp)
{
    printf("Realizing...\n");
    NXPEXPMcuState *s = NXPEXP_MCU(dev);
    const NXPEXPMcuClass *mc = NXPEXP_MCU_GET_CLASS(dev);

    /* CPU */
    object_initialize_child(OBJECT(dev), "cpu", &s->cpu, mc->cpu_type);
    object_property_set_bool(OBJECT(&s->cpu), "realized", true, &error_abort);

    /* Flash */
    memory_region_init_rom(&s->flash, OBJECT(dev),
                           "flash", mc->flash_size, &error_fatal);
    memory_region_add_subregion(get_system_memory(),
                                0xd0000000, &s->flash);
}

static void nxpexp_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = nxpexp_realize;
    dc->user_creatable = false;
}

static void nxp2exps_class_init(ObjectClass *oc, void *data){

    NXPEXPMcuClass* nxpexp = NXPEXP_MCU_CLASS(oc);

    nxpexp->cpu_type = NXPA_CPU_TYPE_NAME("NXPEXPC");
    nxpexp->flash_size = 1024 * KiB;
}

static const TypeInfo nxpexp_mcu_types[] = {
        {
                .name           = TYPE_NXPEXPS_MCU,
                .parent         = TYPE_NXPEXP_MCU,
                .class_init     = nxpexps_class_init,
        }, {
                .name           = TYPE_NXPEXP_MCU,
                .parent         = TYPE_SYS_BUS_DEVICE,
                .instance_size  = sizeof(NXPEXPMcuState),
                .class_size     = sizeof(NXPEXPMcuClass),
                .class_init     = nxpexp_class_init,
                .abstract       = true,
        }
};

DEFINE_TYPES(nxpexp_mcu_types)