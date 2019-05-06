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

import optparse
import os
import subprocess

if os.name == "nt":
    DumpPath = "./build/Debug/rafi-dump.exe"
    DumpPcPath = "./build/Debug/rafi-dump-pc.exe"
    EmulatorPath = "./build/Debug/rafi-emu.exe"
else:
    DumpPath = "./build/rafi-dump"
    DumpPcPath = "./build/rafi-dump-pc"
    EmulatorPath = "./build/rafi-emu"

FreedomDirPath = os.environ["RAFI_FREEDOM_U_SDK"]
BinaryDirPath = "./work/linux"
TraceDirPath = "./work/linux/trace"

DefaultCycle = 10000000

#
# Functions
#
def InitializeDirectory(path):
    os.makedirs(path, exist_ok=True)
    for filename in os.listdir(f"{TraceDirPath}"):
        os.remove(f"{TraceDirPath}/{filename}")

def PrintCommand(msg, cmd):
    print(f"{msg} {cmd[0]}")
    if len(cmd) > 1:
        if os.name == "nt":
            print(' '.join(cmd[1:]))
        else:
            args = map(lambda x: f'"{x}"', cmd[1:])
            print(', '.join(args))

def MakeEmulatorCommand(config):
    rom_path = f"{BinaryDirPath}/rom.bin"
    bbl_path = f"{BinaryDirPath}/bbl.bin"
    vmlinux_path = f"{BinaryDirPath}/vmlinux.bin"
    initrd_path = f"{BinaryDirPath}/initramfs.cpio.gz"
    trace_path = f"{TraceDirPath}/linux.trace.bin"

    cmd = [
        EmulatorPath,
        "--cycle", str(config['cycle']),
        "--dump-skip-cycle", str(config['dump_skip_cycle']),
        "--dump-path", trace_path,
        "--load", f"{rom_path}:0x1000",
        "--load", f"{bbl_path}:0x80000000",
        "--load", f"{vmlinux_path}:0x80200000",
        "--load", f"{initrd_path}:0x84000000",
        "--ram-size", str(128 * 1024 * 1024),
        "--pc", "0x1000",
        "--xlen", "64",
    ]
    if config['enable_dump_csr']:
        cmd.append("--enable-dump-csr")
    if config['enable_dump_memory']:
        cmd.append("--enable-dump-memory")
    return cmd

def MakeAddrToLineCommand():
    elf_path = os.path.join(FreedomDirPath, f"work/riscv_pk/bbl")
    return [
        "riscv64-unknown-elf-addr2line",
        "-e", elf_path,
    ]

def RunEmulator(config):
    cmd = MakeEmulatorCommand(config)
    PrintCommand("[cmd]", cmd)

    return subprocess.run(cmd).returncode

def RunDump(config):
    pc_log_path = f"{TraceDirPath}/linux.pc.log"
    gdb_log_path = f"{TraceDirPath}/linux.gdb.log"

    cmd_dump_pc = [ DumpPcPath, "--virtual", f"{TraceDirPath}/linux.trace.bin" ]
    PrintCommand("[cmd]", cmd_dump_pc)
    with open(pc_log_path, 'w') as f:
        subprocess.run(cmd_dump_pc, stdout=f).returncode

    cmd_dump_gdb = [ DumpPath, "--gdb", f"{TraceDirPath}/linux.trace.bin" ]
    PrintCommand("[cmd]", cmd_dump_gdb)
    with open(gdb_log_path, 'w') as f:
        subprocess.run(cmd_dump_gdb, stdout=f).returncode

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-c", dest="cycle", default=DefaultCycle, help="Number of emulation cycles.")
    parser.add_option("--dump", dest="dump", action="store_true", default=False, help="Run rafi-dump after emulation.")
    parser.add_option("--dump-skip-cycle", dest="dump_skip_cycle", default=0, help="Skip dump for specified cycles.")
    parser.add_option("--enable-dump-csr", dest="enable_dump_csr", action="store_true", default=False, help="Enable csr dump.")
    parser.add_option("--enable-dump-memory", dest="enable_dump_memory", action="store_true", default=False, help="Enable memory dump.")

    (options, args) = parser.parse_args()

    config = {
        'cycle': options.cycle,
        'dump_skip_cycle': options.dump_skip_cycle,
        'enable_dump_csr': options.enable_dump_csr,
        'enable_dump_memory': options.enable_dump_memory,
    }

    InitializeDirectory(TraceDirPath)

    result = RunEmulator(config)
    if result != 0:
        exit(result)

    if options.dump:
        RunDump(config)