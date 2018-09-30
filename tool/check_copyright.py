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
import functools
import operator

Directories = ["include", "src", "test", "script"]
Extensions = [".cpp", ".h", ".py"]
LinesToRead = 10
KeyWords = ["Copyright", "Akifumi", "Fujita"]

def list_files_to_check(directory_path):
    files_to_check = []
    for root, dirs, files in os.walk(directory_path):
        for name in files:
            path = os.path.join(root, name)
            (_, ext) = os.path.splitext(path)
            if ext in Extensions:
                files_to_check.append(path)
    return files_to_check

def is_copyright_included(path):
    with open(path, "r", encoding='utf8') as f:
        included = False
        for i in range(LinesToRead):
            line = f.readline()
            if line is None:
                break
            if functools.reduce(operator.and_, map(lambda keyword: keyword in line, KeyWords)):
                included = True
                break
        return included

files = []
for directory in Directories:
    path = os.path.join(os.getcwd(), directory)
    files.extend(list_files_to_check(path))

error = 0

for path in files:
    if not is_copyright_included(path):
        print(f"Copyright is not found in '{path}'.'")
        error += 1

if error == 0:
    print(f"No error.")
