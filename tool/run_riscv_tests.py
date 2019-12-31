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

import fnmatch
import json
import multiprocessing
import optparse
import os
import subprocess

from functools import reduce
from operator import or_

BinaryDirPath = "./third_party/rafi-prebuilt-binary/riscv-tests/isa"
TraceDirPath = "./work/riscv-tests/trace"

#
# Functions
#
def ReadConfig(json_path, name_filter, build_type):
    with open(options.input_path, "r") as f:
        configs = json.load(f)
        matched = list(filter(lambda x: fnmatch.fnmatch(x['name'], name_filter), configs))

        runnable = list(filter(lambda x: 'skip' not in x or x['skip'] == False, matched))
        skipped = list(filter(lambda x: 'skip' in x and x['skip'] == True, matched))
        unmatched = list(filter(lambda x: not fnmatch.fnmatch(x['name'], name_filter), configs))

        return (runnable, skipped, unmatched)

def GetCheckIoPath(build_type):
    if os.name == "nt":
        return f"./build_{build_type}/{build_type}/rafi-check-io.exe"
    else:
        return f"./build_{build_type}/rafi-check-io"

def GetEmulatorPath(build_type):
    if os.name == "nt":
        return f"./build_{build_type}/{build_type}/rafi-emu.exe"
    else:
        return f"./build_{build_type}/rafi-emu"

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

def VerifyTraces(paths, build_type):
    cmd = [GetCheckIoPath(build_type)]
    cmd.extend(paths)
    PrintCommand("Run", cmd)
    return subprocess.run(cmd).returncode

def RunEmulator(config):
    binary_path = f"{BinaryDirPath}/{config['name']}.bin"
    trace_path = f"{TraceDirPath}/{config['name']}"
    cmd = [
        GetEmulatorPath(config['build_type']),
        "--cycle", str(config['cycle']),
        "--load", f"{binary_path}:0x80000000",
        "--enable-dump-fp-reg",
        "--dump-path", trace_path,
        "--pc", "0x80000000",
        "--host-io-addr", str(config['host-io-addr']),
        "--xlen", str(config['xlen']),
    ]

    PrintCommand("Run", cmd)

    result = subprocess.run(cmd)
    if result.returncode != 0:
        return False # Emulation Failure

def RunTests(configs, build_type):
    for config in configs:
        config['build_type'] = build_type

    with multiprocessing.Pool(multiprocessing.cpu_count()) as p:
        p.map(RunEmulator, configs)

    trace_paths = list(map(lambda config: f"{TraceDirPath}/{config['name']}.tidx", configs))
    exit_code = VerifyTraces(trace_paths, build_type)

    return exit_code

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-d", dest="debug", action="store_true", default=False, help="Use debug build.")
    parser.add_option("-f", dest="filter", default="*", help="Filter test by name.")
    parser.add_option("-i", dest="input_path", default=None, help="Input test list json path.")
    parser.add_option("-l", dest="list_tests", action="store_true", default=False, help="List test names.")

    (options, args) = parser.parse_args()

    if options.input_path is None:
        print("Input test list json is not specified.")
        exit(1)

    build_type = "Debug" if options.debug else "Release"

    (runnable, skipped, _) = ReadConfig(options.input_path, options.filter, build_type)

    if options.list_tests:
        for config in runnable:
            print(config['name'])
        exit(0)

    print("-------------------------------------------------------------")
    print(f"Initialize trace directory ({TraceDirPath})")
    InitializeDirectory(TraceDirPath)

    print("Run test on emulator:")
    exit_code = RunTests(runnable, build_type)

    if len(skipped) > 0:
        print("Skipped tests:")
        for config in skipped:
            print(f"    {config['name']}")

    exit(exit_code)
