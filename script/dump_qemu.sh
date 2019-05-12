#!/bin/bash

# Usage: before running this script, run qemu with gdbserver!

GDB=riscv64-unknown-elf-gdb
AWK=awk

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

mkdir -p work/gdb
rm -f work/gdb/* # Remove old log

${GDB} --batch-silent --command=script/dump_qemu.gdb
cat ./work/gdb/qemu.log | grep pc | ${AWK} '{print $2}' > ./work/gdb/qemu.pc.log

popd
