#!/bin/bash

if [[ "$RAFI_FREEDOM_U_SDK" = "" ]]; then
    echo "Envvar RAFI_FREEDOM_U_SDK must be set."
    exit 1
fi

AS=riscv64-unknown-elf-as
LD=riscv64-unknown-elf-ld
OBJCOPY=riscv64-unknown-elf-objcopy

QEMU=${RAFI_FREEDOM_U_SDK}/work/riscv-qemu/prefix/bin/qemu-system-riscv64

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

rm -rf fw/work/*
rm -rf work/linux/*

mkdir -p fw/work
mkdir -p work/linux

# dtb
echo "preparing dtb..."
${QEMU} -nographic -machine virt,dumpdtb=./fw/work/qemu.dtb -bios ${RAFI_FREEDOM_U_SDK}/work/riscv-pk/bbl -kernel ${RAFI_FREEDOM_U_SDK}/work/linux/vmlinux -initrd ${RAFI_FREEDOM_U_SDK}/work/initramfs.cpio.gz

# rom
echo "preparing rom..."
${AS} -o fw/work/rom.o -I./fw fw/rom.S
${LD} -o fw/work/rom -Ttext 0x1000 -Tdata 0x1020 fw/work/rom.o
${OBJCOPY} -O binary --set-start=0x1000 --pad-to=0x2000 ./fw/work/rom ./work/linux/rom.bin

# bbl
echo "preparing bbl..."
cp ${RAFI_FREEDOM_U_SDK}/work/riscv-pk/bbl ./work/linux/bbl
${OBJCOPY} -O binary --set-start=0x80000000 --pad-to=0x80200000 ./work/linux/bbl ./work/linux/bbl.bin

# vmlinux
echo "preparing vmlinux..."
${OBJCOPY} --adjust-vma=0x2080200000 ${RAFI_FREEDOM_U_SDK}/work/linux/vmlinux ./work/linux/vmlinux
${OBJCOPY} -O binary --set-start=0x80200000 --pad-to=0x81f00000 ./work/linux/vmlinux ./work/linux/vmlinux.bin

# initramfs
echo "preparing initramfs..."
cp ${RAFI_FREEDOM_U_SDK}/work/initramfs.cpio.gz ./work/linux/initramfs.cpio.gz

popd
