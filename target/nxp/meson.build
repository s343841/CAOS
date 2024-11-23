gen = [
  decodetree.process('insn.decode', extra_args: [ '--decode', 'decode_insn',
                                                  '--varinsnwidth', '32'  ])
]

nxp_ss = ss.source_set()
nxp_softmmu_ss = ss.source_set()
nxp_ss.add(gen)

nxp_ss.add(files(
  'cpu.c',
  'disas.c',
  'helper.c',
  'helper_conditions.c',
  'helper_elf.c',
  'gdbstub.c',
  'translate.c'
  ))

nxp_softmmu_ss.add(files('machine.c'))

target_arch += {'nxp': nxp_ss}
target_softmmu_arch += {'nxp': nxp_softmmu_ss}
