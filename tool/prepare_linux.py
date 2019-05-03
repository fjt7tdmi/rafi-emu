# Copyright 2018 Akifumi Fujita
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import shutil
import subprocess
import sys

CopyCmd = "cp"
ObjcopyCmd = "riscv64-unknown-elf-objcopy"
InDirPath = os.environ["RAFI_FREEDOM_U_SDK"]
OutDirPath = "./work/linux"
QemuCmd = os.path.join(InDirPath, "work/riscv-qemu/prefix/bin/qemu-system-riscv64")

#
# Functions
#
def init_dir(path):
    shutil.rmtree(path, ignore_errors=True)
    os.makedirs(path)

def copy_bbl():
    in_path = os.path.join(InDirPath, f"work/riscv-pk/bbl")
    out_path = os.path.join(OutDirPath, f"bbl.bin")

    cmd = [
        ObjcopyCmd,
        "-O", "binary",
        f"--set-start=0x80000000",
        f"--pad-to=0x80200000",
        in_path,
        out_path,
    ]

    print(' '.join(cmd))
    subprocess.run(cmd)

def generate_dtb():
    bbl_path = os.path.join(InDirPath, f"work/riscv-pk/bbl")
    vmlinux_path = os.path.join(InDirPath, f"work/linux/vmlinux")
    initrd_path = os.path.join(InDirPath, f"work/initramfs.cpio.gz")
    out_path = os.path.abspath(os.path.join(OutDirPath, f"rafi-emu.dtb"))

    cmd = [
        QemuCmd,
        "-nographic",
        "-machine", f"virt,dumpdtb={out_path}",
        "-bios", bbl_path,
        "-kernel", vmlinux_path,
        "-initrd", initrd_path,
    ]

    print(' '.join(cmd))
    subprocess.run(cmd)

def copy_vmlinux():
    in_path = os.path.join(InDirPath, f"work/linux/vmlinux")
    out_path = os.path.join(OutDirPath, f"vmlinux.bin")

    cmd = [
        ObjcopyCmd,
        "-O", "binary",
        "--adjust-vma=0x2080200000",
        "--set-start=0x80200000",
        "--pad-to=0x81f00000",
        in_path,
        out_path,
    ]

    print(' '.join(cmd))
    subprocess.run(cmd)

def copy_initramfs():
    in_path = os.path.join(InDirPath, f"work/initramfs.cpio.gz")
    out_path = os.path.join(OutDirPath, f"initramfs.cpio.gz")

    cmd = [
        CopyCmd,
        in_path,
        out_path,
    ]

    print(' '.join(cmd))
    subprocess.run(cmd)

#
# Entry point
#
if __name__ == '__main__':
    if "RAFI_FREEDOM_U_SDK" not in os.environ.keys():
        print("Envvar 'RAFI_FREEDOM_U_SDK' is not set.")
        sys.exit(1)
    init_dir(OutDirPath)
    copy_bbl()
    generate_dtb()
    copy_vmlinux()
    copy_initramfs()
