#!/bin/bash

AS=riscv64-unknown-elf-as
LD=riscv64-unknown-elf-ld
OBJCOPY=riscv64-unknown-elf-objcopy

qemu_dtb=third_party/rafi-prebuilt-binary/linux/qemu.dtb
bbl=third_party/rafi-prebuilt-binary/linux/bbl
vmlinux=third_party/rafi-prebuilt-binary/linux/vmlinux
initramfs=third_party/rafi-prebuilt-binary/linux/initramfs.cpio.gz

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc

rm -rf work/riscv-benchmarks/*
rm -rf work/riscv-tests/*
rm -rf work/linux/*

mkdir -p fw/work
mkdir -p work/riscv-benchmarks/*
mkdir -p work/riscv-tests/*
mkdir -p work/linux

# riscv_tests
echo "preparing riscv_tests..."
${RAFI_PYTHON} ./tool/prepare_riscv_tests.py -t isa -i third_party/rafi-prebuilt-binary/riscv-tests -o ./work/riscv-tests -c ./test/riscv_tests.config.json

# riscv_benchmarks
echo "preparing riscv_benchmarks..."
${RAFI_PYTHON} ./tool/prepare_riscv_tests.py -t benchmarks -i third_party/rafi-prebuilt-binary/riscv-tests -o ./work/riscv-benchmarks -c ./test/riscv_benchmarks.config.json

# linux/rom
echo "preparing rom..."
mkdir -p fw/dtb
cp ${qemu_dtb} fw/dtb/qemu.dtb
${AS} -o fw/work/rom.o -I./fw fw/rom.S
${LD} -o fw/work/rom -Ttext 0x1000 -Tdata 0x1020 fw/work/rom.o
${OBJCOPY} -O binary --set-start=0x1000 --pad-to=0x2000 ./fw/work/rom ./work/linux/rom.bin

# linux/bbl
echo "preparing bbl..."
cp ${bbl} ./work/linux/bbl
${OBJCOPY} -O binary --set-start=0x80000000 --pad-to=0x80200000 ./work/linux/bbl ./work/linux/bbl.bin

# linux/vmlinux
echo "preparing vmlinux..."
${OBJCOPY} --adjust-vma=0x2080200000 ${vmlinux} ./work/linux/vmlinux
${OBJCOPY} -O binary --set-start=0x80200000 --pad-to=0x81f00000 ./work/linux/vmlinux ./work/linux/vmlinux.bin

# linux/initramfs
echo "preparing initramfs..."
cp ${initramfs} ./work/linux/initramfs.cpio.gz
gunzip ./work/linux/initramfs.cpio.gz
cp ./work/linux/initramfs.cpio ./work/linux/initramfs.cpio.gz

popd
