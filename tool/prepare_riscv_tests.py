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
import os
import optparse
import subprocess
import sys

ObjcopyCmd = "riscv64-unknown-elf-objcopy"
StartAddr = "0x8000000"
EndAddr = "0x80008000"
InDirPath = os.environ["RISCV_TESTS"]
OutDirPath = "./work/riscv-tests"

#
# Functions
#
def InitializeDirectory(path):
    os.makedirs(path, exist_ok=True)
    for filename in os.listdir(f"{path}"):
        os.remove(f"{path}/{filename}")

def MakeObjcopyCommand(testname):
    in_path = os.path.join(InDirPath, f"build/isa/{testname}")
    out_path = os.path.join(OutDirPath, f"{testname}.bin")

    return [
        ObjcopyCmd,
        "-O", "binary",
        f"--set-start={StartAddr}",
        f"--pad-to={EndAddr}",
        in_path,
        out_path,
    ]

def RunObjcopy(configs):
    for config in configs:
        cmd = MakeObjcopyCommand(config['name'])
        print(' '.join(cmd))

        subprocess.run(cmd)

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-f", dest="filter", default=None, help="Filter test by name.")
    parser.add_option("-i", dest="input_path", default=None, help="Input test list json path.")
    parser.add_option("-l", dest="list_tests", action="store_true", default=False, help="List test names.")

    (options, args) = parser.parse_args()

    if options.input_path is None:
        print("Input test list json is not specified.")
        exit(1)

    configs = []

    with open(options.input_path, "r") as f:
        configs = json.load(f)

    if options.list_tests:
        for config in configs:
            print(config['name'])
        exit(0)

    if options.filter is not None:
        configs = list(filter(lambda config: fnmatch.fnmatch(config['name'], options.filter), configs))

    print("-------------------------------------------------------------")
    InitializeDirectory(OutDirPath)

    print("Run objcopy:")
    RunObjcopy(configs)
