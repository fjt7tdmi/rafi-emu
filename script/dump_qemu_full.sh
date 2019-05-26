#!/bin/bash

# Usage: before running this script, run qemu with gdbserver!

GDB=riscv64-unknown-elf-gdb
AWK=awk

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

mkdir -p work/gdb
rm work/gdb/* # Remove old log

${GDB} --batch-silent --command=script/dump_qemu_full.gdb

popd
