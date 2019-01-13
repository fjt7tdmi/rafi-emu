#!/bin/bash

OBJCOPY=riscv64-unknown-elf-objcopy
RAM_START=0x0
RAM_END=0x8000

IN=/opt/riscv/riscv-tools/riscv-tests/isa
OUT=/share/work

mkdir -p ${OUT}

ELFS=`find ${IN} -type f -executable`

for elf in ${ELFS}; do
    filename=`basename ${elf}`
    ${OBJCOPY} -O binary --set-start=${RAM_START} --pad-to=${RAM_END} ${IN}/${filename} ${OUT}/${filename}
done
