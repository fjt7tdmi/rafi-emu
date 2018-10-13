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
    CheckIoPath = "./build/Debug/rafi-check-io.exe"
    EmulatorPath = "./build/Debug/rafi-emu.exe"
else:
    CheckIoPath = "./build/rafi-check-io"
    EmulatorPath = "./build/rafi-emu"

BinaryDirPath = "./work/zephyr"
TraceDirPath = "./work/zephyr/trace"

DefaultCycle = 10000

#
# Functions
#
def InitializeDirectory(path):
    os.makedirs(path, exist_ok=True)
    for filename in os.listdir(f"{TraceDirPath}"):
        os.remove(f"{TraceDirPath}/{filename}")

def MakeEmulatorCommand(testname, cycle):
    binary_path = f"{BinaryDirPath}/{testname}.bin"
    trace_path = f"{TraceDirPath}/{testname}.trace.bin"
    return [
        EmulatorPath,
        "--cycle", str(cycle),
        "--binary", binary_path,
        "--dump-path", trace_path,
    ]

def RunEmulator(config):
    cmd = MakeEmulatorCommand(config['name'], config['cycle'])
    print(f"Run {' '.join(cmd)}")

    return subprocess.run(cmd).returncode

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-c", dest="cycle", default=DefaultCycle, help="Number of emulation cycles.")

    (options, args) = parser.parse_args()

    InitializeDirectory(TraceDirPath)

    result = RunEmulator({'name': "philosophers", 'cycle': options.cycle})
    exit(result)
