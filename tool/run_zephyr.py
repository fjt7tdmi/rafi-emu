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
    DumpPcPath = "./build/Debug/rafi-dump-pc.exe"
    EmulatorPath = "./build/Debug/rafi-emu.exe"
else:
    DumpPcPath = "./build/rafi-dump-pc"
    EmulatorPath = "./build/rafi-emu"

BinaryDirPath = "./work/zephyr"
TraceDirPath = "./work/zephyr/trace"
ZephyrDirPath = os.environ["ZEPHYR_BASE"]

DefaultCycle = 10000

#
# Functions
#
def InitializeDirectory(path):
    os.makedirs(path, exist_ok=True)
    for filename in os.listdir(f"{TraceDirPath}"):
        os.remove(f"{TraceDirPath}/{filename}")

def PrintCommand(cmd):
    print(f"[cmd] {' '.join(cmd)}")

def MakeEmulatorCommand(testname, cycle):
    binary_path = f"{BinaryDirPath}/{testname}.bin"
    trace_path = f"{TraceDirPath}/{testname}.trace.bin"
    return [
        EmulatorPath,
        "--cycle", str(cycle),
        "--binary", binary_path,
        "--dump-path", trace_path,
    ]

def MakeDumpPcCommand(testname):
    trace_path = f"{TraceDirPath}/{testname}.trace.bin"
    return [DumpPcPath, trace_path]

def MakeAddrToLineCommand(testname):
    elf_path = os.path.join(ZephyrDirPath, f"samples/{config['name']}/outdir/qemu_riscv32/zephyr.elf")
    return [
        "riscv64-unknown-elf-addr2line",
        "-e", elf_path,
    ]

def RunEmulator(config):
    cmd = MakeEmulatorCommand(config['name'], config['cycle'])
    PrintCommand(cmd)

    return subprocess.run(cmd).returncode

def RunDumpPc(config):
    out_path = f"{TraceDirPath}/{config['name']}.pc.txt"

    cmd = MakeDumpPcCommand(config['name'])
    PrintCommand(cmd)

    with open(out_path, 'w') as f:
        return subprocess.run(cmd, stdout=f).returncode

def RunAddrToLine(config):
    in_path = f"{TraceDirPath}/{config['name']}.pc.txt"
    out_path = f"{TraceDirPath}/{config['name']}.line.txt"

    cmd = MakeAddrToLineCommand(config['name'])
    PrintCommand(cmd)

    with open(in_path, 'r') as in_file:
        with open(out_path, 'w') as out_file:
            return subprocess.run(cmd, stdin=in_file, stdout=out_file).returncode

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-c", dest="cycle", default=DefaultCycle, help="Number of emulation cycles.")
    parser.add_option("--dump-pc", dest="dump_pc", action="store_true", default=False, help="Enable pc dump.")

    (options, args) = parser.parse_args()

    config = {'name': "philosophers", 'cycle': options.cycle}

    InitializeDirectory(TraceDirPath)

    result = RunEmulator(config)
    if result != 0:
        exit(result)
    
    if not options.dump_pc:
        exit(result)

    result = RunDumpPc(config)
    if result != 0:
        exit(result)

    result = RunAddrToLine(config)
    if result != 0:
        exit(result)
