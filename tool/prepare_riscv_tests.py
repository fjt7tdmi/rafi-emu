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
import shutil
import sys

ObjcopyCmd = "riscv64-unknown-elf-objcopy"
StartAddr = "0x8000000"
EndAddr = "0x80008000"

#
# Functions
#
def make_objcopy_cmd(in_dir, out_dir, test_type, test_name):
    in_path = os.path.join(in_dir, f"{test_type}/{test_name}")
    out_path = os.path.join(out_dir, f"{test_name}.bin")

    return [
        ObjcopyCmd,
        "-O", "binary",
        f"--set-start={StartAddr}",
        f"--pad-to={EndAddr}",
        in_path,
        out_path,
    ]

def run_objcopy(in_dir, out_dir, test_type, configs):
    for config in configs:
        cmd = make_objcopy_cmd(in_dir, out_dir, test_type, config['name'])
        print(' '.join(cmd))

        subprocess.run(cmd)

#
# Entry point
#
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-c", dest="config_path", default=None, help="Config json path.")
    parser.add_option("-i", dest="in_dir", default=None, help="Input directory path.")
    parser.add_option("-l", dest="list_tests", action="store_true", default=False, help="List test names.")
    parser.add_option("-o", dest="out_dir", default=None, help="Output directory path.")
    parser.add_option("-t", dest="type", default=None, help="Specify 'benchmarks' or 'isa'.")

    (options, args) = parser.parse_args()

    if options.type is None or options.type not in ['benchmarks', 'isa']:
        print("Specify test type. ('benchmarks' or 'isa')")
        exit(1)
    if options.config_path is None:
        print("Input test list json is not specified.")
        exit(1)

    configs = []
    with open(options.config_path, "r") as f:
        configs = json.load(f)

    if options.list_tests:
        for config in configs:
            print(config['name'])
        exit(0)

    run_objcopy(options.in_dir, options.out_dir, options.type, configs)
