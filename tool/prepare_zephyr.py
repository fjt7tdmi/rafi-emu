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
InDirPath = os.environ["ZEPHYR_BASE"]
OutDirPath = "./work/zephyr"

#
# Functions
#
def InitializeDirectory(path):
    os.makedirs(path, exist_ok=True)
    for filename in os.listdir(f"{path}"):
        os.remove(f"{path}/{filename}")

def MakeObjcopyCommand(config):
    in_path = os.path.join(InDirPath, f"samples/{config['name']}/outdir/qemu_riscv32/zephyr.strip")
    out_path = os.path.join(OutDirPath, f"{config['name']}.bin")

    return [
        ObjcopyCmd,
        "-O", "binary",
        "--remove-section", "vector",
        f"--set-start={StartAddr}",
        f"--pad-to={EndAddr}",
        in_path,
        out_path,
    ]

def RunObjcopy(configs):
    for config in configs:
        cmd = MakeObjcopyCommand(config)
        print(' '.join(cmd))

        subprocess.run(cmd)

#
# Entry point
#
if __name__ == '__main__':
    configs = [
        {'name': "philosophers"}
    ]

    InitializeDirectory(OutDirPath)
    RunObjcopy(configs)
